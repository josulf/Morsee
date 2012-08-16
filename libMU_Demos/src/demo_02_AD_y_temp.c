/**
 * @addtogroup demos
 * Test del HW analogico digital, temperatura y pantalla:
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#include <stdint.h>
#include <stdio.h>
#include <libMU/analogico_digital.h>
#include <libMU/temperatura.h>
#include <libMU/pantalla.h>
#include <libMU/retardo.h>

/**
 * Programa principal
 */
int main_demo_02(void)
{
	uint32_t	ADC_Values[4];
	int 		Temp; 
	char		msg[50];
	int			count = 0;

	libMU_Display_Initialize();	
	libMU_AD_Initialize();			

	libMU_Display_DrawString	( "Kaixo!!", 0, 0, 15 );

	while(1)	// repite indefinidamente
	{
		libMU_AD_StartConversion();
		count++;
		ADC_Values[0] = libMU_AD_GetChannelValue( ADC_CANAL_0 );    
		ADC_Values[1] = libMU_AD_GetChannelValue( ADC_CANAL_1 );    
		ADC_Values[2] = libMU_AD_GetChannelValue( ADC_CANAL_2 );
		ADC_Values[3] = libMU_AD_GetChannelValue( ADC_CANAL_3 );
		Temp          = libMU_Temperature_Get();

		// Mostrar resultados
		snprintf	( msg, sizeof(msg), "Temp = % 3d.%d C", Temp/10, Temp%10 );
		libMU_Display_DrawString( msg, 0,  8, 15 );
		snprintf	( msg, sizeof(msg), "ADC0 = %.2f V", ADC_Values[0]*(3.0/1024.0) );
		libMU_Display_DrawString( msg, 0, 16, 15 );
		snprintf	( msg, sizeof(msg), "ADC1 = %.2f V", ADC_Values[1]*(3.0/1024.0) );
		libMU_Display_DrawString( msg, 0, 24, 15 );
		snprintf	( msg, sizeof(msg), "ADC2 = %.2f V", ADC_Values[2]*(3.0/1024.0) );
		libMU_Display_DrawString( msg, 0, 32, 15 );
		snprintf	( msg, sizeof(msg), "ADC3 = %.2f V", ADC_Values[3]*(3.0/1024.0) );
		libMU_Display_DrawString( msg, 0, 40, 15 );
		snprintf	( msg, sizeof(msg), "Num = % 8d", count );
		libMU_Display_DrawString( msg, 0, 56, 8 );

		// Retardo ~ 15000000 x 60ns = 900ms
		libMU_Retardo( 15000000 );
	}
}
/**
 * @}
 */
