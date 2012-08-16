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
#ifndef BOTONES_H_
#define BOTONES_H_

/**
 * Constantes usadas en la gestion de botones
 */
typedef enum _libMU_Button_Identifiers_t {
	BUTTON_UP		= 0x01,
	BUTTON_DOWN		= 0x02,
	BUTTON_LEFT 	= 0x04,
	BUTTON_RIGHT	= 0x08,
	BUTTON_SELECT	= 0x10,

	BOTON_ARRIBA	= BUTTON_UP,
	BOTON_ABAJO		= BUTTON_DOWN,
	BOTON_IZQUIERDA = BUTTON_LEFT,
	BOTON_DERECHA	= BUTTON_RIGHT,
	BOTON_SELECT	= BUTTON_SELECT
} libMU_Button_Identifiers_t;

/**
 * Tipo de datos especial para registrar el estado de los botones
 */
typedef unsigned int libMU_Button_t;

/**
 * Variable donde se guarda el estado de los botones para detectar flancos de subida y bajada
 */
extern	libMU_Button_t	libMU_ButtonState;

/**
 * Inicializa el HW de los botones de la placa
 */
void 	libMU_Button_Initialize(void);

/**
 * Lee el estado de los botones de la placa
 */
libMU_Button_t	libMU_Button_GetFullStatus(void);

/**
 * Comprueba si el boton correspondiente esta pulsado
 * @param	button	Código del boton a comprobar
 * @return			false si no pulsado, true si pulsado
 */
static inline unsigned int	libMU_Button_GetStatus( libMU_Button_Identifiers_t button )
{
	return ( libMU_Button_GetFullStatus() & button );
}

/**
 * Comprueba si se acaba de pulsar el boton correspondiente
 * @param	button	Código del boton a comprobar
 * @return			true si se ha pulsado, si no false
 * @note
 * - Se detecta el flanco de subida en la señal correspondiente
 */
static inline unsigned int	libMU_Button_Pressed( libMU_Button_Identifiers_t button )
{
	libMU_Button_t b_prv = libMU_ButtonState & button;
	libMU_Button_t b = libMU_Button_GetFullStatus() & button;
	// Actualiza estado del boton
	libMU_ButtonState = ( libMU_ButtonState & ~button ) | b;
	return ( b & ~b_prv );
}

/**
 * Comprueba si se acaba de soltar el boton correspondiente
 * @param	button	Código del boton a comprobar
 * @return			true si se ha soltado, si no false
 * @note
 * - Se detecta el flanco de bajada en la señal correspondiente
 */
static inline unsigned int	libMU_Button_Depressed( libMU_Button_Identifiers_t button )
{
	libMU_Button_t b_prv = libMU_ButtonState & button;
	libMU_Button_t b = libMU_Button_GetFullStatus() & button;
	// Actualiza estado del boton
	libMU_ButtonState = ( libMU_ButtonState & ~button ) | b;
	return ( ~b & b_prv );
}

#endif /*BOTONES_H_*/
/**
 * @}
 */
