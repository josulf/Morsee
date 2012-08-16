#ifndef _TASK_DEFINITIONS_H_
#define _TASK_DEFINITIONS_H_

//#include "global_vars.h"

/************************************************/
/* Constants									*/
/************************************************/

/* Ethernet task */
#define	tsk_ETHERNET_PRIORITY	( tskIDLE_PRIORITY + 2 )
#define tsk_ETHERNET_STACK_SIZE	(1024)
#define tsk_ETHERNET_RATE_MS	((portTickType) 50 )	/* Rate in ms */

/* Network init task */
#define	tsk_NETINIT_PRIORITY	( tskIDLE_PRIORITY )
#define tsk_NETINIT_STACK_SIZE	(1024)

/************************************************/
/* Prototypes									*/
/************************************************/
void Task_Ethernet_Create	( unsigned portBASE_TYPE uxPriority );
void Task_NetInit_Create	( unsigned portBASE_TYPE uxPriority );

#endif//_TASK_DEFINITIONS_H_
