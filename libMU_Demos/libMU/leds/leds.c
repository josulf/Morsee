/**
 * @addtogroup	libMU_LED
 * Librería para el uso sencillo de los indicadores LED
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#include <inc/hw_types.h>		// New data types
#include <inc/hw_memmap.h>		// Peripheral memory map
#include <driverlib/sysctl.h>	// System control functions
#include <driverlib/gpio.h>		// General Purpose IO functions
#include <libMU/leds.h>

/**
 * Inicializa el HW de los leds de la placa
 */
void	libMU_LED_Initialize(void)
{
    SysCtlPeripheralEnable	( SYSCTL_PERIPH_GPIOF );// Enable port for LEDs
    GPIODirModeSet	( GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_DIR_MODE_OUT );
    GPIOPadConfigSet( GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU );
    // Initial value switched off
    GPIOPinWrite	( GPIO_PORTF_BASE, GPIO_PIN_0, 0xFF );
}

/**
 * Inicializa el HW de los leds de la placa (del interface ethernet LED_2 y LED_3)
 */
void	libMU_LED_InitializeEth(void)
{
	///////////////////////////////////////////////////////////////////
	// 7.- Configure LEDs (PF0/PWM0/, PF2/LED0, PF3/LED1)
	// pins PF2 / PF3 drive the ethernet interface LEDs, they can only be used
	// if the Ethernet interface is not being used
    SysCtlPeripheralEnable	( SYSCTL_PERIPH_GPIOF );// Enable port for LEDs
    GPIODirModeSet	( GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3,
    					GPIO_DIR_MODE_OUT );
    GPIOPadConfigSet( GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3,
    					GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU );
    // Initial value switched off
    GPIOPinWrite	( GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3, 0xFF );
}

/**
 * Enciende la luz LED correspondiente
 * @param	led		Código del LED a encender
 */
void	libMU_LED_On(libMU_LED_Identifiers_t led)
{
    GPIOPinWrite( GPIO_PORTF_BASE, (unsigned char)led, 0 );
}

/**
 * Apaga la luz LED correspondiente
 * @param	led		Código del LED a apagar
 */
void	libMU_LED_Off(libMU_LED_Identifiers_t led)
{
    GPIOPinWrite( GPIO_PORTF_BASE, (unsigned char)led, 0xFF );
}

/**
 * Conmuta la luz LED correspondiente
 * @param	led		Código del LED a conmutar
 */
void	libMU_LED_Toggle(libMU_LED_Identifiers_t led)
{
    GPIOPinWrite( GPIO_PORTF_BASE, (unsigned char)led,
    		GPIOPinRead( GPIO_PORTF_BASE, (unsigned char)led ) ^ led );
}

/**
 * @}
 */
