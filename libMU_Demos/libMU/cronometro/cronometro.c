/**
 * @addtogroup	libMU_Stopwatch
 * Library for easy access to the timing capabities
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#include <stdint.h>
#include <inc/hw_types.h>		// New data types
#include <inc/hw_memmap.h>		// Peripheral memory map
#include <driverlib/sysctl.h>	// System control functions
#include <driverlib/timer.h>	// Timer functions
//#include <inc/hw_ints.h>		// Interrupt vector definitions
//#include <driverlib/interrupt.h>// Interrupt functions
#include <libMU/cronometro.h>

/**
 * libMU_Stopwatch: Cuenta estática para usar en la interrupción
 */
uint32_t	libMU_Stopwatch_Count_HighOrder = 0;

/**
 * Inicializa el HW para la medidad de tiempos
 */
void 	libMU_Stopwatch_Initialize(void)
{	
	///////////////////////////////////////////////////////////////////
	// Initialize Timer 1 to measure times 
	SysCtlPeripheralEnable	( SYSCTL_PERIPH_TIMER1 );	// Enable TIMER1
	// Set Timer 1 as a 32 bit periodic timer with maximum count & clear interrupt signal
    TimerControlTrigger		( TIMER1_BASE, TIMER_BOTH, false );	// Disable ADC trigger
    TimerConfigure			( TIMER1_BASE, TIMER_CFG_32_BIT_PER );
    TimerLoadSet			( TIMER1_BASE, TIMER_A, CRONO_CUENTA_MAX );
    TimerIntClear			( TIMER1_BASE, TIMER_TIMA_TIMEOUT );
    TimerEnable				( TIMER1_BASE, TIMER_A );	// Enable TIMER1	
}

/**
 * Empieza el cronometraje
 * @param	time	Dirección de la variable donde se va a guardar el tiempo inicial
 */
void	libMU_Stopwatch_Start(libMU_Stopwatch_Time_t* time)
{
	time->t32[0] = ( TimerValueGet( TIMER1_BASE, TIMER_A ) << 1 );
#ifdef USAR_64_BITS
	time->t32[1] = libMU_Stopwatch_Count_HighOrder;
#endif
}

/**
 * Tiempo intermedio el cronometraje
 * @param	time	Dirección de la variable donde se va a guardar el tiempo intermedio
 * @return			Devuelve el tiempo transcurrido desde la anterior medida de tiempo
 */
libMU_Time_t	libMU_Stopwatch_Peek(libMU_Stopwatch_Time_t* time)
{
	libMU_Stopwatch_Time_t current, diff;
	current.t32[0] = ( TimerValueGet( TIMER1_BASE, TIMER_A ) << 1 );
#ifdef USAR_64_BITS
	current.t32[1] = libMU_Stopwatch_Count_HighOrder;
#endif
	diff.time = current.time - time->time;
	time->time = current.time;
	return diff.time;
}

/**
 * Termina el cronometraje
 * @param	time	Dirección de la variable donde se va a guardar el tiempo final
 * @return			Devuelve el tiempo transcurrido desde la anterior medida de tiempo
 */
libMU_Time_t	libMU_Stopwatch_Stop(libMU_Stopwatch_Time_t* time)
{
	libMU_Stopwatch_Time_t current, diff;
	current.t32[0] = ( TimerValueGet( TIMER1_BASE, TIMER_A ) << 1 );
#ifdef USAR_64_BITS
	current.t32[1] = libMU_Stopwatch_Count_HighOrder;
#endif
	diff.time = current.time - time->time;
	time->time = current.time;
	return diff.time;
}

/**
 * @}
 */
