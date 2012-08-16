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

#ifndef __UDP_FRAME_H__
#define __UDP_FRAME_H__

#include <uip/uipopt.h>
#include <stdint.h>

//typedef int uip_udp_appstate_t;	// defined in dhcpc.h

/**
 * Data type for UDP frame information to send packets
 */
typedef struct _udp_frame_packet_info_t {
	struct uip_udp_conn* 	conn;
	char*					data;
	int						length;
	unsigned int			age;
} udp_frame_packet_info_t;

/**
 * Data type for UDP frame processing callback function
 * @param conn	connection information (remote host ip address, remote port and local port)
 * @param data	packet data pointer
 * @param len	packet data length
 * @note
 * - This function HAS to be non blocking
 * - An answer packet may be sent using @ref{udp_frame_send_answer()} function
 */
typedef void (*upd_frame_callback_t)(struct uip_udp_conn* conn, char *data, int len);

/**
 * Function to process UIP frames
 */
void udp_frame_appcall(void);

/**
 * Function to set callback function for UDP frames
 * @param function	callback function to proccess received frames
 * @note
 * - There is only a single callback function for ALL UDP frames
 *   except for DNS and DHCP frames
 */
void udp_frame_setcallback(upd_frame_callback_t function);

/**
 * Function to send a packet to a destination host
 * @param ip	ip address of packet destination host
 * @param port	port to use at the destination/local hosts
 * @param data	packet data pointer
 * @param len	packet data length
 * @return		1 if packet successfully stored, 0 if connection not possible, -1 if packet not sent yet
 * @note
 * - The packet is not sent immediately. The information is stored in an
 *   intermediate variable and sent in the next UDP polling event
 */
int	udp_frame_send(uip_ipaddr_t ip, int port, char* data, int len);

/**
 * Function to send an answer packet to the host from which we have received a packet
 * @param data	packet data pointer
 * @param len	packet data length
 * @note
 * - This function can ONLY be called from the callback function
 */
void	udp_frame_send_answer(char* data, int len);



#endif /* __UDP_FRAME_H__ */

/**
 * @}
 */
