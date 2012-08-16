/**
 * @addtogroup	libMU_PWM
 * Librería para el uso de retardos
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef RETARDO_H_
#define RETARDO_H_

/**
 * @param	repeticiones	Número de veces que se repite el retardo
 * @note
 * Cada retardo necesita aproximadamente 3 ciclos de reloj
 * (es decir, 60ns por retardo a 50MHz)
 */
static inline void libMU_Retardo(unsigned long repeticiones)
{
	extern void SysCtlDelay( unsigned long );
	SysCtlDelay( repeticiones );
}

#endif /*RETARDO_H_*/
/**
 * @}
 */
