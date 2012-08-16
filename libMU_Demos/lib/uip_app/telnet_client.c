/**
 * \addtogroup apps
 * @{
 */

/**
 * \defgroup telnet_client Telnet client
 * @{
 *
 * This example shows a simplified telent client that is able to download web pages
 * and files from web servers. It requires a number of callback
 * functions to be implemented by the module that utilizes the code:
 * telnet_client_datahandler(), telnet_client_connected(),
 * telnet_client_timedout(), telnet_client_aborted(), telnet_client_closed().
 */

/**
 * \file
 * Implementation of the HTTP client.
 * \author Eñaut Muxika <emuxika@eps.mondragon.edu>
 */

#include <string.h>
#include <uip/uip.h>
#include <uip/uiplib.h>
#include "network_state.h"
#include "telnet_client.h"

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

/************************************************/
/* Constants									*/
/************************************************/
#define	TELNET_CLIENT_TIMEOUT	20

/* First data sent by telnet server
 * ff fd 01 ff fd 1f ff fd 21 ff fb 01 ff fb 03
 */

#define	SETUP1_CMD_LEN	3
const char	setup1_cmd[] = {
	0xff, 0xfb, 0x01
};

#define	SETUP2_CMD_LEN	21
const char	setup2_cmd[] = {
	0xff, 0xfb, 0x1f, 	// Will negotiate
	0xff, 0xfa, 0x1f, 0x00, 0x6e, 0x00, 0x2e, 
	0xff, 0xf0, 		// suboption end
	0xff, 0xfc, 0x21, 
	0xff, 0xfd, 0x01, 
	0xff, 0xfd, 0x03
};

/************************************************/
/* Global variables								*/
/************************************************/
		telnet_client_state	s;
static 	xSemaphoreHandle	Sem_Telnet = NULL;
		frame_stats_t		telnet_stats;

/************************************************/
/* FIFO Macros									*/
/************************************************/
#define	CMD_FIFO_CLEAR(x)	{ (x).count = (x).first = (x).last = 0; }
#define	CMD_FIFO_COUNT(x)	( (x).count )
#define	CMD_FIFO_FULL(x)	( (x).count >= (CMD_FIFO_SIZE-1) ) 
#define	CMD_FIFO_EMPTY(x)	( (x).count == 0 ) 
#define	CMD_FIFO_BACK(x)	((x).fifo[ (x).last ])
#define	CMD_FIFO_FRONT(x)	((x).fifo[ (x).first ])

#define	CMD_FIFO_FIRST(x)	{								\
		(x).first++; (x).count--; 							\
		if( (x).first >= CMD_FIFO_SIZE ) (x).first = 0;  	}

#define	CMD_FIFO_LAST(x)	{								\
		(x).last++; (x).count++;							\
		if( (x).last >= CMD_FIFO_SIZE ) (x).last = 0;  		}	

#define	CMD_FIFO_GET(x,e)		{ 	\
 		(e) = CMD_FIFO_FRONT(x);	\
 		CMD_FIFO_FIRST(x); 		}

#define	CMD_FIFO_PUT(x,e)		{ 	\
 		CMD_FIFO_BACK(x) = (e);		\
 		CMD_FIFO_LAST(x); 		}
 		
#define	CMD_CUR		CMD_FIFO_FRONT( s.cmd_fifo )

/************************************************/
/* Function implementations						*/
/************************************************/

/*-----------------------------------------------------------------------------------*/
uip_ipaddr_t*
telnet_client_server(void)
{
  return &s.server;
}
/*-----------------------------------------------------------------------------------*/
u16_t
telnet_client_port(void)
{
  return s.port;
}
/*-----------------------------------------------------------------------------------*/
void
telnet_client_init(void)
{
	s.state = TN_DISCONNECTED;
    CMD_FIFO_CLEAR( s.cmd_fifo );
	
	/* We are using the semaphore for synchronisation so we create a binary
    semaphore rather than a mutex.  We must make sure that the interrupt
    does not attempt to use the semaphore before it is created! */
    vSemaphoreCreateBinary( Sem_Telnet );
    
    telnet_stats.recv = telnet_stats.sent = 0;    
}
/*-----------------------------------------------------------------------------------*/
u8_t 
telnet_client_open(uip_ipaddr_t *host, u16_t port)
{
	if( s.state == TN_CONNECTED ) return 0;
	struct uip_conn *conn = uip_connect( host, htons(port) );
    if( conn == NULL ) return 0;
	s.port = port;
  	uip_ipaddr_copy( &s.server, host );
  	s.state = TN_DISCONNECTED;
  	CMD_FIFO_CLEAR( s.cmd_fifo );
  	s.resp = NULL; s.retries = 0; s.bEcho = 1;
  	telnet_client_cmd_int( setup1_cmd, sizeof(setup1_cmd), NULL, 0 );
  	telnet_client_cmd_int( setup2_cmd, sizeof(setup2_cmd), NULL, 0 );
	return 1;
}
/*-----------------------------------------------------------------------------------*/
char*
telnet_client_resp(void)
{
	return s.resp;
}

