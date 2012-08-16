/**
 * @addtogroup demos
 * Test del PWM
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#include <stdint.h>
#include <stdio.h>
#include <libMU/pantalla.h>
#include <libMU/botones.h>
#include <libMU/leds.h>
#include <libMU/pwm.h>

/**
 * Programa principal
 */
int main_demo_03(void)
{
	char		msg[50]; int status;
	int 		period = 5000; uint32_t count = 0; int inc; uint32_t stepup;

	libMU_Display_Initialize();
	libMU_Button_Initialize();	
	libMU_PWM_InitializeOutput( LIBMU_PWM_0, 5000/*Hz*/ );
	libMU_PWM_InitializeChopperOutput( LIBMU_PWM_GRP_23, 1000/*Hz*/, 10000/*ns*/ );

	libMU_Display_DrawString	( "Kaixo!!", 0, 0, 15 );

	for(;;)	// repite indefinidamente
	{
        if( libMU_Button_GetStatus(BUTTON_SELECT) ) {
	        libMU_Display_DrawString( "Select", 0, 8, 15 );
        }else{
	        libMU_Display_DrawString( "      ", 0, 8, 15 );        
        }
        snprintf( msg, sizeof(msg), "Count=%8d", count++ );
        libMU_Display_DrawString( msg, 0, 16, 12 ); 
        snprintf( msg, sizeof(msg), "Period=%3d.%02d%%", period/100, period%100 );
        libMU_Display_DrawString( msg, 0, 24, 15 ); 

        // actualiza la cuenta y cambia el estado de LED_2 y LED_3 segun los periodos
        if( libMU_Button_GetStatus( (libMU_Button_Identifiers_t)(BUTTON_LEFT|BUTTON_RIGHT) ) ) {
			if( libMU_Button_Pressed( (libMU_Button_Identifiers_t)(BUTTON_LEFT|BUTTON_RIGHT) ) ) {
				inc = 1; stepup = 0;
			}else{
				stepup++;
				if( stepup > 300 )		inc = 500;
				else if( stepup > 200 )	inc = 100;
				else if( stepup > 100 )	inc = 10;
			}
			if( libMU_Button_GetStatus(BUTTON_LEFT) )	period += inc;
			if( libMU_Button_GetStatus(BUTTON_RIGHT) )	period -= inc;
		}
		if( period > 10000 )	period = 10000;
		if( period < 1 )		period = 1;
		libMU_PWM_SetPulseWidth( LIBMU_PWM_0, period );
		if( status != ( period / 3334 ) ) {
			status = period / 3334;
			switch( status ) {
			case 0: libMU_PWM_SetPulseWidth( LIBMU_PWM_2, 2500 ); break;	// 25%
			case 1: libMU_PWM_SetPulseWidth( LIBMU_PWM_2, 5000 ); break;	// 50%
			case 2: libMU_PWM_SetPulseWidth( LIBMU_PWM_2, 7500 ); break;	// 75%
			}
		}
	}
}

/**
 * @}
 */
