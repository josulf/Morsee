/**
 * @addtogroup demos
 * Test del uso del sistema operativo FreeRTOS
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
#include <libMU/serie.h>

#include <FreeRTOS.h>
#include <task.h>

/**
 * Count milliseconds
 */
uint32_t	Count_1ms;
/**
 * Prototypes
 */
void T1_Display(void* unused);
void T2_Counter(void* unused);

/**
 * Programa principal
 */
int main_demo_06(void)
{
	/* Initialize HW */
	libMU_Display_Initialize();
	libMU_Button_Initialize();
	libMU_Serial_Initialize(115200);
	libMU_Display_DrawString( "HW init done!", 0, 0, 12 );	

	/* Create tasks */
	xTaskCreate( T1_Display, "T1_Disp",  1024, NULL, tskIDLE_PRIORITY, NULL ); 
	xTaskCreate( T2_Counter, "T1_Count", 1024, NULL, tskIDLE_PRIORITY, NULL ); 

	/* Start FreeRTOS scheduler */
	vTaskStartScheduler();
	
	/* Should never reach this point */
	libMU_Display_DrawString("Error           ", 0, 0, 15);
	for(;;) {}
}

void T1_Display(void* unused)
{
	char msg[50];
	uint32_t	count = 0;
	portTickType xLast = xTaskGetTickCount();
	for(;;) {
		vTaskDelayUntil( &xLast, 1000 * portTICK_RATE_MS_INV );	/* 1000 milliseconds */
		count++;
		snprintf( msg, sizeof(msg), "Sec=%8d", count );
		libMU_Display_DrawString( msg, 0, 8, 12 );
		snprintf( msg, sizeof(msg), "ms =%8d", Count_1ms );
		libMU_Display_DrawString( msg, 0, 16, 12 );
	}	
}

void T2_Counter(void* unused)
{
	portTickType xLast = xTaskGetTickCount();
	for(;;) {
		vTaskDelayUntil( &xLast, 10 * portTICK_RATE_MS_INV );	/* 10 milliseconds */
		Count_1ms+=10;
	}	
}

/**
 * @}
 */
