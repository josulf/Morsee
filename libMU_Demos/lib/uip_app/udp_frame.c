/**
 * \addtogroup udp_frame
 * UIP application for udp frame manipulation
 * @{
 ********************************************************************
 * \author		Eñaut Muxika <emuxika at mondragon dot edu>
 * \date		2012/2/28
 * \copyright	BSDL
 ********************************************************************
 */

#include <uip/uip.h>
#include "udp_frame.h"

/**
 * Local variable to store the UDP frame processing callback function
 * @param conn	connection information (remote host ip address, remote port and local port)
 * @param data	packet data pointer
 * @param len	packet data length
 * \return length of response packet created in "data", 0 if we do not want a response
 * @note
 * - This function HAS to be non blocking
 */
static upd_frame_callback_t	udp_frame_callback = NULL;

/**
 * Local variables to store information about the frame to be sent
 */
#define	AGE_MAX		(UIP_UDP_CONNS + 1)
static	udp_frame_packet_info_t	udp_frame_pi[ UIP_UDP_CONNS ] = {
		{ NULL, NULL, 0, AGE_MAX },
		{ NULL, NULL, 0, AGE_MAX },
		{ NULL, NULL, 0, AGE_MAX },
		{ NULL, NULL, 0, AGE_MAX }
	};

/**
 * Function to process UIP frames
 */
void udp_frame_appcall(void)
{
	int docheck = 0;
	if( udp_frame_callback != NULL && uip_newdata() ) {
		udp_frame_callback( uip_udp_conn, uip_appdata, uip_len );
		docheck = 1;
	}
	if( uip_poll() || docheck ) {
		int i;
		for( i = 0; i < UIP_UDP_CONNS; i++ ) {
			if( udp_frame_pi[i].length > 0 && udp_frame_pi[i].conn == uip_udp_conn ) {
				uip_send( udp_frame_pi[i].data, udp_frame_pi[i].length );
				udp_frame_pi[i].data = NULL;
				udp_frame_pi[i].length = 0;
				udp_frame_pi[i].age = 0;
				return;
			}
		}
	}
}

/**
 * Function to set callback function for UDP frames
 * @param function	callback function to proccess received frames
 * @note
 * - There is only a single callback function for ALL UDP frames
 *   except for DNS and DHCP frames
 * - If function is NULL the we disable the UDP frame reception
 */
void udp_frame_setcallback(upd_frame_callback_t function)
{
	udp_frame_callback = function;
}

/**
 * Function to send an answer packet to the host from which we have received a packet
 * @param data	packet data pointer
 * @param len	packet data length
 * @note
 * - This function can ONLY be called from the callback function
 */
void	udp_frame_send_answer(char* data, int len)
{
	uip_send(data, len);
}

/**
 * Function to send a packet to a destination host
 * @param ip	ip address of packet destination host
 * @param port	port to use at the destination/local hosts
 * @param data	packet data pointer
 * @param len	packet data length
 * \return		1 if packet successfully stored, 0 if connection not possible, -1 if packet not sent yet
 * @note
 * - The packet is not sent immediately. The information is stored in an
 *   intermediate variable and sent in the next UDP polling event
 */
int	udp_frame_send(uip_ipaddr_t ip, int port, char* data, int len)
{
	int i; struct uip_udp_conn* conn;
	port = htons(port);
	if( len < 0 || data == NULL ) return 0;

	// Check if there is an existing connection
	for( i = 0; i < UIP_UDP_CONNS; i++ ) {
		if( uip_udp_conns[i].rport != port ||
			uip_udp_conns[i].lport != port ||
			!uip_ipaddr_cmp( uip_udp_conns[i].ripaddr, ip ) ) continue;

		if( udp_frame_pi[i].length > 0 ) return 0;
		udp_frame_pi[i].conn = &uip_udp_conn[i];
		udp_frame_pi[i].data = data;
		udp_frame_pi[i].length = len;
		udp_frame_pi[i].age = 0;
		return 1;
	}

	// Create new connection
	conn = uip_udp_new( (uip_ipaddr_t*) ip, port );
	if( conn == NULL ) {
		int oldest = 0;
		// remove oldest connection to create a new one
		for( i = 1; i < UIP_UDP_CONNS; i++ ) {
			if( udp_frame_pi[i].age > udp_frame_pi[oldest].age ) oldest = i;
		}
		uip_udp_remove( udp_frame_pi[oldest].conn );
		conn = uip_udp_new( (uip_ipaddr_t*) ip, port );
		if( conn == NULL ) return 0;
	}
	// Update connection ages and data
	uip_udp_bind( conn, port );
	for( i = 0; i < UIP_UDP_CONNS; i++ ) {
		if( &uip_udp_conns[i] == conn ) {
			udp_frame_pi[i].conn = conn;
			udp_frame_pi[i].data = data;
			udp_frame_pi[i].length = len;
			udp_frame_pi[i].age = 0;
		}else{
			if( udp_frame_pi[i].conn != NULL &&  udp_frame_pi[i].age < AGE_MAX )
				udp_frame_pi[i].age++;	// Update age of connections
		}
	}

	// Store packet information to be sent
	return 1;
}

/**
 * @}
 */
