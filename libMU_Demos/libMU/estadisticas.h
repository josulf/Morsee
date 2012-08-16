/**
 * @addtogroup	libMU_Statistics
 * Librería para el analisis estadístico sencillo
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef ESTADISTICAS_H_
#define ESTADISTICAS_H_

#include <stdint.h>
#include <libMU/cronometro.h>

/**
 * Tipos de datos usados en la gestion del cronometro
 */
typedef struct _libMU_Statistics_t {
	uint64_t	min;			// Valor minimo de tiempo medido
	uint64_t	max;			// Valor maximo de tiempo medido
	uint32_t	count;			// Numero de tiempos medidos en las estadisticas
	uint64_t	sum;			// Suma de tiempos medidos para calculo del valor medio
	uint64_t	sumsquare;		// Suma de cuadrados de tiempos medidos para calculo de desviación estandar
} libMU_Statistics_t;

/**
 * Inicializa las variables estadísticas
 * @param	est		Dirección de la variable donde se va a guardar los datos estadísticos
 */
void	libMU_Statistics_Initialize( libMU_Statistics_t* est );

/**
 * Actualiza las variables estadísticas
 * @param	est		Dirección de la variable donde se va a guardar los datos estadísticos
 * @param	tiempo	Medida para actualizar las estadísticas
 */
void	libMU_Statistics_Update(
					libMU_Statistics_t*		est,
					libMU_Stopwatch_Time_t	tiempo );

/**
 * Valor máximo de las medidas
 * @param	est		Drección de la variable donde están los datos estadísticos
 * @return			Valor máximo de las medidas
 */
static inline libMU_Stopwatch_Time_t	libMU_Statistics_GetMaxValue( libMU_Statistics_t* est )
{
	return (libMU_Stopwatch_Time_t)est->max;
}

/**
 * Valor mínimo de las medidas
 * @param	est		Dirección de la variable donde están los datos estadísticos
 * @return			Valor mínimo de las medidas
 */
static inline libMU_Stopwatch_Time_t	libMU_Statistics_GetMinValue( libMU_Statistics_t* est )
{
	return (libMU_Stopwatch_Time_t)est->min;
}

/**
 * Valor máximo de las medidas
 * @param	est		Dirección de la variable donde están los datos estadísticos
 * @return			Valor medio de las medidas
 */
static inline libMU_Stopwatch_Time_t	libMU_Statistics_GetMeanValue( libMU_Statistics_t* est )
{
	return (libMU_Stopwatch_Time_t)(est->sum/est->count);
}

/**
 * Varianza de las medidas
 * @param	est		Dirección de la variable donde están los datos estadísticos
 * @return			Varianza de las medidas
 */
static inline double	libMU_Statistics_GetVariance( libMU_Statistics_t* est )
{
	libMU_Stopwatch_Time_t t = libMU_Statistics_GetMeanValue( est );
	double var = ((double)est->sumsquare) / ((double)est->count) - t.time * t.time;
	return var;
}

/**
 * Desviación estandar de las medidas
 * @param	est		Dirección de la variable donde están los datos estadísticos
 * @return			Desviación estandar de las medidas
 */
static inline double	libMU_Statistics_GetStdDev( libMU_Statistics_t* est )
{
	return sqrt( libMU_Statistics_GetVariance(est) );
}

#endif /*ESTADISTICAS_H_*/
/**
 * @}
 */
