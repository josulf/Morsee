/**
 * @addtogroup	libMU_TS27010
 * Librería para el protocol de multiplexacion 3GPP TS27.010 v10.0.0
 * crea puertos virtuales COM usando una linea serie
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2012/6/05
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef TS27010_H_
#define TS27010_H_

#include <stdint.h>
#include <libMU/ts27010.h>

typedef struct _libMU_FIFO_t {
	uint8_t*	buffer;			// Puntero a los datos de la FIFO
	uint32_t	next_write;		// Indice del siguiente espacio libre
	uint32_t	next_read;		// Indice del siguiente elemento a leer
	uint32_t	count;			// Número de elementos en la FIFO
	uint32_t	size;			// Número máximo de elementos en la FIFO
} libMU_FIFO_t;

/**
 * Tipos de datos usados en la gestion de los canales multiplexados
 * @param	data_fifo	Puntero a la cola FIFO de datos
 */
typedef void (*libMU_TS27010_Handler_t)( libMU_FIFO_t* data_fifo );

/**
 * Inicializa las variables/funciones del protocolo TS 27.010
 * @param	baudrate		Velocidad en baudios de la linea serie multiplexada
 * @param	output_buffer	Punteros a la memoria intermedia utilizada para el envio de datos
 * @param	out_buf_len		Dimensión de la memoria intermedia utilizada para el envio de datos
 * @param	input_buffer	Punteros a la memoria intermedia utilizada para la recepción de datos
 * @param	in_buf_len		Dimensión de la memoria intermedia utilizada para la recepción de datos
 */
void	libMU_TS27010_Initialize( uint32_t baudrate,
								uint8_t* output_buffer, uint32_t out_buf_len,
								uint8_t* input_buffer, uint32_t in_buf_len );

/**
 * Establece un canal de enlace de datos (Data Link Channel)
 * @param	dlc		Numero del puerto virtual para el canal de enlace de datos
 * @return			0 si se ha establecido el canal correctamente
 */
int		libMU_TS27010_Connect( int	dlc );

/**
 * Envía los datos al dlc
 * @param	dlc		Numero del puerto virtual para el canal de enlace de datos
 * @param	data	Puntero a los datos a enviar
 * @param	len		Dimensión (en octetos) de los datos a enviar
 * @return			0 si se ha conseguido introducir los datos en la memoria intermedia
 * @note			Usa una memoria intermedia para los datos
 */
int		libMU_TS27010_Send( int	dlc, uint8_t* data, uint32_t len );

/**
 * Establece la funcion que tratará los datos recibidos desde el dlc
 * @param	dlc		Numero del puerto virtual para el canal de enlace de datos
 * @param	fun		Puntero a la funcion que tratará los datos recibidos
 * @return			0 si se ha conseguido introducir los datos en la memoria intermedia
 * @note			Usa una memoria intermedia para la recepción de datos los datos
 */
void	libMU_TS27010_SetHandler( int	dlc, libMU_TS27010_Handler_t fun );

#endif /*TS27010_H_*/
/**
 * @}
 */
