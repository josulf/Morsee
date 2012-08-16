/**
 * @addtogroup demos
 * Test de la linea serie
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
#include <libMU/pantalla.h>

/**
 * Programa principal
 */
int main_demo_04(void)
{
	char msg[50];
	
	libMU_Display_Initialize();
	libMU_Serial_Initialize(115200);
		
	libMU_Display_DrawString	( "Guten morgen, bidali!!\r\n", 0, 0, 15 );

	while(1)	// repite indefinidamente
	{        			
        libMU_Serial_SendString( "\r\nEnvia algo por la linea serie!!\r\n" );
        
        // Leer texto
        libMU_Serial_GetString( msg, sizeof(msg)-1 );
        
        libMU_Serial_SendString( "\r\nHas enviado :\r\n" );
        libMU_Serial_SendString( msg );
	}
}
/**
 * @}
 */
