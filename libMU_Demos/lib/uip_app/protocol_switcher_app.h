#ifndef PROTOCOL_SWITCHER_APP_H_
#define PROTOCOL_SWITCHER_APP_H_

// UDP protocol managers
#include "uip_app/dhcpc.h"
#include "uip_app/resolv.h"

// TCP protocol managers
#include "uip_app/siemensTCP_app.h"
#include "uip_app/telnet_client.h"
#include "uip_app/special_server.h"
#include "uip_app/httpd.h"
#include "uip_app/webclient.h"
#include "uip_app/udp_frame.h"

/************************************************/
/* New data types								*/
/************************************************/
typedef struct httpd_state uip_tcp_appstate_t;

/************************************************/
/* Constants									*/
/************************************************/
//#define USE_STATIC_IP
#define DEFAULT_IPADDR0         130
#define DEFAULT_IPADDR1         130
#define DEFAULT_IPADDR2         130
#define DEFAULT_IPADDR3         10
#define DEFAULT_NETMASK0        255
#define DEFAULT_NETMASK1        255
#define DEFAULT_NETMASK2        255
#define DEFAULT_NETMASK3        0

#define	UIP_APPCALL		tcp_protocol_switcher_appcall
#define	UIP_UDP_APPCALL	udp_protocol_switcher_appcall

/************************************************/
/* global variables								*/
/************************************************/

/************************************************/
/* Prototypes									*/
/************************************************/
void protocol_switcher_app_init(void);
void tcp_protocol_switcher_appcall(void);
void udp_protocol_switcher_appcall(void);

#endif /*PROTOCOL_SWITCHER_APP_H_*/
