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
#ifndef LIBMU_FIFO_H_
#define LIBMU_FIFO_H_

#include <stdio.h>
#include <stdint.h>

typedef struct _libMU_FIFO_t {
	uint32_t	size;			// Maximum number of elements in FIFO queue
	uint32_t	unit_size;		// Size of one element in FIFO queue
	uint32_t	next_write;		// Index to next free space
	uint32_t	next_read;		// Index to next element to read
	uint32_t	count;			// Number of elements in FIFO queue
	uint8_t		buffer[0];		// FIFO queue data pointer
} libMU_FIFO_t;

/**
 * Initialize FIFO queue
 * @param	buffer		Pointer to FIFO queue memory buffer
 * @param	size		Size of the FIFO queue memory buffer
 * @param	unit_size	Size of each element stored in the memory buffer
 * @return				NULL if buffer size is not big enough
 * @note				The FIFO management data is also stored in the buffer.
 * 						Therefore, the buffer has to be big enough to hold this data
 */
libMU_FIFO_t*	libMU_FIFO_Initialize( uint8_t* buffer, uint32_t size, uint32_t unit_size );

/**
 * @return	The number of elements in FIFO queue
 */
static inline uint32_t	libMU_FIFO_Count( libMU_FIFO_t* fifo )
{
	return fifo->count;
}

/**
 * @return	If FIFO queue is empty
 */
static inline uint32_t	libMU_FIFO_IsEmpty( libMU_FIFO_t* fifo )
{
	return (fifo->count == 0);
}

/**
 * @return	If FIFO queue is full
 */
static inline uint32_t	libMU_FIFO_IsFull( libMU_FIFO_t* fifo )
{
	return (fifo->count == fifo->size);
}

/**
 *
 */
static inline void	libMU_FIFO_PutChar( libMU_FIFO_t* fifo, uint8_t c )
{
	fifo->buffer[ fifo->next_write ] = c;
	fifo->count++;
	fifo->next_write++;
	if( fifo->next_write >= fifo->size ) fifo->next_write = 0;
}

/**
 *
 */
static inline uint8_t	libMU_FIFO_GetChar( libMU_FIFO_t* fifo )
{
	uint8_t c = fifo->buffer[ fifo->next_read ];
	fifo->count--;
	fifo->next_read++;
	if( fifo->next_read >= fifo->size ) fifo->next_read = 0;
	return c;
}

#endif /*LIBMU_FIFO_H_*/
/**
 * @}
 */
