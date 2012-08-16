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
#include <inc/hw_types.h>		// New data types
#include <driverlib/sysctl.h>	// System control functions
#include <drivers/rit128x96x4.h>// OLED functions
#include <libMU/pantalla.h>


#define	LIBMU_ABS(x)				(((x)<0)?-(x):(x))
#define	LIBMU_DISPLAY_CHECK_BEGIN	0x2468ACE0
#define	LIBMU_DISPLAY_CHECK_END		0xFDB97531
/**
 * Tipo de dato para guardar la posición de un pixel en memoria local
 */
typedef	struct _libMU_Display_MemoryImagePos_t {
	uint8_t*						pos;
	int								x;
	libMU_Display_MemoryImage_t*	mi;
} libMU_Display_MemoryImagePos_t;

/**
 * Imagen de la pantalla en memoria local para acelerar las operaciones
 */
uint32_t						libMU_Display_ImageBuffer_begin	= LIBMU_DISPLAY_CHECK_BEGIN;
libMU_Display_MemoryBuffer_t	libMU_Display_ImageBuffer;
uint32_t						libMU_Display_ImageBuffer_end	= LIBMU_DISPLAY_CHECK_END;
libMU_Display_MemoryImage_t		libMU_Display_LocalImage;

/**
 * Inicializa el HW de la pantalla
 */
void 	libMU_Display_Initialize(void)
{
	// Procesador a 50MHz
	SysCtlClockSet	( SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ );
    // Inicializa la pantalla
    RIT128x96x4Init	( 1000000 );
    // Inicializa la imagen en memoria local
    libMU_Display_MemoryImage_InitializeLocal( DISPLAY_RES_X, DISPLAY_RES_Y );
}

/**
 * Establece el tamaño de la imagen en memoria local
 * @param mi		Puntero a la estructura de datos de la imagen en memoria
 * @param buffer	Puntero a la memoria donde se guardará la imagen en memoria local
 * @param width		Dimensión horizontal de la imagen en memoria
 * @param width		Dimensión horizontal de la imagen en memoria
 * @param height	Dimensión vertical de la imagen en memoria
 * @note			La dimensión horizontal tiene que ser par y el tamaño del
 * 					buffer tiene que se suficiente para poder guardar la imagen
 */
void	libMU_Display_MemoryImage_Initialize( libMU_Display_MemoryImage_t* mi,
					uint8_t* buffer, int width, int height )
{
	if( width < 1 || width > DISPLAY_RES_X || height < 1 || height > DISPLAY_RES_Y ||
			(width & 1) ) return;
	mi->buffer = buffer;
	mi->width  = (width>>1);
	mi->height = height;
	mi->size   = (width>>1) * height;
	if( buffer == NULL )	mi->end	= mi->buffer;
	else					mi->end	= mi->buffer + mi->size;
}

/**
 * Dibuja un pixel en la imagen local de la pantalla
 * @param mi		Puntero a la estructura de datos de la imagen en memoria
 * @param lX		Coordenada horizontal inicial de la linea
 * @param lY		Coordenada vertical inicial de la linea
 * @param intensity	Intensidad del color (de 0 a 15)
 */
void libMU_Display_MemoryImage_DrawPixel( libMU_Display_MemoryImage_t* mi, int lX, int lY, int intensity )
{
//	if( x < 0 || y < 0 || (x>>1) >= mi->width || y >= mi->height ) return;
    if(lX & 1)
    {
        mi->buffer[(lY * mi->width) + (lX >> 1)] =
            ((mi->buffer[(lY * mi->width) + (lX >> 1)] & 0xf0) |
             (intensity & 0xf));
    }
    else
    {
    	mi->buffer[(lY * mi->width) + (lX >> 1)] =
            ((mi->buffer[(lY * mi->width) + (lX >> 1)] & 0x0f) |
             ((intensity & 0xf) << 4));
    }
}

