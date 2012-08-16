/**
 * @addtogroup	libMU_Display
 * Librería para el uso sencillo de la pantalla
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef PANTALLA_H_
#define PANTALLA_H_

#include <stdint.h>
#include <string.h>
#include <drivers/rit128x96x4.h>// OLED functions

/**
 * Constantes usadas en la conversion analógico/digital
 */
enum libMU_Display_Constants_t {
	DISPLAY_RES_X		= 128,	// Resolución horizontal de la pantalla
	DISPLAY_RES_Y		= 96,	// Resolución vertical de la pantalla
	DISPLAY_CHAR_WIDTH	= 6,	// Tamaño horizontal de un caracter en pixeles
	DISPLAY_CHAR_HEIGHT	= 8,	// Tamaño vertical de un caracter en pixeles
	DISPLAY_BPP			= 4		// Bits por pixel de la pantalla
};

/**
 * Tipo de datos para guardar la información sobre una imagen en memoria local
 */
typedef struct _libMU_Display_MemoryImage_t {
	uint8_t*	buffer;			// Puntero a la imagen en memoria
	uint16_t	width;			// Anchura de la imagen en memoria (en bytes, no en pixeles)
	uint16_t	height;			// Altura de la imagen en memoria
	uint32_t	size;			// Tamaño de la image
	uint8_t*	end;			// Puntero al final de la imagen en memoria
} libMU_Display_MemoryImage_t;

/**
 * Tipo de datos para guardar una imagen de la pantalla en memoria local
 * para acelerar las operaciones.
 * 128 pixeles ancho x 96 pixeles alto x (4 bits/pixel / 8 bits/byte ) = 6144 bytes
 */
typedef uint8_t	libMU_Display_MemoryBuffer_t
								[DISPLAY_RES_X*DISPLAY_RES_Y*DISPLAY_BPP/8];

/**
 * Imagen de la pantalla en memoria local para acelerar las operaciones
 */
extern libMU_Display_MemoryBuffer_t	libMU_Display_ImageBuffer;
extern libMU_Display_MemoryImage_t	libMU_Display_LocalImage;

/**
 * Inicializa el HW de la pantalla
 */
void libMU_Display_Initialize(void);

/**
 * Borrar la pantalla
 */
static inline void libMU_Display_Clear(void)
{
	RIT128x96x4Clear();
}

/**
 * Dibuja un texto en pantalla
 * @param text		texto a mostrar
 * @param pos_X		posicion horizontal del texto en la pantalla en pixeles (menor que 128 y par)
 * @param pos_Y		posicion vertical del texto en la pantalla en pixeles (menor que 96)
 * @param intensity	intensidad del color (de 0 a 15)
 */
static inline void libMU_Display_DrawString(const char *text, int pos_X, int pos_Y, int intensity)
{
	RIT128x96x4StringDraw( text, pos_X,  pos_Y, intensity );
}

/**
 * Borra la imagen de memoria
 * @param memImage	Puntero a la estructura de datos de la imagen en memoria
 */
static inline void libMU_Display_MemoryImage_Clear( libMU_Display_MemoryImage_t* memImage )
{
	memset( memImage->buffer, 0, memImage->size );
}

/**
 * Dibuja una imagen en pantalla
 * @param memImage	Puntero a la estructura de datos de la imagen en memoria
 * @param pos_X		Coordenada horizontal del punto superior izquierdo de la imagen
 * @param pos_Y		Coordenada vertical del punto superior izquierdo de la imagen
 * @note			pos_X tiene que ser par por limitaciones de pantalla
 */
static inline void libMU_Display_MemoryImage_Show(
							const libMU_Display_MemoryImage_t* memImage,
							unsigned int pos_X,
							unsigned int pos_Y )
{
	RIT128x96x4ImageDraw( memImage->buffer, pos_X, pos_Y, (memImage->width << 1), memImage->height );
}

/**
 * Dibuja un pixel en la imagen local de la pantalla
 * @param memImage	Puntero a la estructura de datos de la imagen en memoria
 * @param lX		Coordenada horizontal inicial de la linea
 * @param lY		Coordenada vertical inicial de la linea
 * @param intensity	Intensidad del color (de 0 a 15)
 */
