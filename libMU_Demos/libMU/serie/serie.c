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
#include <string.h>
#include <inc/hw_types.h>			// New data types
#include <inc/hw_uart.h>			// uart constants
#include <inc/hw_memmap.h>			// Peripheral memory map
#include <driverlib/sysctl.h>		// System control functions
#include <driverlib/gpio.h>			// General Purpose IO functions
#include <driverlib/uart.h>			// UART functions
#include <driverlib/interrupt.h>	// Interrupt functions
#include <libMU/serie.h>
#include <libMU/FIFO.h>

/**
 * Base address of UART device to use
 */
#define	UART_BASE_ADDRESS	UART0_BASE

/**
 * Local variables
 */
uint32_t				libMU_Serial_DataLost;
int						libMU_Serial_UsingInterrupts;	// Variable to signal the use of interruts in the serial port
libMU_Serial_Handler_t	libMU_Serial_RxHandler;
libMU_FIFO_t*			libMU_Serial_RxQueue;
libMU_FIFO_t*			libMU_Serial_TxQueue;

/**
 * Local function prototypes
 */
void libMU_Serial_HandlerFunction( void );

/**
 * Variable to signal the use of interruts in the serial port
 */
int								libMU_Serial_UsingInterrupts;
static libMU_Serial_Handler_t	libMU_Serial_RxHandler;
static libMU_FIFO_t*			libMU_Serial_RxQueue;
static libMU_FIFO_t*			libMU_Serial_TxQueue;

/**
 * Inicializa el HW de la línea serie
 * @param baudrate	Velocidad de comunicaciones
 * @note			The CPU frequency must be set before calling this function
 */
void 	libMU_Serial_Initialize(unsigned int baudrate)
{
    // Enable GPIO port A which is used for UART0 pins.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Select the alternate (UART) function for these pins.
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Initialize the UART 0 for console I/O at given baudrate.
    if( baudrate == 0 ) baudrate = 115200;	// default baudrate
    SysCtlPeripheralEnable( SYSCTL_PERIPH_UART0 );
    UARTConfigSetExpClk( UART_BASE_ADDRESS, SysCtlClockGet(), baudrate,
    	UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE );
    UARTEnable( UART_BASE_ADDRESS );
    // Setup initial interrupt use status
    libMU_Serial_UsingInterrupts = 0;
    libMU_Serial_RxHandler = NULL;
    libMU_Serial_DataLost = 0;
}

/**
 * Checks if the transmission FIFO queue is full
 * @return 		true if queue full
 */
static inline int	libMU_Serial_IsTxFifoFull(void)
{
	// Check if TX FIFO is full
	return  ( HWREG( UART_BASE_ADDRESS + UART_O_FR ) & UART_FR_TXFF );
}

/**
 * Checks if the reception FIFO queue is empty
 * @return 		true if queue empty
 */
static inline int	libMU_Serial_IsRxFifoEmpty(void)
{
	// Check if RX FIFO is empty
	return  ( HWREG( UART_BASE_ADDRESS + UART_O_FR ) & UART_FR_RXFE );
}

/**
 * Sends a character to the UART HW (low level access)
 * @param	c		Character to send
 */
static inline void libMU_Serial_SendCharHW(const char c)
{
	HWREG( UART_BASE_ADDRESS + UART_O_DR ) = c;
}

/**
 * Reads a character from the UART HW (low level access)
 * @return			Read character
 */
static inline uint8_t	libMU_Serial_GetCharHW(void)
{
	return HWREG( UART_BASE_ADDRESS + UART_O_DR );
}

/**
 * Envia un numero de caracteres a la línea serie
 * @param text		Texto enviado a la línea serie
 * @param len		Número de caracteres a enviar a la línea serie
 * @return			Número de caracteres enviados
 */
int	libMU_Serial_SendChars(const char* text, int len)
{
    int i;

    // Check for valid UART base address, and valid arguments.
    if( text == NULL ) return 0;

    // Send the characters
    for(i = 0; i < len; i++)
    {
        // Send the character to the UART output.
        libMU_Serial_SendChar( text[i] );
    }
    // Return the number of characters written.
    return(i);
}

/**
 * Envia un texto a la línea serie
 * @param text		Texto enviado a la línea serie
 * @return			Número de caracteres enviados
 * @note			A '\r' character is sent before every '\n' character
 */
int	libMU_Serial_SendString(const char* text)
{
	uint32_t	len, i;
	if( text == NULL ) return 0;
	len = strlen(text);
    // Send the characters
    for(i = 0; i < len; i++)
    {
        // If the character to the UART is \n, then add a \r before it so that
        // \n is translated to \r\n in the output.
        if(text[i] == '\n')
        {
            libMU_Serial_SendChar('\r');
        }
        // Send the character to the UART output.
        libMU_Serial_SendChar( text[i] );
    }
    // Return the number of characters written.
    return(i);
}

