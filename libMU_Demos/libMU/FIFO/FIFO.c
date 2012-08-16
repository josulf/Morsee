/**
 * @addtogroup	libMU_FIFO
 * Library to create and use FIFO queues
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2012/6/05
 * @copyright	BSDL
 ********************************************************************
 */

#include <libMU/FIFO.h>

/**
 * Initialize FIFO queue
 * @param	buffer		Punteros a la memoria intermedia utilizada para el envio de datos
 * @param	size		Dimensión de la memoria intermedia utilizada para el envio de datos
 */
libMU_FIFO_t*	libMU_FIFO_Initialize( uint8_t* buffer, uint32_t size, uint32_t unit_size )
{
	libMU_FIFO_t* fifo;
	if( buffer == NULL ) 				return NULL;
	if( unit_size < 1 )					return NULL;
	if( size < sizeof(libMU_FIFO_t) )	return NULL;
	size -= sizeof(libMU_FIFO_t);
	if( (size % unit_size) != 0 )		return NULL;
	fifo = (libMU_FIFO_t*)buffer;
	fifo->size			= size;
	fifo->next_write 	= 0;
	fifo->next_read		= 0;
	fifo->count			= 0;
	fifo->unit_size		= unit_size;
	return fifo;
}

/**
 * @}
 */