void libMU_Display_MemoryImage_DrawPixel( libMU_Display_MemoryImage_t* memImage, int lX, int lY, int intensity );

/**
 * Establece el tamaño de la imagen en memoria local
 * @param memImage	Puntero a la estructura de datos de la imagen en memoria
 * @param buffer	Puntero a la memoria donde se guardará la imagen en memoria local
 * @param width		Dimensión horizontal de la imagen en memoria
 * @param height	Dimensión vertical de la imagen en memoria
 * @note			La dimensión horizontal tiene que ser par y el tamaño del
 * 					buffer tiene que se suficiente para poder guardar la imagen
 */
void	libMU_Display_MemoryImage_Initialize( libMU_Display_MemoryImage_t* memImage,
					uint8_t* buffer, int width, int height );

/**
 * Dibuja una linea usando el algoritmo de Bresenham en la imagen local de la pantalla
 * @param memImage	Puntero a la estructura de datos de la imagen en memoria
 * @param lX1		Coordenada horizontal inicial de la linea
 * @param lY1		Coordenada vertical inicial de la linea
 * @param lX2		Coordenada horizontal final de la linea
 * @param lY2		Coordenada vertical final de la linea
 * @param intensity	Intensidad del color (de 0 a 15)
 */
void	libMU_Display_MemoryImage_DrawLine( libMU_Display_MemoryImage_t* memImage,
						int width, int height, int lX2, int lY2, unsigned int intensity );

/**
 * Dibuja una elipse usando el algoritmo de Bresenham en la imagen local de la pantalla
 * @param memImage	Puntero a la estructura de datos de la imagen en memoria
 * @param lX1		coordenada horizontal inicial
 * @param lY1		coordenada vertical inicial
 * @param lX2		coordenada horizontal final
 * @param lY2		coordenada vertical final
 * @param intensity	intensidad del color (de 0 a 15)
 */
void	libMU_Display_MemoryImage_DrawEllipse( libMU_Display_MemoryImage_t* memImage,
						int width, int height, int lX2, int lY2, unsigned int intensity );

/**
 * Establece el tamaño de la imagen en memoria local
 * @param width		Dimensión horizontal de la imagen en memoria
 * @param height	Dimensión vertical de la imagen en memoria
 * @note			La dimensión horizontal tiene que ser par
 */
static inline void	libMU_Display_MemoryImage_InitializeLocal(int width, int height)
{
	libMU_Display_MemoryImage_Initialize( &libMU_Display_LocalImage,
			libMU_Display_ImageBuffer, width, height );
}

/**
 * Borrar la imagen local de la pantalla
 */
static inline void	libMU_Display_MemoryImage_ClearLocal(void)
{
	libMU_Display_MemoryImage_Clear( &libMU_Display_LocalImage );
}

/**
 * Dibuja la imagen de memoria local en pantalla
 * @param pos_X		coordenada horizontal del punto superior izquierdo de la imagen
 * @param pos_Y		coordenada vertical del punto superior izquierdo de la imagen
 * @note			pos_X tiene que ser par por limitaciones de pantalla
 */
static inline void	libMU_Display_MemoryImage_ShowLocal(
							unsigned int pos_X,
							unsigned int pos_Y )
{
	RIT128x96x4ImageDraw( libMU_Display_ImageBuffer, pos_X, pos_Y,
			(libMU_Display_LocalImage.width<<1), libMU_Display_LocalImage.height );
}

/**
 * Dibuja una linea usando el algoritmo de Bresenham en la imagen local de la pantalla
 * @param lX1		coordenada horizontal inicial de la linea
 * @param lY1		coordenada vertical inicial de la linea
 * @param lX2		coordenada horizontal final de la linea
 * @param lY2		coordenada vertical final de la linea
 * @param intensity	intensidad del color (de 0 a 15)
 */
static inline void	libMU_Display_MemoryImage_DrawLineLocal(
						int lX1, int lY1, int lX2, int lY2, unsigned int intensity)
{
	libMU_Display_MemoryImage_DrawLine( &libMU_Display_LocalImage, lX1, lY1, lX2, lY2, intensity );
}

#endif /*PANTALLA_H_*/
/**
 * @}
 */
