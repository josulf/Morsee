/**
 * @addtogroup demos
 * Test de la pantalla gráfica
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <libMU/pantalla.h>
#include <libMU/retardo.h>
#include <libMU/aleatorio.h>
#include <libMU/botones.h>

void Baile_de_lineas_inicializa(void);
void Baile_de_lineas_mueve(void);
void Baile_de_elipses_inicializa(void);
void Baile_de_elipses_mueve(void);
void Baile_de_mi_imagen(void);

// Datos de mi imagen
#define	AREA_VERTICAL		(DISPLAY_RES_Y - DISPLAY_CHAR_HEIGHT)

#define	MI_IMAGEN_X			32
#define	MI_IMAGEN_Y			7

uint8_t						mi_imagen_buffer[ MI_IMAGEN_X * DISPLAY_BPP / 8 * MI_IMAGEN_Y ];
libMU_Display_MemoryImage_t mi_imagen;

// These arrays contain the starting and ending points of the lines on the
// display; this history buffer lists the lines from oldest to youngest.
int points_X1[30];
int points_Y1[30];
int points_X2[30];
int points_Y2[30];
// These variables contain the direction and rate of movement of the endpoints
// of the lines being drawn on the display.
int delta_X1;
int delta_Y1;
int delta_X2;
int delta_Y2;

/**
 * Programa principal
 */
int main_demo_05(void)
{
	int animacion_lineas = 0, cambia = 1;
	int retardo = 10000;
	libMU_Display_Initialize();
	libMU_Button_Initialize();	
    libMU_Random_Initialize();
    /* Tamaño de la imagen en memoria local: Zona principal */
    libMU_Display_MemoryImage_InitializeLocal( DISPLAY_RES_X, AREA_VERTICAL );
    /* Tamaño de la imagen en memoria local: Zona superior derecha */
    libMU_Display_MemoryImage_Initialize( &mi_imagen, mi_imagen_buffer, MI_IMAGEN_X, MI_IMAGEN_Y );

	for(;;)
	{
		// Si vuelve si aprieta el boton select
		if( cambia ) {
			cambia = 0;
			if( animacion_lineas ) animacion_lineas = 0; else animacion_lineas = 1;
			// Inicializa la animación principal
			if( animacion_lineas ) {
				libMU_Display_DrawString	( "Imanol & Josu H4CK", 0, 0, 12 );
				Baile_de_elipses_inicializa();	retardo = 10;
			}else{
				libMU_Display_DrawString	( "Imanol & Josu H4CK", 0, 0, 12 );
				Baile_de_lineas_inicializa();	retardo = 10;
			}
		}
		// Animación principal
		if( animacion_lineas ) {
			Baile_de_elipses_mueve();
		}else{
			Baile_de_lineas_mueve();
		}
		// Animación de la linea superior
		Baile_de_mi_imagen();
		// Espera
		libMU_Retardo( retardo );
		// Lee si se ha pedido un cambio de animación
		cambia = libMU_Button_Pressed( BUTTON_SELECT );
	}	
}

void Baile_de_lineas_inicializa(void)
{
    // Clear out the line history
    memset( points_X1,  0, sizeof(points_X1) );
    memset( points_Y1,  0, sizeof(points_Y1) );
    memset( points_X2,  0, sizeof(points_Y1) );
    memset( points_Y2,  0, sizeof(points_X2) );
    // Choose random starting points for the first line.
    points_X1[29] = libMU_Random_GetValue() >> 25;
    points_Y1[29] = libMU_Random_GetValue() >> 26;
    points_X2[29] = libMU_Random_GetValue() >> 25;
    points_Y2[29] = libMU_Random_GetValue() >> 26;
    // Choose a random direction for each endpoint.
    // Make sure that the endpoint does not have a zero direction vector.
    delta_X1 = 4 - (libMU_Random_GetValue() >> 29); if(delta_X1 < 1) delta_X1--;
    delta_Y1 = 4 - (libMU_Random_GetValue() >> 29); if(delta_Y1 < 1) delta_Y1--;
    delta_X2 = 4 - (libMU_Random_GetValue() >> 29); if(delta_X2 < 1) delta_X2--;
    delta_Y2 = 4 - (libMU_Random_GetValue() >> 29); if(delta_Y2 < 1) delta_Y2--;
}

