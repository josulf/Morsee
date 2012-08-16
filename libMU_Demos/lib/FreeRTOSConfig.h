/*
    FreeRTOS V6.1.1 - Copyright (C) 2011 Real Time Engineers Ltd.

    ***************************************************************************
    *                                                                         *
    * If you are:                                                             *
    *                                                                         *
    *    + New to FreeRTOS,                                                   *
    *    + Wanting to learn FreeRTOS or multitasking in general quickly       *
    *    + Looking for basic training,                                        *
    *    + Wanting to improve your FreeRTOS skills and productivity           *
    *                                                                         *
    * then take a look at the FreeRTOS books - available as PDF or paperback  *
    *                                                                         *
    *        "Using the FreeRTOS Real Time Kernel - a Practical Guide"        *
    *                  http://www.FreeRTOS.org/Documentation                  *
    *                                                                         *
    * A pdf reference manual is also available.  Both are usually delivered   *
    * to your inbox within 20 minutes to two hours when purchased between 8am *
    * and 8pm GMT (although please allow up to 24 hours in case of            *
    * exceptional circumstances).  Thank you for your support!                *
    *                                                                         *
    ***************************************************************************
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION				1
#define configUSE_MUTEXES					1
#define configUSE_RECURSIVE_MUTEXES 		0
#define configUSE_COUNTING_SEMAPHORES		0
#define configUSE_TIMERS 					0
#define configUSE_IDLE_HOOK					0
#define configUSE_TICK_HOOK					0
#define configUSE_MALLOC_FAILED_HOOK 		1
#define configUSE_TRACE_FACILITY			0
#define configUSE_16_BIT_TICKS				0
#define configUSE_APPLICATION_TASK_TAG		0

#define configCPU_CLOCK_HZ					50000000
#define configTICK_RATE_HZ					5000
#define configMINIMAL_STACK_SIZE			64
#define configTOTAL_HEAP_SIZE				1468
#define configMAX_TASK_NAME_LEN				10
#define configIDLE_SHOULD_YIELD				1

#define configMAX_PRIORITIES				4

#define configUSE_CO_ROUTINES 				0
#define configMAX_CO_ROUTINE_PRIORITIES 	2

#define configCHECK_FOR_STACK_OVERFLOW		2		/* Check stack overflows */
#define configQUEUE_REGISTRY_SIZE			10
#define configGENERATE_RUN_TIME_STATS		0
#define configTIMER_TASK_PRIORITY			10
#define configTIMER_QUEUE_LENGTH			10
#define configTIMER_TASK_STACK_DEPTH		120

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskCleanUpResources			0
#define INCLUDE_vTaskDelay						1
#define INCLUDE_vTaskDelayUntil					1
#define INCLUDE_vTaskDelete						0
#define INCLUDE_xTaskGetCurrentTaskHandle		1
#define INCLUDE_xTaskGetIdleTaskHandle			1
#define INCLUDE_xTaskGetSchedulerState			1
#define INCLUDE_xTimerGetTimerDaemonTaskHandle	1
#define INCLUDE_uxTaskGetStackHighWaterMark		1
#define INCLUDE_uxTaskPriorityGet				1
#define INCLUDE_vTaskPrioritySet				1
#define INCLUDE_xTaskResumeFromISR				0
#define INCLUDE_vTaskSuspend					1
#define INCLUDE_pcTaskGetTaskName				1
#define INCLUDE_xTaskGetIdleTaskHandle			1
#define INCLUDE_xTimerGetTimerDaemonTaskHandle	1

/* port specific configurations */
/* ARM Cortex-M3 */
#define configKERNEL_INTERRUPT_PRIORITY 		255
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	191 /* equivalent to 0xa0, or priority 5. */
/* Microblaze */
#define configINSTALL_EXCEPTION_HANDLERS		1
#define configINTERRUPT_CONTROLLER_TO_USE		XPAR_INTC_SINGLE_DEVICE_ID

#endif /* FREERTOS_CONFIG_H */
