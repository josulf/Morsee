/**
 * @addtogroup	libMU_Stopwatch
 * Librería para la medida de tiempos sencilla
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef CRONOMETRO_H_
#define CRONOMETRO_H_

#include <stdint.h>

#define	USAR_64_BITS
#undef	USAR_32_BITS

/**
 * Constantes usadas en la gestion de botones
 */
enum libMU_Stopwatch_Constants {
	CRONO_FREC			= 50000000,		// Frecuencia de contaje 50MHz
	CRONO_CUENTA_MAX	= 0x80000000	// Cuenta maxima de HW = (20ns * 2^31) ~ 42,95s	
};

/**
 * Tipos de datos usados en la gestion del cronometro
 */
#ifdef USAR_64_BITS
typedef uint64_t libMU_Time_t;
#define	LIBMU_TIME_SIZE	2
#else
typedef uint32_t libMU_Time_t;
#define	LIBMU_TIME_SIZE	1
#endif

typedef union _libMU_Stopwatch_Time_t {
	libMU_Time_t	time;
	uint32_t		t32[LIBMU_TIME_SIZE];	
} libMU_Stopwatch_Time_t;

/**
 * Inicializa el HW para la medidad de tiempos
 */
void	libMU_Stopwatch_Initialize(void);

/**
 * Empieza el cronometraje
 * @param	tiempo	Dirección de la variable donde se va a guardar el tiempo inicial
 */
void	libMU_Stopwatch_Start(libMU_Stopwatch_Time_t* tiempo);

/**
 * Tiempo intermedio el cronometraje
 * @param	tiempo	Dirección de la variable donde se va a guardar el tiempo intermedio
 * @return			Devuelve el tiempo transcurrido desde la anterior medida de tiempo
 */
libMU_Time_t	libMU_Stopwatch_Peek(libMU_Stopwatch_Time_t* tiempo);

/**
 * Termina el cronometraje
 * @param	tiempo	Dirección de la variable donde se va a guardar el tiempo final
 * @return			Devuelve el tiempo transcurrido desde la anterior medida de tiempo en ms
 */
static inline libMU_Time_t	libMU_Cronometro_Tiempo_intermedio_ms(libMU_Stopwatch_Time_t* tiempo)
{
	return libMU_Stopwatch_Peek( tiempo ) / ( CRONO_FREC * 2 / 1000 );
}

#endif /*CRONOMETRO_H_*/
/**
 * @}
 */
