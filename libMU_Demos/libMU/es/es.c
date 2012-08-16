/**
 * @addtogroup	libMU_Button
 * Libreria para el uso sencillo de los botones
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
#include <driverlib/gpio.h>		// General Purpose IO functions
#include <libMU/es.h>

/**
 * Variable donde se guarda el estado de los botones para detectar flancos de subida y bajada
 */
libMU_GPIO_t libMU_GPIO_PortStatus[GPIO_PORTS];

/**
 * Local variable file for port constants
 */
typedef struct _libMU_GPIO_Port_t {
	uint32_t	periph_code, port_base;
} libMU_GPIO_Port_t;
static const libMU_GPIO_Port_t libMU_GPIO_Ports[GPIO_PORTS] = {
		{ SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE },
		{ SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE },
		{ SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE },
		{ SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE },
		{ SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE },
		{ SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE },
		{ SYSCTL_PERIPH_GPIOG, GPIO_PORTG_BASE }
};


/**
 * Initialize the board pins as inputs or outputs
 * @param	port_and_pin_ID	Port and pin identifications
 * @param	type			Specify the pin types
 * @note					Only one port can be initialized at a time
 * @see 	libMU_GPIO_Type_t
 */
void 	libMU_GPIO_Initialize( libMU_GPIO_ID_t port_and_pins, libMU_GPIO_Type_t type )
{
	unsigned int	port = (port_and_pins >> GPIO_PORT_POS);
	unsigned int	pins = libMU_GPIO_GetBitMask( port_and_pins );
	if( port >= GPIO_PORTS ) return;
    SysCtlPeripheralEnable	( libMU_GPIO_Ports[port].periph_code );
    if( type & GPIO_MASK_INOUT ) {
//    	GPIODirModeSet	( libMU_GPIO_Ports[port].port_base, pins, GPIO_DIR_MODE_OUT );
    }else{
    	GPIODirModeSet	( libMU_GPIO_Ports[port].port_base, pins, GPIO_DIR_MODE_IN );
    }
    switch( type & GPIO_MASK_TYPE ) {
    case GPIO_TYPE_WEAK_PULLDOWN:
        GPIOPadConfigSet( libMU_GPIO_Ports[port].port_base, pins, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD );
        break;
    case GPIO_TYPE_WEAK_PULLUP:
        GPIOPadConfigSet( libMU_GPIO_Ports[port].port_base, pins, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD );
        break;
    default:
        GPIOPadConfigSet( libMU_GPIO_Ports[port].port_base, pins, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD );
    }
    libMU_GPIO_PortStatus[port] = 0;
}

/**
 * Read the status of the input pins
 * @param	port	Code of the port to read
 * @return			Returns the status of the given port
 * @note			This function can only read pins of the same port
 */
libMU_GPIO_t	libMU_GPIO_GetPortStatus( libMU_GPIO_ID_t portID )
{
	unsigned int	port = ( portID >> GPIO_PORT_POS );
	if( port >= GPIO_PORTS ) return 0;
	return GPIOPinRead( libMU_GPIO_Ports[port].port_base, (unsigned char)-1 );
}

/**
 * Change the status of the output pins
 * @param	port	Code of the port to write
 * @param	value	New status of the port
 * @note			Only output pins are changed
 */
void	libMU_GPIO_SetPortStatus( libMU_GPIO_ID_t portID, unsigned int value )
{
	unsigned int	port = ( portID >> GPIO_PORT_POS );
	if( port >= GPIO_PORTS ) return;
	GPIOPinWrite( libMU_GPIO_Ports[port].port_base, libMU_GPIO_GetBitMask( portID ), value );
}

/**
 * Set the status of the output pins to one
 * @param	pins	Code of the port to write
 * @note			Only output pins are changed
 */
void	libMU_GPIO_SetPinsToOne( libMU_GPIO_ID_t pins  )
{
	unsigned int	port = ( pins >> GPIO_PORT_POS );
	if( port >= GPIO_PORTS ) return;
	GPIOPinWrite( libMU_GPIO_Ports[port].port_base,
			libMU_GPIO_GetBitMask( pins ), (unsigned char)UINT32_MAX );
}

/**
 * Set the status of the output pins to zero
 * @param	pins	Code of the port to write
 * @note			Only output pins are changed
 */
void	libMU_GPIO_SetPinsToZero( libMU_GPIO_ID_t pins  )
{
	unsigned int	port = ( pins >> GPIO_PORT_POS );
	if( port >= GPIO_PORTS ) return;
	GPIOPinWrite( libMU_GPIO_Ports[port].port_base,
			libMU_GPIO_GetBitMask( pins ), 0 );
}

/**
 * Toggle the status of the output pins
 * @param	pins	Code of the port to write
 * @note			Only output pins are changed
 */
void	libMU_GPIO_TogglePins( libMU_GPIO_ID_t pins  )
{
	unsigned int	port = ( pins >> GPIO_PORT_POS );
	if( port >= GPIO_PORTS ) return;
	unsigned int	bits = libMU_GPIO_GetBitMask( pins );
	GPIOPinWrite( libMU_GPIO_Ports[port].port_base, bits,
			GPIOPinRead( libMU_GPIO_Ports[port].port_base, bits ) ^ bits );
}

/**
 * @}
 */