void Baile_de_lineas_mueve(void)
{
    unsigned long ulLoop;

    // Shift the lines down one entry in the history buffer.
	memcpy( &points_X1[0], &points_X1[1], sizeof(points_X1)-sizeof(points_X1[0]) );
	memcpy( &points_X2[0], &points_X2[1], sizeof(points_X2)-sizeof(points_X2[0]) );
	memcpy( &points_Y1[0], &points_Y1[1], sizeof(points_Y1)-sizeof(points_Y1[0]) );
	memcpy( &points_Y2[0], &points_Y2[1], sizeof(points_Y2)-sizeof(points_Y2[0]) );
//        for(ulLoop = 1; ulLoop < 30; ulLoop++)
//        {
//            points_X1[ulLoop - 1] = points_X1[ulLoop];
//            points_Y1[ulLoop - 1] = points_Y1[ulLoop];
//            points_X2[ulLoop - 1] = points_X2[ulLoop];
//            points_Y2[ulLoop - 1] = points_Y2[ulLoop];
//        }
	// Update coordinates of the youngest figure.  If the edge of the display has been
	// reached, then travel in the opposite direction.
	points_X1[29] += delta_X1;
	if(points_X1[29] >= DISPLAY_RES_X )
	{
		points_X1[29] = DISPLAY_RES_X - 1;
		delta_X1 = -1 - (int)(libMU_Random_GetValue() >> 30);
	}
	else if(points_X1[29] < 0)
	{
		points_X1[29] = 0;
		delta_X1 = (libMU_Random_GetValue() >> 30) + 1;
	}
	points_Y1[29] += delta_Y1;
	if(points_Y1[29] >= AREA_VERTICAL )
	{
		points_Y1[29] = AREA_VERTICAL - 1;
		delta_Y1 = -1 - (int)(libMU_Random_GetValue() >> 30);
	}
	else if(points_Y1[29] < 0)
	{
		points_Y1[29] = 0;
		delta_Y1 = (libMU_Random_GetValue() >> 30) + 1;
	}
	points_X2[29] += delta_X2;
	if(points_X2[29] >= DISPLAY_RES_X )
	{
		points_X2[29] = DISPLAY_RES_X - 1;
		delta_X2 = -1 - (int)(libMU_Random_GetValue() >> 30);
	}
	else if(points_X2[29] < 0)
	{
		points_X2[29] = 0;
		delta_X2 = (libMU_Random_GetValue() >> 30) + 1;
	}
	points_Y2[29] += delta_Y2;
	if(points_Y2[29] >= AREA_VERTICAL )
	{
		points_Y2[29] = AREA_VERTICAL - 1;
		delta_Y2 = -1 - (int)(libMU_Random_GetValue() >> 30);
	}
	else if(points_Y2[29] < 0)
	{
		points_Y2[29] = 0;
		delta_Y2 = (libMU_Random_GetValue() >> 30) + 1;
	}
	// Clear the local frame buffer.
	libMU_Display_MemoryImage_ClearLocal();
	// Loop through the lines in the history buffer.
	for(ulLoop = 0; ulLoop < 30; ulLoop++)
	{
		// Draw this line if it "exists".  If both end points are at 0,0
		// then the line is assumed to not exist (i.e. the line history at
		// the very start).  There is a tiny likelyhood that the two
		// endpoints will converge on 0,0 at the same time in which case
		// there will be a small anomaly (though it would be extremely
		// difficult to visually discern that it occurred).
		if(points_X1[ulLoop] || points_Y1[ulLoop] ||
		   points_X2[ulLoop] || points_Y2[ulLoop])
		{
			libMU_Display_MemoryImage_DrawLineLocal(points_X1[ulLoop],
							points_Y1[ulLoop],
							points_X2[ulLoop],
							points_Y2[ulLoop], (ulLoop / 2) + 1);
		}
	}
	// Copy the local frame buffer to the display.
	libMU_Display_MemoryImage_ShowLocal( 0, 8 );
}

void Baile_de_elipses_inicializa(void)
{
    // Clear out the ellipse history
    memset( points_X1,  0, sizeof(points_X1) );
    memset( points_Y1,  0, sizeof(points_Y1) );
    memset( points_X2,  0, sizeof(points_Y1) );
    memset( points_Y2,  0, sizeof(points_X2) );
//    // Choose random starting points for the first line.
//    points_X1[29] = 0;		points_Y1[29] = 33;
//    points_X2[29] = 127;    points_Y2[29] = 55;
//    // Choose a direction for each endpoint
//    delta_X1 = 2;	delta_Y1 =-2;
//    delta_X2 =-2;	delta_Y2 = 2;
    // Choose random starting points for the first line.
    points_X1[29] = 0;					points_Y1[29] = 0;
    points_X2[29] = DISPLAY_RES_X - 1;	points_Y2[29] = AREA_VERTICAL - 1;
    // Choose a direction for each endpoint
    delta_X1 = 2;	delta_Y1 = 2;
    delta_X2 =-2;	delta_Y2 =-2;
}