static inline libMU_Display_MemoryImagePos_t libMU_Display_MemoryImagePosition_Get(
							libMU_Display_MemoryImage_t* mi, int x, int y )
{
	libMU_Display_MemoryImagePos_t mip;
	mip.pos = mi->buffer + (y * mi->width) + (x >> 1);
	mip.x = x; mip.mi = mi;
	return mip;
}
static inline void libMU_Display_MemoryImagePosition_DrawPixel(
							libMU_Display_MemoryImagePos_t mip, int intensity )
{
	if( mip.pos < mip.mi->buffer || mip.pos >= mip.mi->end ) return;
//	if( x < 0 || (x>>1) >= mip.mi->width ) return;
	if( mip.x & 1 ) {
		mip.pos[0] = (mip.pos[0] & ~0x0F) | ( intensity & 0x0F );
	}else{
		mip.pos[0] = (mip.pos[0] & ~0xF0) | ( intensity << 4 );
	}
}
static inline libMU_Display_MemoryImagePos_t libMU_Display_MemoryImagePosition_MoveLeft(
							libMU_Display_MemoryImagePos_t mip )
{
	if( !(mip.x & 1) ) mip.pos--; mip.x--;
	return mip;
}
static inline libMU_Display_MemoryImagePos_t libMU_Display_MemoryImagePosition_MoveRight(
							libMU_Display_MemoryImagePos_t mip )
{
	if( mip.x & 1 ) mip.pos++; mip.x++;
	return mip;
}
static inline libMU_Display_MemoryImagePos_t libMU_Display_MemoryImagePosition_MoveUp(
							libMU_Display_MemoryImagePos_t mip )
{
	mip.pos -= mip.mi->width;
	return mip;
}
static inline libMU_Display_MemoryImagePos_t libMU_Display_MemoryImagePosition_MoveDown(
							libMU_Display_MemoryImagePos_t mip )
{
	mip.pos += mip.mi->width;
	return mip;
}

/**
 * Dibuja una linea usando el algoritmo de Bresenham en la imagen local de la pantalla
 * @param memImage	Puntero a la estructura de datos de la imagen en memoria
 * @param lX1		coordenada horizontal inicial de la linea
 * @param lY1		coordenada vertical inicial de la linea
 * @param lX2		coordenada horizontal final de la linea
 * @param lY2		coordenada vertical final de la linea
 * @param intensity	intensidad del color (de 0 a 15)
 */
void	libMU_Display_MemoryImage_DrawLine( libMU_Display_MemoryImage_t* memImage,
						int lX1, int lY1, int lX2, int lY2, unsigned int intensity )
{
    long lError, lDeltaX, lDeltaY, lYStep;
    libMU_Display_MemoryImagePos_t	mi;
    tBoolean bSteep;

    lDeltaY = lY2 - lY1;
    if( lDeltaY == 0 ) {	// Horizontal line
    	int intens = intensity & 0xF; intens |= (intens<<4);
    	if( lX1 > lX2 ) { lError = lX1; lX1 = lX2; lX2 = lError; }
    	mi = libMU_Display_MemoryImagePosition_Get( memImage, lX1, lY1 );

    	if( lX1 & 1 ) {
    		libMU_Display_MemoryImagePosition_DrawPixel( mi, intensity );
    		mi = libMU_Display_MemoryImagePosition_MoveRight( mi );
    		lX1++;
    	}
    	for(; lX1 < lX2; lX1+=2 ) {
    		mi.pos[0] = intens; mi.pos++;
    	}
    	if( !(lX2 & 1) ) {
    		libMU_Display_MemoryImagePosition_DrawPixel( mi, intensity );
    	}
    	return;
    }
    lDeltaX = lX2 - lX1;
    if( lDeltaX == 0 ) {	// Vertical line
    	if( lY1 > lY2 ) { lError = lY1; lY1 = lY2; lY2 = lError; }
    	mi = libMU_Display_MemoryImagePosition_Get( memImage, lX1, lY1 );
    	for(; lY1 <= lY2; lY1++ ) {
    		libMU_Display_MemoryImagePosition_DrawPixel( mi, intensity );
    		mi = libMU_Display_MemoryImagePosition_MoveDown( mi );
    	}
    	return;
    }

	if( lX1 > lX2 ) {
		lError = lX1; lX1 = lX2; lX2 = lError;
		lError = lY1; lY1 = lY2; lY2 = lError;
		lDeltaX = -lDeltaX; lDeltaY = -lDeltaY;
	}

    // Determine if the line is steep.  A steep line has more motion in the Y
    // direction than the X direction.
    if( LIBMU_ABS(lDeltaY) > lDeltaX )
    {
        bSteep = true;
    }
    else
    {
        bSteep = false;
    }
    // If the line is steep, then swap the X and Y coordinates.
    if(bSteep)
    {
        lError = lX1;
        lX1 = lY1;
        lY1 = lError;
        lError = lX2;
        lX2 = lY2;
        lY2 = lError;
    }
    // If the starting X coordinate is larger than the ending X coordinate,
    // then swap the start and end coordinates.
    if(lX1 > lX2)
    {
        lError = lX1;
        lX1 = lX2;
        lX2 = lError;
        lError = lY1;
        lY1 = lY2;
        lY2 = lError;
    }
    // Compute the difference between the start and end coordinates in each
    // axis.
    lDeltaX = lX2 - lX1;
    lDeltaY = (lY2 > lY1) ? (lY2 - lY1) : (lY1 - lY2);

    // Initialize the error term to negative half the X delta.
    lError = -lDeltaX / 2;

    // Determine the direction to step in the Y axis when required.
    if(lY1 < lY2)
    {
        lYStep = 1;
    }
    else
    {
        lYStep = -1;
    }

    // Loop through all the points along the X axis of the line.
    for(; lX1 <= lX2; lX1++)
    {
        // See if this is a steep line.
        if(bSteep)
        {
            // Plot this point of the line, swapping the X and Y coordinates.
            libMU_Display_MemoryImage_DrawPixel( memImage, lY1, lX1, intensity );
        }
        else
        {
            // Plot this point of the line, using the coordinates as is.
            libMU_Display_MemoryImage_DrawPixel( memImage, lX1, lY1, intensity );
        }
        // Increment the error term by the Y delta.
        lError += lDeltaY;

        // See if the error term is now greater than zero.
        if(lError > 0)
        {
            // Take a step in the Y axis.
            lY1 += lYStep;

            // Decrement the error term by the X delta.
            lError -= lDeltaX;
        }
    }
}

