/**
 * @addtogroup	FreeRTOS_Helper
 * Library for FreeRTOS helper/debugging functions
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2012/6/6
 * @copyright	BSDL
 ********************************************************************
 */
#include <FreeRTOS.h>
#include <task.h>

/**
 * Function called from vTaskStartScheduler() to initialize interrupt system and timer
 * @note	This function is called depending on the used architecture, i.e.,
 * 			ARM Cortex-M3 does not use this function
 * 			MicroblazeV8 uses this function
 */
void vApplicationSetupTimerInterrupt(void)
{
//	libMU_Timer_Initialize();
//	libMU_Timer_SetFrequency( configTICK_RATE_HZ );
//	libMU_Timer_SetHandler( vPortTickISR, NULL );
}

/**
 * Funcion called to clear timer interrupt request
 * @note	It can be empty if it is previously done in the interrupt handler
 */
void vApplicationClearTimerInterrupt(void)
{
}

/**
 * Function called when a task stack overflow occurs if
 * configCHECK_FOR_STACK_OVERFLOW > 0
 * @param	task		Task handle of current task
 * @param	taskName	Task name of current task
 */
void	vApplicationStackOverflowHook( xTaskHandle task, const char* taskName )
{
	for(;;) {}
}

/**
 * Function called when a malloc() function call fails in FreeRTOS code if
 * configUSE_MALLOC_FAILED_HOOK > 0
 */
void	vApplicationMallocFailedHook(void)
{
	xTaskHandle		task	 = xTaskGetCurrentTaskHandle();
	signed char*	taskName = pcTaskGetTaskName( task );
	for(;;) {}
}

/**
 * @}
 */
