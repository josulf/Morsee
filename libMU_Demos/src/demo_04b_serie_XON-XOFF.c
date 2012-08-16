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

/**
 * Constantes XON y XOFF para las comunicaciones
 */
enum constants {
	XON = 0x11,
	XOFF= 0x13
};

/**
 * Programa principal
 */
int main_demo_04b(void)
{
	int xon_xoff_status = 0;
	char c;
	
	libMU_Display_Initialize();
	libMU_Button_Initialize();
	libMU_Serial_Initialize(115200);	// 115200,8N1

	libMU_Display_DrawString	( "Kaixo! Bidali zerbait!!", 0, 0, 15 );

	for(;;)	// repite indefinidamente
	{
		if( libMU_Button_Pressed(BUTTON_SELECT) ) {	// Detectar que se pulsa el boton
			xon_xoff_status = !xon_xoff_status;	// cambia el estado
			if( xon_xoff_status ) {
				libMU_Serial_SendChar	( XON );
				libMU_Display_DrawString( "XON ", 0, 8, 15 );
			}else{
				libMU_Serial_SendChar	( XOFF );
				libMU_Display_DrawString( "XOFF", 0, 8, 15 );
			}
		}
		// Si se recibe caracter enviar de vuelta
		if( libMU_Serial_GetChar_NB( &c ) > 0 ) {
			libMU_Serial_SendChar( c );
		}
	}
}
/**
 * @}
 */