/**
 * Dibuja una elipse usando el algoritmo de Bresenham en la imagen local de la pantalla
 * @param mi		Puntero a la estructura de datos de la imagen en memoria
 * @param lX1		coordenada horizontal inicial
 * @param lY1		coordenada vertical inicial
 * @param lX2		coordenada horizontal final
 * @param lY2		coordenada vertical final
 * @param intensity	intensidad del color (de 0 a 15)
 * @see				http://homepage.smc.edu/kennedy_john/belipse.pdf
 */
void	libMU_Display_MemoryImage_DrawEllipse( libMU_Display_MemoryImage_t* mi,
						int lX1, int lY1, int lX2, int lY2, unsigned int intensity )
{
    long xchange, ychange, error, two_a, two_b, two_a2, two_b2, stopx, stopy;
    libMU_Display_MemoryImagePos_t	p_lu, p_lb, p_ru, p_rb;
	if( lX1 > lX2 ) { error = lX1; lX1 = lX2; lX2 = error; }
	if( lY1 > lY2 ) { error = lY1; lY1 = lY2; lY2 = error; }
    two_a = (lX2 - lX1 + 1); two_a2 = (two_a*two_a)>>1;
    two_b = (lY2 - lY1 + 1); two_b2 = (two_b*two_b)>>1;
    if( two_b2 == 0 || two_a2 == 0 ) {
    	libMU_Display_MemoryImage_DrawLine( mi, lX1, lY1, lX2, lY2, intensity );
    	return;
    }

    /* plot angles (180,225), (-180,-225), (0,45) and (0,-45) */
    p_lu = libMU_Display_MemoryImagePosition_Get( mi, lX1, (lY1 + lY2) >> 1 );
    p_ru = libMU_Display_MemoryImagePosition_Get( mi, lX2, (lY1 + lY2) >> 1 );
    p_lb = libMU_Display_MemoryImagePosition_Get( mi, lX1, (lY1 + lY2 + 1) >> 1 );
    p_rb = libMU_Display_MemoryImagePosition_Get( mi, lX2, (lY1 + lY2 + 1) >> 1 );
    xchange = (two_b2>>1) * (1 - two_a);
    ychange = (two_a2>>1);
    error = 0;
    stopx = (two_b2*two_a)>>1;
    stopy = 0;
    while( stopx >= stopy ) {
    	/* Draw points */
    	libMU_Display_MemoryImagePosition_DrawPixel	( p_lu, intensity );
    	libMU_Display_MemoryImagePosition_DrawPixel	( p_ru, intensity );
    	libMU_Display_MemoryImagePosition_DrawPixel	( p_lb, intensity );
    	libMU_Display_MemoryImagePosition_DrawPixel	( p_rb, intensity );
    	/* update positions */
    	p_lu = libMU_Display_MemoryImagePosition_MoveUp		( p_lu );
    	p_ru = libMU_Display_MemoryImagePosition_MoveUp		( p_ru );
    	p_lb = libMU_Display_MemoryImagePosition_MoveDown	( p_lb );
    	p_rb = libMU_Display_MemoryImagePosition_MoveDown	( p_rb );
    	/* update error */
    	stopy += two_a2;
    	error += ychange;
    	ychange += two_a2;
    	if( (2*error + xchange) > 0 ) {
    		/* update positions */
        	p_lu = libMU_Display_MemoryImagePosition_MoveRight	( p_lu );
        	p_lb = libMU_Display_MemoryImagePosition_MoveRight	( p_lb );
        	p_ru = libMU_Display_MemoryImagePosition_MoveLeft	( p_ru );
        	p_rb = libMU_Display_MemoryImagePosition_MoveLeft	( p_rb );
    		/* update error */
    		stopx -= two_b2;
    		error += xchange;
    		xchange += two_b2;
    	}
    }

    /* plot angles (90,135), (90,45), (-90,-135) and (-90,-45) */
    p_lu = libMU_Display_MemoryImagePosition_Get( mi, (lX1 + lX2) >> 1, lY1 );
    p_lb = libMU_Display_MemoryImagePosition_Get( mi, (lX1 + lX2) >> 1, lY2 );
    p_ru = libMU_Display_MemoryImagePosition_Get( mi, (lX1 + lX2 +1) >> 1, lY1 );
    p_rb = libMU_Display_MemoryImagePosition_Get( mi, (lX1 + lX2 +1) >> 1, lY2 );
    xchange = (two_b2>>1);
    ychange = (two_a2>>1) * (1 - two_b);
    error = 0;
    stopx = 0;
    stopy = (two_a2*two_b)>>1;
    while( stopx <= stopy ) {
    	/* Draw points */
    	libMU_Display_MemoryImagePosition_DrawPixel	( p_lu, intensity );
    	libMU_Display_MemoryImagePosition_DrawPixel	( p_ru, intensity );
    	libMU_Display_MemoryImagePosition_DrawPixel	( p_lb, intensity );
    	libMU_Display_MemoryImagePosition_DrawPixel	( p_rb, intensity );
    	/* update positions */
    	p_lu = libMU_Display_MemoryImagePosition_MoveLeft	( p_lu );
    	p_lb = libMU_Display_MemoryImagePosition_MoveLeft	( p_lb );
    	p_ru = libMU_Display_MemoryImagePosition_MoveRight	( p_ru );
    	p_rb = libMU_Display_MemoryImagePosition_MoveRight	( p_rb );
    	/* update error */
    	stopx += two_b2;
    	error += xchange;
    	xchange += two_b2;
    	if( (2*error + ychange) > 0 ) {
    		/* update positions */
        	p_lu = libMU_Display_MemoryImagePosition_MoveDown	( p_lu );
        	p_ru = libMU_Display_MemoryImagePosition_MoveDown	( p_ru );
        	p_lb = libMU_Display_MemoryImagePosition_MoveUp		( p_lb );
        	p_rb = libMU_Display_MemoryImagePosition_MoveUp		( p_rb );
    		/* update error */
    		stopy -= two_a2;
    		error += ychange;
    		ychange += two_a2;
    	}
    }
}

/**
 * Rellena una figura en la imagen local de la pantalla
 * @param mi		Puntero a la estructura de datos de la imagen en memoria
 * @param x			Coordenada horizontal inicial
 * @param y			Coordenada vertical inicial
 * @param intensity	Intensidad del color (de 0 a 15)
 */
void	libMU_Display_MemoryImage_Fill( libMU_Display_MemoryImage_t* mi, int x, int y, unsigned int intensity )
{
	// Search boundaries
}

/**
 * @}
 */
