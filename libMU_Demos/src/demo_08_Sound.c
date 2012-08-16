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
#include <FreeRTOS.h>
#include <task.h>
#include <libMU/pantalla.h>
#include <libMU/botones.h>
#include <libMU/leds.h>
#include <libMU/pwm.h>
#include <libMU/sonido.h>

/**
 * Frère Jacques
 */
libMU_Sound_ScoreNote_t	frere_jacques[] = {
		/* 4/4 */
		{ DO,	400 },
		{ RE,	400 },
		{ MI,	400 },
		{ DO,	400 },

		{ DO,	400 },
		{ RE,	400 },
		{ MI,	400 },
		{ DO,	400 },

		{ MI,	400 },
		{ FA,	400 },
		{ SOL,	800 },

		{ MI,	400 },
		{ FA,	400 },
		{ SOL,	800 },

		{ SOL,	200 },
		{ LA,	200 },
		{ SOL,	200 },
		{ FA,	200 },
		{ MI,	400 },
		{ DO,	400 },

		{ SOL,	200 },
		{ LA,	200 },
		{ SOL,	200 },
		{ FA,	200 },
		{ MI,	400 },
		{ DO,	400 },

		{ RE,	400 },
		{ SOL,	400 },
		{ DO,	800 },

		{ RE,	400 },
		{ SOL,	400 },
		{ DO,	800 },

};

/**
 * Input processing and display task
 */
void Task_Control(void* unused);

/**
 * Programa principal
 */
int main_demo_08(void)
{
	libMU_Display_Initialize();
	libMU_Button_Initialize();
	libMU_Sound_Initialize();
	libMU_Sound_SetSong( frere_jacques, sizeof(frere_jacques)/sizeof(libMU_Sound_ScoreNote_t) );

	libMU_Display_DrawString	( "Kaixo!!", 0, 0, 15 );

	xTaskCreate( Task_Control, "Control", 1024, NULL, tskIDLE_PRIORITY, NULL );
	vTaskStartScheduler();
	for(;;) { /* never here */ }
}

void Task_Control(void* unused)
{
	portTickType	last;
	char			msg[50]; uint32_t col = 1, count, up;
	libMU_Display_DrawString( "SEL : Play", 0, 89, 12 );
	libMU_Display_DrawString( "< > : Volume Up/down", 0, 81, 12 );

	for(;;) {
		vTaskDelayUntil( &last, 100 * portTICK_RATE_MS_INV );
		if( libMU_Button_Pressed( BUTTON_SELECT ) ) {
			if( !libMU_Sound_Playing )	libMU_Sound_Playing = 1;
			else						libMU_Sound_Repeat++;
		}
		if( libMU_Button_Pressed( BUTTON_LEFT )  && libMU_Sound_Volume < 50 )	libMU_Sound_Volume++;
		if( libMU_Button_Pressed( BUTTON_RIGHT ) && libMU_Sound_Volume > 0 )	libMU_Sound_Volume--;
		snprintf( msg, sizeof(msg), "Volume = %3d", libMU_Sound_Volume );
		libMU_Display_DrawString( msg, 0, 8, 15 );
		if( libMU_Sound_Playing )
			libMU_Display_DrawString("playing", 64, 0, col );
		else
			libMU_Display_DrawString("       ", 64, 0, 0 );
		count++;
		if( count >= 3 ) {
			count = 0;
			if( up ) {
				if( col < 15 ) col += 2; else up = 0;
			}else{
				if( col > 1  ) col -= 2; else up = 1;
			}
		}
		snprintf( msg, sizeof(msg), "Repeat = %3d", libMU_Sound_Repeat );
		libMU_Display_DrawString( msg, 0, 16, 12 );
	}
}

/**
 * @}
 */