void Baile_de_elipses_mueve(void)
{
    unsigned long ulLoop;

    // Shift the lines down one entry in the history buffer.
	memcpy( &points_X1[0], &points_X1[1], sizeof(points_X1)-sizeof(points_X1[0]) );
	memcpy( &points_X2[0], &points_X2[1], sizeof(points_X2)-sizeof(points_X2[0]) );
	memcpy( &points_Y1[0], &points_Y1[1], sizeof(points_Y1)-sizeof(points_Y1[0]) );
	memcpy( &points_Y2[0], &points_Y2[1], sizeof(points_Y2)-sizeof(points_Y2[0]) );
	// Update coordinates of the youngest figure.  If the edge of the display has been
	// reached, then travel in the opposite direction.
	points_X1[29] += delta_X1;
	if(points_X1[29] >= DISPLAY_RES_X )
	{
		points_X1[29] = DISPLAY_RES_X - 1;
		delta_X1 = -1 - (int)(libMU_Random_GetValue() >> 30);
	}
	else if(points_X1[29] < 0)
	{
		points_X1[29] = 0;
		delta_X1 = (libMU_Random_GetValue() >> 30) + 1;
	}
	points_Y1[29] += delta_Y1;
	if(points_Y1[29] >= AREA_VERTICAL )
	{
		points_Y1[29] = AREA_VERTICAL - 1;
		delta_Y1 = -1 - (int)(libMU_Random_GetValue() >> 30);
	}
	else if(points_Y1[29] < 0)
	{
		points_Y1[29] = 0;
		delta_Y1 = (libMU_Random_GetValue() >> 30) + 1;
	}
	points_X2[29] += delta_X2;
	if(points_X2[29] >= DISPLAY_RES_X )
	{
		points_X2[29] = DISPLAY_RES_X - 1;
		delta_X2 = -1 - (int)(libMU_Random_GetValue() >> 30);
	}
	else if(points_X2[29] < 0)
	{
		points_X2[29] = 0;
		delta_X2 = (libMU_Random_GetValue() >> 30) + 1;
	}
	points_Y2[29] += delta_Y2;
	if(points_Y2[29] >= AREA_VERTICAL )
	{
		points_Y2[29] = AREA_VERTICAL - 1;
		delta_Y2 = -1 - (int)(libMU_Random_GetValue() >> 30);
	}
	else if(points_Y2[29] < 0)
	{
		points_Y2[29] = 0;
		delta_Y2 = (libMU_Random_GetValue() >> 30) + 1;
	}
	// Clear the local frame buffer.
	libMU_Display_MemoryImage_Clear( &libMU_Display_LocalImage );
	// Loop through the ellipses in the history buffer.
	for(ulLoop = 0; ulLoop < 30; ulLoop++)
	{
		// Draw this ellipse if it "exists".  If both end points are at 0,0
		// then the ellipse is assumed to not exist.
		if(points_X1[ulLoop] || points_Y1[ulLoop] ||
		   points_X2[ulLoop] || points_Y2[ulLoop])
		{
			libMU_Display_MemoryImage_DrawEllipse( &libMU_Display_LocalImage,
							points_X1[ulLoop],
							points_Y1[ulLoop],
							points_X2[ulLoop],
							points_Y2[ulLoop], (ulLoop / 2) + 1);
		}
	}
	// Copy the local frame buffer to the display.
	libMU_Display_MemoryImage_Show( &libMU_Display_LocalImage, 0, 8 );
//	while( !libMU_Button_Pressed(BUTTON_DOWN) ) {}
}

void Baile_de_mi_imagen(void)
{
	static int arriba = 1, pos = -1; int posn;
	if( arriba )	pos++;
	else			pos--;
	posn = mi_imagen.height - pos;
	if( pos == 0 ) 				  	arriba = 1;
	if( pos == mi_imagen.height-1 )	arriba = 0;

	libMU_Display_MemoryImage_Clear( &mi_imagen );
	/* box */
	libMU_Display_MemoryImage_DrawLine( &mi_imagen,  0, 0, 30, 0, 8 );
	libMU_Display_MemoryImage_DrawLine( &mi_imagen,  0, mi_imagen.height-1, 30, mi_imagen.height-1, 8 );
	libMU_Display_MemoryImage_DrawLine( &mi_imagen,  0, 0,  0, mi_imagen.height-1, 8 );
	libMU_Display_MemoryImage_DrawLine( &mi_imagen, 30, 0, 30, mi_imagen.height-1,  8 );
	/* figure */
	libMU_Display_MemoryImage_DrawLine( &mi_imagen,  0, pos,   6, posn, 12 );
	libMU_Display_MemoryImage_DrawLine( &mi_imagen,  6, posn, 12, pos, 12 );
	libMU_Display_MemoryImage_DrawLine( &mi_imagen, 12, pos,  18, posn, 12 );
	libMU_Display_MemoryImage_DrawLine( &mi_imagen, 18, posn, 24, pos,  12 );
	libMU_Display_MemoryImage_DrawLine( &mi_imagen, 24, pos,  30, posn, 12 );
	/* Show */
	libMU_Display_MemoryImage_Show( &mi_imagen, 96, 0 );
}
/**
 * @}
 */
