/**
 * \addtogroup telnet_client
 * @{
 */

/**
 * \file
 * Header file for the telnet client.
 * \author E�aut Muxika <emuxika@eps.mondragon.edu>
 */

#ifndef __TELNET_CLIENT_H__
#define __TELNET_CLIENT_H__

#include <uip/uipopt.h>

/************************************************/
/* Constants									*/
/************************************************/
#define	TELNET_SERVER_PORT	23

/************************************************/
/* New data types								*/
/************************************************/
typedef enum _telnet_state {
	TN_DISCONNECTED, TN_CONNECTED, TN_PENDING_ECHO, TN_PENDING_RESP, TN_CLOSE
} telnet_state;

#define	CMD_FIFO_SIZE	8

typedef struct _cmd_info_t {
	char*	resp;
	size_t	resp_size;
	char*	cmd;
	size_t	cmd_size;
} cmd_info_t;

typedef	struct	_cmd_fifo_t {
	size_t			count;
	size_t			first, last;
	cmd_info_t		fifo[CMD_FIFO_SIZE];
} cmd_fifo_t;

typedef struct _telnet_client_state {
	u16_t			timer;
	telnet_state	state;
	u16_t 			port;
  	uip_ipaddr_t	server;
  	size_t			retries;
  	char*			resp;
  	u8_t			bRespReceived;
  	u8_t			bCmdSent;
  	u8_t			bEcho;
  	cmd_info_t*		cmd_info;
  	
  	// Telnet Cmd FIFO
  	cmd_fifo_t		cmd_fifo;
} telnet_client_state;

/************************************************/
/* Global Vars									*/
/************************************************/

//typedef telnet_client_state uip_tcp_appstate_t;
//#define UIP_APPCALL telnet_client_appcall

/************************************************/
/* Prototypes									*/
/************************************************/
/**
 * Initialize the telnet client module.
 */
void telnet_client_init(void);

/**
 * Open an telnet connection to a telnet server
 *
 * This function opens an telnet connection to the specified telnet server.
 * 
 * A new request is sent by using the telnet_client_cmd() function.
 * The corresponding response can be obtained by sending 
 * and requests the specified file using the GET method. When the HTTP
 * connection has been connected, the telnet_client_connected() callback
 * function is called and when the HTTP data arrives the
 * telnet_client_datahandler() callback function is called.
 *
 * The callback function telnet_client_timedout() is called if the web
 * server could not be contacted, and the telnet_client_aborted() callback
 * function is called if the HTTP connection is aborted by the web
 * server.
 *
 * When the HTTP request has been completed and the HTTP connection is
 * closed, the telnet_client_closed() callback function will be called.
 *
 * @note If the function is passed a host name, it must already be in
 * the resolver cache in order for the function to connect to the web
 * server. It is therefore up to the calling module to implement the
 * resolver calls and the signal handler used for reporting a resolv
 * query answer.
 *
 * @param host A pointer to a string containing either a host name or
 * a numerical IP address in dotted decimal notation (e.g., 192.168.23.1).
 *
 * @param port The port number to which to connect, in host byte order.
 *
 * @param file A pointer to the name of the file to get.
 *
 * \retval 0 if the host name could not be found in the cache, or
 * if a TCP connection could not be created.
 *
 * \retval 1 if the connection was initiated.
 */
u8_t	telnet_client_open(uip_ipaddr_t *host, u16_t port);

/**
 * New command to send to telnet server.
 * Returns 0 if a command is pending
 */
u8_t	telnet_client_cmd(const char *cmd, char *resp_buffer, size_t resp_buffer_size, uint32_t timeout_ms);
u8_t	telnet_client_cmd_int(const char *cmd, size_t len, char *resp_buff, size_t resp_buffer_size);
/**
 * Read response from telnet server.
 * Returns NULL if we have not received a response yet.
 */
char*	telnet_client_resp(void);

/**
 * Callback
 */
void	telnet_client_resp_received(void);

/**
 * return telnet client state
 */
telnet_state	telnet_client_get_state(void);
/**
 * Close the currently open HTTP connection.
 */
void telnet_client_close(void);
void telnet_client_appcall(void);

/**
 * Obtain the hostname of the current HTTP data stream.
 *
 * The hostname of the web server of an HTTP request may be changed
 * by the web server, and may therefore not be the same as when the
 * original GET request was made with telnet_client_get(). This function
 * is used for obtaining the current hostname.
 *
 * \return A pointer to the current hostname.
 */
char *telnet_client_hostname(void);

/**
 * Obtain the port number of the current HTTP data stream.
 *
 * The port number of an HTTP request may be changed by the web
 * server, and may therefore not be the same as when the original GET
 * request was made with telnet_client_get(). This function is used for
 * obtaining the current port number.
 *
 * \return The port number of the current HTTP data stream, in host byte order.
 */
unsigned short telnet_client_port(void);



#endif /* __WEBCLIENT_H__ */

/** @} */
