#ifndef _NETWORK_STATE_H_
#define _NETWORK_STATE_H_

#include <stdint.h>

/************************************************/
/* New Data types								*/
/************************************************/

// Network connection state information
typedef enum _network_state_t {
	NETWORK_IP_MISSING	= 0, 
	NETWORK_IP_OK		= 1,
	NETWORK_ERROR		= -1
} network_state_t;

// Network frame information
typedef struct _frame_stats_t {
	uint32_t		recv;
	uint32_t		sent;
	uint32_t		frames;
} frame_stats_t;

/************************************************/
/* Globals variables							*/
/************************************************/
extern	network_state_t	network_state;

#endif//_NETWORK_STATE_H_
