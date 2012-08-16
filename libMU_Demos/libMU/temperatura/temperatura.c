/**
 * @addtogroup	libMU_Temperatura
 * Librería para el uso sencillo del sensor de temperatura analógico
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 * @note		Necesita la libreria libMU_AD (analogico digital) para funcionar
 ********************************************************************
 */
#include <libMU/analogico_digital.h>

/**
 * @param value, valor a convertir (en formato devuelto por el sensor interno del LM3S8962)
 * @return valor de la temperatura
 * @note
 * - Un valor 0 corresponde a 0ºC
 * - Un valor 100 corresponde a 10ºC
 * - Un valor -100 corresponde a -10ºC
 * - El sensor de temperatura mide la temperatura del procesador!!
 */
int libMU_Temperature_Convert(unsigned long value)
{
	int temp = (int)value;
    temp = 1475 - (temp * 2250) / 1023;	// From stellaris datasheet info
	return temp;
}	

/**
 * @return valor de la temperatura
 * @note
 * - Un valor 0 corresponde a 0ºC
 * - Un valor 100 corresponde a 10ºC
 * - Un valor -100 corresponde a -10ºC
 * - El sensor de temperatura mide la temperatura del procesador!!
 */
int libMU_Temperature_Get()
{
	return libMU_Temperature_Convert( libMU_AD_GetChannelValue( ADC_CANAL_TS ) );
}	

/**
 * @}
 */
