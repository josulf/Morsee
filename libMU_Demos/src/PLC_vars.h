#ifndef PLC_VARS_H_
#define PLC_VARS_H_

#include <stdint.h>
#include <libMU/plc.h>

/**
 * Information received from KepServer/InTouch
 */
extern uint8_t				Switches;

/**
 * Other variables
 */
extern int					PLC_Writes;
extern int					PLC_Reads;
extern libMU_PLC_Transfer_t	tr;
extern int					tr_byte;


#endif /*PLC_VARS_H_*/
