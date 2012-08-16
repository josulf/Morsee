//*****************************************************************************
//
// uip-conf.h - uIP Project Specific Configuration File
//
// Copyright (c) 2007-2011 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 6852 of the EK-LM3S8962 Firmware Package.
//
//*****************************************************************************

#ifndef __UIP_CONF_H__
#define __UIP_CONF_H__

#include <stdint.h>

typedef	uint8_t		u8_t;
typedef uint16_t 	u16_t;

// Statistics datatype
// This typedef defines the dataype used for keeping statistics in
// uIP.
typedef unsigned short uip_stats_t;

// Ping IP address assignment
// Use first incoming "ping" packet to derive host IP address
#define UIP_CONF_PINGADDRCONF       0

// UDP support on or off
#define UIP_CONF_UDP                1

// UDP checksums on or off
// (not currently supported ... should be 0)
#define UIP_CONF_UDP_CHECKSUMS      0

// UDP Maximum Connections
#define UIP_CONF_UDP_CONNS          4

// Maximum number of TCP connections.
#define UIP_CONF_MAX_CONNECTIONS    4

// Maximum number of listening TCP ports.
#define UIP_CONF_MAX_LISTENPORTS    4

// Size of advertised receiver's window
//#define UIP_CONF_RECEIVE_WINDOW     400

// Size of ARP table
#define UIP_CONF_ARPTAB_SIZE        8

// uIP buffer size.
#define UIP_CONF_BUFFER_SIZE        1500

// uIP statistics on or off
#define UIP_CONF_STATISTICS         1

// Logging on or off
#define UIP_CONF_LOGGING            0

// Broadcast Support
#define UIP_CONF_BROADCAST          0

// Link-Level Header length
#define UIP_CONF_LLH_LEN            14

// CPU byte order.
#define UIP_CONF_BYTE_ORDER         LITTLE_ENDIAN

// Representation of an IP address.
typedef u16_t uip_ip4addr_t[2];
typedef u16_t uip_ip6addr_t[8];
#if UIP_CONF_IPV6
typedef uip_ip6addr_t uip_ipaddr_t;
#else /* UIP_CONF_IPV6 */
typedef uip_ip4addr_t uip_ipaddr_t;
#endif /* UIP_CONF_IPV6 */

// Here we include the header file for the application we are using in
// this example
#include "uip_app/protocol_switcher_app.h"

// Define the uIP Application State type, based on the httpd.h state variable.
//typedef u16_t uip_tcp_appstate_t;

// UIP_APPCALL: the name of the application function. This function
// must return void and take no arguments (i.e., C type "void
// appfunc(void)").
#ifndef UIP_APPCALL
#define UIP_APPCALL     defined_in_protocol_switcher_app_h
#endif

#endif // __UIP_CONF_H_
