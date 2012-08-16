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
#include <stdint.h>
#include <math.h>
#include <libMU/estadisticas.h>

/**
 * Inicializa las variables estadísticas
 * @param	est		Dirección de la variable donde se va a guardar los datos estadísticos
 */
void	libMU_Statistics_Initialize( libMU_Statistics_t* est )
{
	est->max = 0;
	est->min = UINT64_MAX;
	est->count = 0;
	est->sum = 0;
	est->sumsquare = 0;
}

/**
 * Inicializa las variables estadísticas
 * @param	est		Dirección de la variable donde se va a guardar los datos estadísticos
 * @param	tiempo	Medida para actualizar las estadísticas
 */
void	libMU_Statistics_Update(
					libMU_Statistics_t*		est,
					libMU_Stopwatch_Time_t	tiempo )
{
	if( tiempo.time > est->max ) est->max = tiempo.time;
	if( tiempo.time < est->min ) est->min = tiempo.time;
	
	uint64_t tmp = tiempo.time * tiempo.time;		
	if( (est->sumsquare + tmp) < est->sumsquare ||
		(est->sum + tiempo.time) < est->sum ) {
		est->count = est->sum = est->sumsquare = 0;	// Reset if overflow
	}
	est->sumsquare	+= tmp;
	est->sum 		+= tiempo.time;
	est->count++;
}

/**
 * @}
 */
