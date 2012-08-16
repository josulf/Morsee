/**
 * \addtogroup apps
 * @{
 */

/**
 * \defgroup special_server Special webserver
 * @{
 *
 * This example shows a simplified webserver that always responds with the same information.
 */

/**
 * \file
 * Implementation of the HTTP client.
 * \author Eñaut Muxika <emuxika@eps.mondragon.edu>
 */

#include <stdlib.h>
#include <string.h>
#include <uip/uip.h>
#include <uip/uiplib.h>
#include "special_server.h"
#include "network_state.h"

/************************************************/
/* Constants									*/
/************************************************/
#define	SPECIAL_SERVER_TIMEOUT	20

/************************************************/
/* Global variables								*/
/************************************************/
char*			specserver_resp_data;
int				specserver_resp_data_len;						
frame_stats_t	specserver_stats;

/************************************************/
/* Prototypes									*/
/************************************************/

/************************************************/
/* Function implementations						*/
/************************************************/

/*-----------------------------------------------------------------------------------*/
void
specserver_app_init(void)
{
	uip_listen( HTONS( SPECIAL_SERVER_PORT ) );
    specserver_stats.recv = specserver_stats.sent = 0;    
}
/*-----------------------------------------------------------------------------------*/
void
specserver_appcall(void)
{
	static uint8_t reqs = 0; static char *data;
	if( uip_conn->lport != HTONS( SPECIAL_SERVER_PORT ) ) return; 
	specserver_stats.frames++;

	if( uip_connected() ) {
		reqs = 0;
	}
	if( uip_newdata() ) {
		specserver_stats.recv++;
		char *p;
		// Process response
		data = (char*)uip_appdata;
		p = strchr( data, '\r' ); if( p != NULL ) *p = 0;
		p = strchr( data, '\n' ); if( p != NULL ) *p = 0;
		data[50] = 0;	// Limit max size to one line or 50 char, whicever is smaller
		reqs++;
	}
	if( uip_acked() ) {
	}
	if( uip_rexmit() ||
		uip_newdata() ||
		uip_acked()) {
		// Send response
		if( reqs > 0 ) {
			reqs--;
			uip_send( specserver_resp_data, specserver_resp_data_len );
		}else{
			uip_close();
		}
	} else if(uip_poll()) {
		uip_close();
	}
}
/*-----------------------------------------------------------------------------------*/
char	*i2dec(char *buf, int num, int width)
{
	char c; int i = width; int ndig = 0; int sgn = 0;
	if( i == 0 ) i = 10;	// all digits
	if( i < 0 ) i = -i;		// justify left
	if( num < 0 ) { num = -num; sgn = 1; }
	buf[i] = 0;
	
	while( i > 0 && num > 0 ) {
		c = (num%10) + '0'; num /= 10; 
		i--; buf[i] = c; ndig++;		
	}
	if( ndig == 0 && num == 0 ) {
		i--; buf[i] = '0'; ndig++;		
	}
	if( sgn && i > 0 ) {
		i--; buf[i] = '-'; ndig++; sgn = 0;		
	}
	if( num > 0 || sgn ) { // Not enough space
		if( sgn )	buf[0] = '<';
		else		buf[0] = '>';
	}
	if( i > 0 ) {
		if( width == 0 ) { // 
			memcpy( buf, buf + i, ndig + 1 );
		}else if( width > 0 ) { // Justify right
			memset( buf, ' ', i ); ndig += i;
		}else{	// Justify left
			memcpy( buf, buf + i, ndig );
			memset( buf + ndig, ' ', i ); ndig += i;
		}
	}
	return (buf+ndig);
}

char	*i2hex(char *buf, uint32_t num, int width)
{
	char c; int i = width; int ndig = 0;
	if( i <= 0 ) i = 8;	// all digits
	buf[i] = 0;
	
	while( i >= 0 ) {
		c = (num & 0xF) + '0'; num >>= 4;
		if( c > '9' ) c += ('A' - '0' - 10); 
		i--; buf[i] = c; ndig++;		
	}
	return (buf+ndig);
}

/** @} */
/** @} */
