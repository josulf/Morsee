/**
 * @addtogroup	libMU_AD
 * libreria para el uso sencillo de los convertidores AD
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2012/5/28
 * @copyright	BSDL
 ********************************************************************
 */

#ifndef ANALOGICO_DIGITAL_H_
#define ANALOGICO_DIGITAL_H_

/**
 * Constantes usadas en la conversion analógico/digital
 */
enum libMU_AD_Constantes {
	ADC_CANAL_0 = 0,
	ADC_CANAL_1 = 1,
	ADC_CANAL_2 = 2,
	ADC_CANAL_3 = 3,
	ADC_CANAL_TS = 4
};

/**
 * Inicializa el HW de la conversion analogico digital
 */
void 	libMU_AD_Initialize(void);

/**
 * Inicia la conversión de todos los canales a la vez
 */
void	libMU_AD_StartConversion(void);

/**
 * libMU_AD_Obtener_valor_canal
 * @param	canal	Valor de canal ( ADC_CANAL_0, ADC_CANAL_1, ADC_CANAL_2, ADC_CANAL_3, ADC_CANAL_TS )
 * @return			Valor convertido
 * NOTAS:
 * - Un valor 0 corresponde a 0 voltios en el pin correspondiente
 * - Un valor 1024 corresponde a 3.0 voltions en el pin correspondiente
 * - Esta funcion tarda entre 0 y 8us en devolver el resultado
 */
unsigned long libMU_AD_GetChannelValue(int canal);

#endif /*ANALOGICO_DIGITAL_H_*/
/**
 * @}
 */
