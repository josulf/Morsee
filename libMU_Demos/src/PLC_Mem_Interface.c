
#include <libMU/plc.h>
#include "PLC_vars.h"

/************************************************/
/* New data types								*/
/************************************************/
libMU_PLC_Transfer_t	tr;

/************************************************/
/* Function implementation						*/
/************************************************/
void PLC_ReceiveMemoryData(libMU_PLC_Transfer_t* trdata)
{
	PLC_Writes++;
	tr = *trdata;
	
	/* Here we transfer the received information to the internal variables */
	/* InTouch -> uC transfer, ONLY 1 var per write */
	if( !libMU_PLC_Receive_MB( trdata, 0, &Switches ) ) return;	// MB0
//	if( !libMU_PLC_Receive_MW( trdata, 2, &TransferData[0] ) ) return;	// MW2
//	if( !libMU_PLC_Receive_MW( trdata, 4, &TransferData[1] ) ) return;	// MW4
//	if( !libMU_PLC_Receive_MW( trdata, 6, &TransferData[2] ) ) return;	// MW6
//	if( !libMU_PLC_Receive_MW( trdata, 8, &TransferData[3] ) ) return;	// MW8
//	if( !libMU_PLC_Receive_Bit( trdata, 10, 3, &BitVal ) ) return;	// M10.3	
}

void PLC_SendMemoryData(libMU_PLC_Transfer_t* trdata)
{
	PLC_Reads++;

	tr = *trdata;
	tr_byte = tr.data[0];

	/* Here we transfer the received information to the internal variables */
	/* uC -> InTouch transfer, All variables at the same time */
	libMU_PLC_Send_MB( trdata, 0, Switches );
//	libMU_PLC_Send_MW( trdata, 0, TransferData[0] );
//	libMU_PLC_Send_MW( trdata, 0, TransferData[1] );
//	libMU_PLC_Send_MW( trdata, 0, TransferData[2] );
//	libMU_PLC_Send_MW( trdata, 0, TransferData[3] );
//	libMU_PLC_Send_MB( trdata, 0, BitGroup );	// Send 8 bits
}
