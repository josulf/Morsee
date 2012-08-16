/**
 * @addtogroup demos
 * Test de los botones y LEDs
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

static inline unsigned char bit_a_texto( unsigned long num, int bit)
{
	return (num & (1<<bit))?'1':'0';
//	unsigned long mask = (1<<bit);		/* All zeros except one '1' in the position given by 'bit' */
//	unsigned long cond = num & mask;	/* 0 if the bit number 'bit' is zero, nonzero otherwise */
//	if( cond ) return '1' else '0';		/* return '1' or '0' corresponding */
}

/**
 * Programa principal
 */
int main_demo_01(void)
{
	char		msg[50]; int count = 0; int period_1, period_2, mask;
	unsigned int estado;

	libMU_Display_Initialize();
	libMU_Button_Initialize();	
	libMU_LED_Initialize();
	libMU_LED_InitializeEth();

	libMU_Display_DrawString	( "Kaixo!!", 0, 0, 15 );

	period_1 = (1<<7);
	period_2 = (1<<8);

	for(;;)	// repite indefinidamente
	{
		estado = libMU_Button_GetFullStatus();		

		// Mostrar resultados
        snprintf	( msg, sizeof(msg), "Botones = %c%c%c%c|%c%c%c%c", 
        			  bit_a_texto( estado, 7 ),
        			  bit_a_texto( estado, 6 ),
        			  bit_a_texto( estado, 5 ),
        			  bit_a_texto( estado, 4 ),
        			  bit_a_texto( estado, 3 ),
        			  bit_a_texto( estado, 2 ),
        			  bit_a_texto( estado, 1 ),
        			  bit_a_texto( estado, 0 )
        			);
        			
        libMU_Display_DrawString( msg, 0,  8, 15 );
                
        if( libMU_Button_GetStatus(BUTTON_UP) ) {
	        libMU_Display_DrawString( "Arriba", 0, 16, 15 );        
        }else{
	        libMU_Display_DrawString( "      ", 0, 16, 15 );        
        }
        if( libMU_Button_GetStatus(BUTTON_DOWN) ) {
	        libMU_Display_DrawString( "Abajo", 0, 24, 15 );        
        }else{
	        libMU_Display_DrawString( "     ", 0, 24, 15 );        
        }
        if( libMU_Button_GetStatus(BUTTON_LEFT) ) {
	        libMU_Display_DrawString( "Izquierda", 0, 32, 15 );        
        }else{
	        libMU_Display_DrawString( "         ", 0, 32, 15 );        
        }
        if( libMU_Button_GetStatus(BUTTON_RIGHT) ) {
	        libMU_Display_DrawString( "Derecha", 0, 40, 15 );        
        }else{
	        libMU_Display_DrawString( "       ", 0, 40, 15 );        
        }
        if( libMU_Button_GetStatus(BUTTON_SELECT) ) {
	        libMU_Display_DrawString( "Select", 0, 48, 15 );
        }else{
	        libMU_Display_DrawString( "      ", 0, 48, 15 );        
        }

        // Cambia el estado del LED 1 cada vez que se pulse el botton select
        if( libMU_Button_Pressed(BUTTON_SELECT) ) {
        	libMU_LED_Toggle( LED_1 );
        }

        // actualiza la cuenta y cambia el estado de LED_2 y LED_3 segun los periodos
		count++;
		if( libMU_Button_Pressed(BUTTON_UP)   	&& period_1 < (1<<11) ) period_1<<=1;
		if( libMU_Button_Pressed(BUTTON_DOWN) 	&& period_1 > 1 ) 		period_1>>=1;
		if( libMU_Button_Pressed(BUTTON_RIGHT)	&& period_2 < (1<<11) ) period_2<<=1;
		if( libMU_Button_Pressed(BUTTON_LEFT)	&& period_2 > 1 ) 		period_2>>=1;

		mask = (period_1<<1) - 1;
		if( (count & mask) == 0 )			libMU_LED_On ( LED_2 );
		if( (count & mask) == period_1 )	libMU_LED_Off( LED_2 );
		mask = (period_2<<1) - 1;
		if( (count & mask) == 0 )			libMU_LED_On ( LED_3 );
		if( (count & mask) == period_2 )	libMU_LED_Off( LED_3 );
	}
}
/**
 * @}
 */
