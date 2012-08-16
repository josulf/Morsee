//*****************************************************************************
//
// startup_ccs.c - Startup code for use with TI's Code Composer Studio.
//
// Copyright (c) 2006-2010 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 6594 of the EK-LM3S8962 Firmware Package.
//
//*****************************************************************************

// Make sure we have a compiler that support the "__attribute__((weak))" construct
//#if __TI_COMPILER_VERSION__ > 4008000
#if 0

//*****************************************************************************
//
// Forward declaration of the FreeRTOS handlers.
//
//*****************************************************************************
extern void EthernetIntHandler(void) __attribute__((weak));
void EthernetIntHandler(void)
{
	for(;;) { }	// Enter an infinite loop.
}
extern void ADC_SampleSequencer1_IntHandler(void) __attribute__((weak));
void ADC_SampleSequencer1_IntHandler(void)
{
	for(;;) { }	// Enter an infinite loop.
}

//*****************************************************************************
//
// Forward declaration of the FreeRTOS handlers.
//
//*****************************************************************************
extern void xPortPendSVHandler( void ) __attribute__((weak));
void xPortPendSVHandler(void)
{
	for(;;) { }	// Enter an infinite loop.
}
extern void xPortSysTickHandler( void ) __attribute__((weak));
void xPortSysTickHandler(void)
{
	for(;;) { }	// Enter an infinite loop.
}
extern void vPortSVCHandler( void ) __attribute__((weak));
void vPortSVCHandler(void)
{
	for(;;) { }	// Enter an infinite loop.
}

#endif
