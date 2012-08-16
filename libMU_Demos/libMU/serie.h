/**
 * @addtogroup libMU_Serial
 * Library for easy access to the serial port
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef SERIE_H_
#define SERIE_H_

#include <stdint.h>
/**
 * Variable to indicate if the serial library is using interrupts or not
 */
extern int		libMU_Serial_UsingInterrupts;

/**
 * Function pointer for interrupt handler functions
 */
typedef	void (*libMU_Serial_Handler_t)(void);

/**
 * Inicializa el HW de la linea serie
 * @param	baudrate	Velocidad de comunicaciones
 */
void 	libMU_Serial_Initialize(unsigned int baudrate);

/**
 * Envia un texto a la linea serie
 * @param	texto	Texto enviado a la linea serie
 * @return			Numero de caracteres enviados
 */
int	libMU_Serial_SendString(const char* texto);

/**
 * Lee un texto de la linea serie
 * @param	donde	Dirección donde se guarda texto leído desde la linea serie
 * @param	tamano	Cantidad máxima de caracteres que se pueden leer
 * @return			Numero de caracteres leidos
 */
int	libMU_Serial_GetString(char* donde, int tamano);

/**
 * Envía un caracter a la línea serie
 * @param c			Caracter enviado a la línea serie
 * @return			Numero de caracteres enviados
 * @note			Esta función bloquea la ejecución hasta que se pueda enviar el carácter
 */
int	libMU_Serial_SendChar(const char c);

/**
 * Envia un caracter a la linea serie sin bloquear ejecución
 * @param	c		Caracter enviado a la linea serie
 * @return			Numero de caracteres enviados
 * @note			Esta función NO bloquea la ejecución
 */
int	libMU_Serial_SendChar_NB(const char c);

/**
 * Envia un caracter a la linea serie sin bloquear ejecución
 * @param	c		Caracter enviado a la linea serie
 * @return			Numero de caracteres enviados
 * @note			Esta función NO bloquea la ejecución
 */
int	libMU_Serial_SendCharFromISR_NB(const char c);

/**
 * Lee un carácter de la linea serie
 * @param	donde	Dirección donde se guarda el carácter leído desde la linea serie
 * @return			Numero de caracteres leidos
 * @note			Esta función bloquea la ejecución hasta que se reciba un carácter
 */
int	libMU_Serial_GetChar(char* donde);

/**
 * Lee un carácter de la linea serie sin bloquear ejecución
 * @param	donde	Dirección donde se guarda el carácter leído desde la linea serie
 * @return			Numero de caracteres leidos
 * @note			Esta función NO bloquea la ejecución
 */
int	libMU_Serial_GetChar_NB(char* donde);

/**
 * Lee un carácter de la línea serie sin bloquear la ejecución del programa
 * @param donde		Dirección donde se guarda el carácter leído desde la línea serie
 * @return			Número de caracteres leídos
 * @note			Esta función NO bloquea la ejecución
 */
int	libMU_Serial_GetCharFromISR_NB(char* donde);

/**
 * Set interrupt handler for the serial port and enable interrupt mode
 * @param	out_buffer		Pointer to intermediate buffer for data transmission
 * @param	out_buffer_len	Size of intermediate buffer for data transmission
 * @param	in_buffer		Pointer to intermediate buffer for data reception
 * @param	in_buffer_len	Size of intermediate buffer for data reception
 */
void	libMU_Serial_SetBufferedTransmission(
				uint8_t* out_buffer, uint32_t out_buffer_len,
				uint8_t* in_buffer,	 uint32_t in_buffer_len );

#endif /*SERIE_H_*/
/**
 * @}
 */

