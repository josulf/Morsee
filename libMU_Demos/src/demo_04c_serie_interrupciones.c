/**
 * @addtogroup demos
 * Test del protocolo XON/XOFF
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#include <stdint.h>
#include <stdio.h>
#include <libMU/serie.h>
#include <libMU/botones.h>
#include <libMU/pantalla.h>


uint8_t	TxBuf[1024];
uint8_t	RxBuf[1024];

/**
 * Programa principal
 */
int main_demo_04c(void)
{
	char msg[50];
	
	libMU_Display_Initialize();
	libMU_Serial_Initialize(115200);

	libMU_Display_DrawString( "Guten morgen, bidali!!\r\n", 0, 0, 15 );
	libMU_Serial_SendString	( "Guten morgen, bidali!!\n" );
	libMU_Serial_SetBufferedTransmission( TxBuf, sizeof(TxBuf), RxBuf, sizeof(RxBuf) );

	while(1)	// repite indefinidamente
	{
        libMU_Serial_SendString( "\n\nEnvia algo por la linea serie : " );

        // Leer texto
        libMU_Serial_GetString( msg, sizeof(msg)-1 );

        libMU_Serial_SendString( "\nHas enviado : " );
        libMU_Serial_SendString( msg );
	}
}
/**
 * @}
 */
