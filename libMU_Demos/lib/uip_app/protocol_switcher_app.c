
#include <string.h>
#include <uip/uip.h>
#include <uip/uip_arp.h>
#include "protocol_switcher_app.h"
//#include "global_vars.h"

/************************************************/
/* Global vars									*/
/************************************************/

/************************************************/
/* Implementations								*/
/************************************************/
void protocol_switcher_app_init(void)
{
	telnet_client_init();
	siemensTCP_app_init();
	specserver_app_init();
	webclient_init();
	httpd_init();

	resolv_init();
#ifndef USE_STATIC_IP
    // Initialize the DHCP Client Application.
    static struct uip_eth_addr sTempAddr;
    uip_getethaddr(sTempAddr);
    dhcpc_init(&sTempAddr.addr[0], 6);
    dhcpc_request();
#else
	uip_ip4addr_t uip_dnsaddr;
	uip_ipaddr(uip_dnsaddr, DNS_IP1, DNS_IP2, DNS_IP3, DNS_IP4 );
	resolv_conf(uip_dnsaddr);
#endif
}

void tcp_protocol_switcher_appcall(void)
{
	if( uip_conn->rport == HTONS( TELNET_SERVER_PORT ) ) { // telnet port
		telnet_client_appcall();
		return;
	}
	if( uip_conn->lport == HTONS( SIEMENS_TCP_PORT ) ) { // Siements TCPIP port
		siemensTCP_appcall();
		return;
	}
	if( uip_conn->lport == HTONS( 80 ) ) { // Web Server port
		httpd_appcall();
		return;
	}
	if( uip_conn->lport == HTONS( SPECIAL_SERVER_PORT ) ) { // Special Server port
		specserver_appcall();
		return;
	}
	webclient_appcall();	// Usually remote port 80 but can be any other one
}

void udp_protocol_switcher_appcall(void)
{
	if( uip_udp_conn->rport == HTONS(DNS_SERVER_PORT) ) {	// DNS resolver port
		resolv_appcall();
		return;
	}
	if( uip_udp_conn->rport == HTONS(DHCP_SERVER_PORT) ) {	// DHCP port
		dhcpc_appcall();
		return;
	}
	udp_frame_appcall();
}