/**
 * Lee un texto de la línea serie
 * @param donde		Dirección donde se guarda texto leído desde la línea serie
 * @param tamano	Cantidad máxima de caracteres que se pueden leer
 * @return			Número de caracteres leídos
 */
int	libMU_Serial_GetString(char* donde, int tamano)
{
    unsigned long ulCount = 0;
    char cChar;
    static char bLastWasCR = 0;

    // Check for valid UART base address, and valid arguments.
    if( donde == NULL || tamano <= 0 ) return 0;

    // Adjust the length back by 1 to leave space for the trailing null terminator.
    tamano--;

    // Process characters until a newline is received.
    for(;;)
    {
        // Read the next character from the console.
        libMU_Serial_GetChar( &cChar );
        // See if the backspace key was pressed.
        if(cChar == '\b')
        {
            // If there are any characters already in the buffer, then delete
            // the last.
            if(ulCount)
            {
                // Rub out the previous character.
                libMU_Serial_SendChars( "\b \b", 3 );
                // Decrement the number of characters in the buffer.
                ulCount--;
            }
            // Skip ahead to read the next character.
            continue;
        }
        // If this character is LF and last was CR, then just gobble up the
        // character because the EOL processing was taken care of with the CR.
        if((cChar == '\n') && bLastWasCR)
        {
            bLastWasCR = 0;
            continue;
        }
        // See if a newline or escape character was received.
        if((cChar == '\r') || (cChar == '\n') || (cChar == 0x1b))
        {
            // If the character is a CR, then it may be followed by a LF which
            // should be paired with the CR.  So remember that a CR was
            // received.
            if(cChar == '\r')
            {
                bLastWasCR = 1;
            }
            // Stop processing the input and end the line.
            break;
        }
        // Process the received character as long as we are not at the end of
        // the buffer.  If the end of the buffer has been reached then all
        // additional characters are ignored until a newline is received.
        if( ulCount < tamano )
        {
            // Store the character in the caller supplied buffer.
        	donde[ulCount] = cChar;
            // Increment the count of characters received.
            ulCount++;
            // Reflect the character back to the user.
            libMU_Serial_SendChar( cChar );
        }
    }
    // Add a null termination to the string.
    donde[ulCount] = 0;
    // Send a CRLF pair to the terminal to end the line.
    libMU_Serial_SendChars( "\n", 1 );
    // Return the count of chars in the buffer, not counting the trailing 0.
    return(ulCount);
}

/**
 * Envía un caracter a la línea serie
 * @param c			Caracter enviado a la línea serie
 * @return			Número de caracteres enviados
 * @note			Esta función bloquea la ejecución hasta que se pueda enviar el carácter
 */
int	libMU_Serial_SendChar(const char c)
{
	if( libMU_Serial_UsingInterrupts ) {
		while( libMU_FIFO_IsFull( libMU_Serial_TxQueue ) ) {};
		libMU_FIFO_PutChar( libMU_Serial_TxQueue, c );
		return 1;
	}
	while( libMU_Serial_IsTxFifoFull() ) {}
	libMU_Serial_SendCharHW( c );
	return 1;
}

/**
 * Envía un caracter a la línea serie sin bloquear la ejecución del programa
 * @param	c		Caracter enviado a la línea serie
 * @return			Número de caracteres enviados
 * @note			Esta función NO bloquea la ejecución
 */
int	libMU_Serial_SendChar_NB(const char c)
{
	if( libMU_Serial_UsingInterrupts ) {
		if( libMU_FIFO_IsFull( libMU_Serial_TxQueue ) ) return 0;
		libMU_FIFO_PutChar( libMU_Serial_TxQueue, c );
		return 1;
	}
	if( libMU_Serial_IsTxFifoFull() ) return 0;
	libMU_Serial_SendCharHW( c );
	return 1;
}

/**
 * Lee un carácter de la línea serie
 * @param donde		Dirección donde se guarda el carácter leído desde la línea serie
 * @return			Número de caracteres leídos
 * @note			Esta función bloquea la ejecución hasta que se reciba un carácter
 */
int	libMU_Serial_GetChar(char* donde)
{
	if( libMU_Serial_UsingInterrupts ) {
		while( libMU_FIFO_IsEmpty( libMU_Serial_RxQueue ) ) {};
		*donde = libMU_FIFO_GetChar( libMU_Serial_RxQueue );
		return 1;
	}
	while( libMU_Serial_IsRxFifoEmpty() ) {}
	*donde = libMU_Serial_GetCharHW();
	return 1;
}

