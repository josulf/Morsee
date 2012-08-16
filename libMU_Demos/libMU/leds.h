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
#ifndef LED_H_
#define LED_H_

/**
 * Constantes usadas en la gestion de botones
 */
typedef enum _libMU_LED_Identifiers_t {
	LED_1	= 0x01,
	LED_2	= 0x04,
	LED_3 	= 0x08
} libMU_LED_Identifiers_t;

/**
 * Inicializa el HW de los leds de la placa
 */
void	libMU_LED_Initialize(void);

/**
 * Inicializa el HW de los leds de la placa (del interface ethernet LED_2 y LED_3)
 */
void	libMU_LED_InitializeEth(void);

/**
 * Enciende la luz LED correspondiente
 * @param	led		Código del LED a encender
 */
void	libMU_LED_On(libMU_LED_Identifiers_t led);

/**
 * Apaga la luz LED correspondiente
 * @param	led		Código del LED a apagar
 */
void	libMU_LED_Off(libMU_LED_Identifiers_t led);

/**
 * Conmuta la luz LED correspondiente
 * @param	led		Código del LED a conmutar
 */
void	libMU_LED_Toggle(libMU_LED_Identifiers_t led);

#endif /*LED_H_*/
/**
 * @}
 */
