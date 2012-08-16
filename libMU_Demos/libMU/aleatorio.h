/**
 * @addtogroup	libMU_Random
 * Librería para la generación de números aleatorios
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2012/5/28
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef _RANDOM_H_
#define _RANDOM_H_

/**
 * Añade valores entrópicos al conjunto
 * @param	ulEntropy	New entropy value
 */
void	libMU_Random_AddEntropy(unsigned long ulEntropy);

/**
 * Inicializa el generador de números aleatorios
 */
void 	libMU_Random_Initialize(void);

/**
 * @return	Número aleatorio
 */
unsigned long libMU_Random_GetValue(void);

#endif /*_RANDOM_H_*/
/**
 * @}
 */