/**
 * Lee un carácter de la línea serie sin bloquear la ejecución del programa
 * @param donde		Dirección donde se guarda el carácter leído desde la línea serie
 * @return			Número de caracteres leídos
 * @note			Esta función NO bloquea la ejecución
 */
int	libMU_Serial_GetChar_NB(char* donde)
{
	if( libMU_Serial_UsingInterrupts ) {
		if( libMU_FIFO_IsEmpty( libMU_Serial_RxQueue ) ) return 0;
		*donde = libMU_FIFO_GetChar( libMU_Serial_RxQueue );
		return 1;
	}
	if( libMU_Serial_IsRxFifoEmpty() ) return 0;
	*donde = libMU_Serial_GetCharHW();
	return 1;
}

/**
 * Set interrupt handler for the serial port and enable interrupt mode
 * @param	out_buffer		Pointer to intermediate buffer for data transmission
 * @param	out_buffer_len	Size of intermediate buffer for data transmission
 * @param	function		Pointer to reception handler function
 */
void	libMU_Serial_SetHandler( uint8_t* out_buffer, uint32_t out_buffer_len,
								 libMU_Serial_Handler_t function )
{
	libMU_Serial_TxQueue = libMU_FIFO_Initialize( out_buffer, out_buffer_len, sizeof(uint8_t) );
	if( libMU_Serial_TxQueue == NULL ) return;
	UARTIntRegister	( UART_BASE_ADDRESS, libMU_Serial_HandlerFunction );
	UARTIntClear	( UART_BASE_ADDRESS, UART_INT_RX );
	UARTIntEnable	( UART_BASE_ADDRESS, UART_INT_RX );
	libMU_Serial_UsingInterrupts = 1;
	libMU_Serial_RxHandler = function;
}

/**
 * Set interrupt handler for the serial port and enable interrupt mode
 * @param	out_buffer		Pointer to intermediate buffer for data transmission
 * @param	out_buffer_len	Size of intermediate buffer for data transmission
 * @param	in_buffer		Pointer to intermediate buffer for data reception
 * @param	in_buffer_len	Size of intermediate buffer for data reception
 */
void	libMU_Serial_SetBufferedTransmission(
				uint8_t* out_buffer, uint32_t out_buffer_len,
				uint8_t* in_buffer,	 uint32_t in_buffer_len )
{
	libMU_Serial_TxQueue = libMU_FIFO_Initialize( out_buffer, out_buffer_len, sizeof(uint8_t) );
	if( libMU_Serial_TxQueue == NULL ) return;
	libMU_Serial_RxQueue = libMU_FIFO_Initialize( in_buffer, in_buffer_len, sizeof(uint8_t) );
	if( libMU_Serial_RxQueue == NULL ) return;
	UARTIntRegister	( UART_BASE_ADDRESS, libMU_Serial_HandlerFunction );
	UARTIntClear	( UART_BASE_ADDRESS, UART_INT_RX|UART_INT_TX );
	UARTIntEnable	( UART_BASE_ADDRESS, UART_INT_RX|UART_INT_TX );
	libMU_Serial_UsingInterrupts = 1;
	libMU_Serial_RxHandler = NULL;
}

/**
 * Interrupt service routine handler for serial port
 */
void	libMU_Serial_HandlerFunction( void )
{
	uint32_t	Status = UARTIntStatus( UART_BASE_ADDRESS, false );
	if( Status & UART_INT_RX ) {
		UARTIntClear( UART_BASE_ADDRESS, UART_INT_RX );
		if( libMU_Serial_RxHandler != NULL ) {
			libMU_Serial_RxHandler();
		}else{
			do{
				char c = libMU_Serial_GetCharHW();
				if(!libMU_FIFO_IsFull ( libMU_Serial_RxQueue ) )
					libMU_FIFO_PutChar( libMU_Serial_RxQueue, c );
				else
					libMU_Serial_DataLost++;
			}while( !libMU_Serial_IsRxFifoEmpty() );
		}
	}
	if( Status & UART_INT_TX ) {
		UARTIntClear( UART_BASE_ADDRESS, UART_INT_TX );
		while( libMU_FIFO_Count( libMU_Serial_TxQueue ) > 0 ) {
			char c = libMU_FIFO_GetChar( libMU_Serial_TxQueue );
			libMU_Serial_SendCharHW( c );
			if( libMU_Serial_IsTxFifoFull() ) break;
		}
	}
}

/**
 * @}
 */