/*-----------------------------------------------------------------------------------*/
static void 
senddata(void)
{
    if( s.state == TN_CONNECTED && CMD_FIFO_COUNT( s.cmd_fifo ) > 0 ) {
    	telnet_stats.sent++; 
		uip_send( CMD_CUR.cmd, CMD_CUR.cmd_size ); s.bCmdSent++; s.cmd_info = &CMD_CUR;
		if( CMD_CUR.cmd[0] != 0xFF ) {  
			if(s.bEcho)	s.state = TN_PENDING_ECHO;
			else		s.state = TN_PENDING_RESP;
		}
		s.resp = NULL;
	}
}
/*-----------------------------------------------------------------------------------*/
static void 
acked(void)
{
	// correctly received
	CMD_FIFO_FIRST( s.cmd_fifo ); s.bCmdSent--;
}
/*-----------------------------------------------------------------------------------*/
static void 
newdata(void)
{
	telnet_stats.recv++;
	
	s.resp = (char*)uip_appdata;
//	Request.is_write = 0;
//	Request.num_bytes = uip_len;
//	if( Request.num_bytes > sizeof( Request.data ) ) Request.num_bytes = sizeof( Request.data );
//	memcpy( Request.data, s.resp, Request.num_bytes );
//	
//	if( Request.num_bytes < 0 ) network_state = NETWORK_ERROR; 
		
	if( s.resp[0] == 0xFF ) { // configuration commands
		return;
	}
	switch( s.state ) {
		case TN_PENDING_ECHO:
			if( strncmp( s.resp, s.cmd_info->cmd, s.cmd_info->cmd_size ) != 0 ) {
				//s.state = TN_CONNECTED; 
				s.retries++;
				if( s.retries >= 3 ) {
					uip_abort();
				} 
				return;
			}
			s.retries = 0;				// Echo correctly received
			s.state = TN_PENDING_RESP;	// Wait for response
			break;
		case TN_PENDING_RESP:
			if( s.cmd_info->resp != NULL && s.cmd_info->resp_size > 0 ) {
				if( uip_len < s.cmd_info->resp_size ) { 
					memcpy( s.cmd_info->resp, s.resp, uip_len );
				}else{			
					memcpy( s.cmd_info->resp, s.resp, s.cmd_info->resp_size );
				}
			}
			s.state = TN_CONNECTED; s.bCmdSent = 0;
			s.bRespReceived = 1;
	        static portBASE_TYPE xHigherPriorityTaskWoken;
			xSemaphoreGiveFromISR( Sem_Telnet, &xHigherPriorityTaskWoken );
			break;
		default:
			s.state = TN_CONNECTED;
			// Ignore other results
			return;
	}
}
/*-----------------------------------------------------------------------------------*/
void
telnet_client_appcall(void)
{
	telnet_stats.frames++;

	if( uip_connected() ) {
		s.timer = 0;
		s.state = TN_CONNECTED;
		//senddata();
		return;
	}
	if( s.state == TN_CLOSE ) {
		s.state = TN_DISCONNECTED;
		uip_abort();
		return;
	}
	if( uip_aborted() ) {
		s.state = TN_DISCONNECTED;
		return;
	}
	if( uip_acked() ) {
		s.timer = 0;
		acked();
	}
	if( uip_newdata() ) {
		s.timer = 0;
		newdata();
	}
	if( uip_rexmit() ||
		uip_newdata() ||
		uip_acked()) {
		senddata();
	} else if( uip_poll() ) {
		senddata();
		++s.timer;
		if( s.timer == TELNET_CLIENT_TIMEOUT ) {
//			telnet_client_timedout();
			uip_abort();
			return;
		}
	}
	if(uip_closed()) {
  		s.state = TN_DISCONNECTED;
	}
}
/*-----------------------------------------------------------------------------------*/
u8_t 
telnet_client_cmd_int (	const char *cmd, size_t len, 
						char *resp, size_t resp_max_size )
{
	if( len <= 0 || CMD_FIFO_FULL( s.cmd_fifo ) ) return 0;
	
	xSemaphoreTake( Sem_Telnet, 0 );
	CMD_FIFO_BACK( s.cmd_fifo ).cmd = (char*)cmd;
	CMD_FIFO_BACK( s.cmd_fifo ).cmd_size = len;
	CMD_FIFO_BACK( s.cmd_fifo ).resp = resp;
	CMD_FIFO_BACK( s.cmd_fifo ).resp_size = resp_max_size;
	CMD_FIFO_LAST( s.cmd_fifo );
	return 1;
}
/*-----------------------------------------------------------------------------------*/
u8_t
telnet_client_cmd(const char *cmd, char *resp_buffer, size_t resp_buffer_size, uint32_t timeout_ms)
{
	if( cmd == NULL ) return 0;
	int len = strlen(cmd);
	if( len <= 0 ) return 0;
	xSemaphoreTake( Sem_Telnet, 0 );
	if(!telnet_client_cmd_int( cmd, len, resp_buffer, resp_buffer_size )) return 0;
		
	/* Wait for response with TELNET_RESP_TIMEOUT */
	s.bRespReceived = 0; 
	volatile portTickType	time = xTaskGetTickCount();		
//#if portTICK_RATE_MS > 0
//	xSemaphoreTake( Sem_Telnet, timeout_ms / portTICK_RATE_MS );
//#else
	xSemaphoreTake( Sem_Telnet, timeout_ms * portTICK_RATE_MS_INV );
//#endif
	time = xTaskGetTickCount() - time;
	return s.bRespReceived;	
}
/*-----------------------------------------------------------------------------------*/


/** @} */
/** @} */
