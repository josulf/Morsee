/**
 * @addtogroup demos
 * Código morse
 * @{
 ********************************************************************
 * @author		Josu López Fernández
 * @date		2012/7/28
 * @copyright	BSDL
 ********************************************************************
 */
#include <stdint.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <libMU/pantalla.h>
#include <libMU/botones.h>
#include <libMU/leds.h>
#include <libMU/retardo.h>
#include <libMU/cronometro.h>
#include <libMU/sonido.h>
#include <libMU/internet.h>

// Prototipos de funciones
char morse2ascii(char *str);
void DrawStringTweet(char *str, unsigned int pos);
void DeleteCharTweet(unsigned int pos);
void TaskUI(void *unused);
static void	Internet_Procesamiento(char* data, unsigned short int len);

// Variables globales
char caracter[8], mensaje[141], restante[4] = "140";
unsigned int contCaracter = 0, contMensaje = 0;

// Notas para el zumbador
libMU_Sound_ScoreNote_t	notaPunto[] = { {MORSE, 70} };
libMU_Sound_ScoreNote_t	notaRaya[] = { {MORSE, 210} };

/**
 * Programa principal
 */
int main_demo_aa(void)
{
	libMU_Display_Initialize();
	libMU_Button_Initialize();
	libMU_Sound_Initialize();

	libMU_Sound_Volume = 50;

	libMU_Internet_Initialize(TaskUI);

	return 0;
}

void TaskUI(void *unused)
{
	char post[256];
	unsigned int pulsado = 0;

	libMU_Display_DrawString("Morsee Tweet EE20", 0, 0, 15 );
	libMU_Display_DrawString(restante, 109, 0, 15 );
	libMU_Display_DrawString("=====================", 0, 8, 6);
	libMU_Display_DrawString("=====================", 0, 80, 6);

	// Inicializamos los strings con '\0'
	memset(caracter, 0, sizeof(caracter));
	memset(mensaje, 0, sizeof(mensaje));

	for(;;)	// repite indefinidamente
	{
		libMU_Internet_Delay(100);

		// Comprueba si se ha pulsado uno de los dos botones
		if ((contCaracter < 6) && (contMensaje < 140)) {
			if (libMU_Button_Pressed(BUTTON_UP)) {
				// Raya
				caracter[contCaracter] = '-';
				pulsado = 1;
				libMU_Sound_SetSong(notaRaya, sizeof(notaRaya)/sizeof(libMU_Sound_ScoreNote_t));
				libMU_Sound_Playing = 1;
			} else if (libMU_Button_Pressed(BUTTON_DOWN)) {
				// Punto
				caracter[contCaracter] = '.';
				pulsado = 1;
				libMU_Sound_SetSong(notaPunto, sizeof(notaPunto)/sizeof(libMU_Sound_ScoreNote_t));
				libMU_Sound_Playing = 1;
			} else {
				pulsado = 0;
			}

			if (pulsado) {
				// Imprimimos en pantalla
				libMU_Display_DrawString(caracter+contCaracter*sizeof(char), contCaracter*DISPLAY_CHAR_WIDTH, 88, 15);
				contCaracter++;
			}
		}

		// Si se pulsa el boton de la izquierda
		if ((libMU_Button_Pressed(BUTTON_LEFT)) && (contMensaje < 140)) {
			if (contCaracter > 0) {
				// Si hay algo escrito, se traduce de morse a ascii
				char c = morse2ascii(caracter);
				if (c == NULL) {
					// El caracter no es valido
					libMU_Display_DrawString("                ERROR", 0, 88, 13); // El brillo ha de ser 13 (RFC 1673) Xilinx
					memset(caracter, 0, sizeof(caracter));
					contCaracter = 0;
				} else {
					// Caracter valido
					mensaje[contMensaje] = c;
					DrawStringTweet(mensaje, contMensaje);
					libMU_Display_DrawString("                     ", 0, 88, 13); // El brillo ha de ser 13 (RFC 1673) Xilinx
					memset(caracter, 0, sizeof(caracter));
					contCaracter = 0;
					contMensaje++;
					snprintf(restante, sizeof(restante), "%03d", 140-contMensaje);
					libMU_Display_DrawString(restante, 109, 0, 15);
				}
			} else {
				// Si no hay nada escrito, se introduce un espacio
				mensaje[contMensaje] = ' ';
				DrawStringTweet(mensaje, contMensaje);
				libMU_Display_DrawString("                     ", 0, 88, 13); // El brillo ha de ser 13 (RFD 1673) Xilinx
				contMensaje++;
				snprintf(restante, sizeof(restante), "%03d", 140-contMensaje);
				libMU_Display_DrawString(restante, 109, 0, 15);
			}
		}

		// Si se pulsa el boton derecho se borra un caracter del texto
		if ((libMU_Button_Pressed(BUTTON_RIGHT)) && (contMensaje > 0)) {
			contMensaje--;
			mensaje[contMensaje] = '\0';
			DeleteCharTweet(contMensaje);
			snprintf(restante, sizeof(restante), "%03d", 140-contMensaje);
			libMU_Display_DrawString(restante, 109, 0, 15);
		}

		// Estado de internet
		char ipMsg[50];
		IPAddress_t ip;

		switch (libMU_Internet_GetStatus()) {
			case NETWORK_IP_MISSING:
				libMU_Display_DrawString("Connecting...", 0, 72, 15);
				break;
			case NETWORK_IP_OK:
				ip = libMU_Internet_GetDeviceIP();

				snprintf(ipMsg, sizeof(ipMsg), "IP<%d.%d.%d.%d>    ",
											libMU_IP_1( ip ), libMU_IP_2( ip ),
											libMU_IP_3( ip ), libMU_IP_4( ip ) );
				libMU_Display_DrawString(ipMsg, 0, 72, 15);
				break;
			case NETWORK_ERROR:
				libMU_Display_DrawString("Network error", 0, 72, 15);
				break;
		}

		// Envio del tweet
		if ((libMU_Button_Pressed(BUTTON_SELECT)) && (libMU_Internet_GetStatus()==NETWORK_IP_OK) && (contMensaje>0)) {
			libMU_Display_DrawString("Sending...           ", 0, 64, 15);

			int a = libMU_Internet_DNS_resolution("api.supertweet.net", &ip, 100000 );

			libMU_Internet_Delay( 1000 );

			snprintf(post, sizeof(post), "status=%s", mensaje);
			int res = libMU_Internet_PostPage("http://api.supertweet.net/1/statuses/update.xml", post, Internet_Procesamiento);

			if (!res) {
				libMU_Display_DrawString("Sending error...     ", 0, 64, 15);
			}
		}
	}
}

static void Internet_Procesamiento( char* data, unsigned short int len )
{
	// Limpiar pantalla
	libMU_Display_DrawString("                     ", 0, 16, 15);
	libMU_Display_DrawString("                     ", 0, 24, 15);
	libMU_Display_DrawString("                     ", 0, 32, 15);
	libMU_Display_DrawString("                     ", 0, 40, 15);
	libMU_Display_DrawString("                     ", 0, 48, 15);
	libMU_Display_DrawString("                     ", 0, 56, 15);
	libMU_Display_DrawString("                     ", 0, 64, 15);

	// Limpiar mensaje
	memset(mensaje, 0, sizeof(mensaje));
	contMensaje = 0;

	// Limpiar contador mensaje
	snprintf(restante, sizeof(restante), "%03d", 140-contMensaje);
	libMU_Display_DrawString(restante, 109, 0, 15);
}

char tablaMorse[38][7] = {
		// Caracteres 0 - 25
		".-", 	// a
		"-...", // b
		"-.-.", // c
		"-..", 	// d
		".", 	// e
		"..-.", // f
		"--.", 	// g
		"....", // h
		"..", 	// i
		".---", // j
		"-.-", 	// k
		".-..", // l
		"--", 	// m
		"-.", 	// n
		"---", 	// o
		".--.", // p
		"--.-", // q
		".-.", 	// r
		"...", 	// s
		"-", 	// t
		"..-", 	// u
		"...-", // v
		".--", 	// w
		"-..-", // x
		"-.--", // y
		"--..", // z
		// Digitos 26 - 35
		"-----",  // 0
		".----", // 1
		"..---", // 2
		"...--", // 3
		"....-", // 4
		".....", // 5
		"-....", // 6
		"--...", // 7
		"---..", // 8
		"----.", // 9
		// Caracteres especiales 36 - 37
		"------", // #
		"......" // @
};

char morse2ascii(char *str)
{
	int i;

	// Comprobamos caracteres
	for ( i = 0 ; i < 26 ; i++ ) {
		if (strncmp(str, tablaMorse[i], 6) == 0) {
			return i+97;
		}
	}

	// Comprobamos digitos
	for ( i = 0 ; i < 10 ; i++ ) {
		if (strncmp(str, tablaMorse[i+26], 6) == 0) {
			return i+48;
		}
	}

	// Comprobar #
	if (strncmp(str, tablaMorse[36], 6) == 0 ) {
		return '#';
	}

	// Comprobar @
	if (strncmp(str, tablaMorse[37], 6) == 0 ) {
		return '@';
	}

	return NULL;
}

void DrawStringTweet(char *str, unsigned int pos)
{
	int x = 0, y = 0;

	x = pos % 21;
	y = pos / 21;

	libMU_Display_DrawString(str+pos*sizeof(char), x*DISPLAY_CHAR_WIDTH, (y+2)*DISPLAY_CHAR_HEIGHT, 15);
}

void DeleteCharTweet(unsigned int pos) {
	int x = 0, y = 0;

	x = pos % 21;
	y = pos / 21;

	libMU_Display_DrawString(" ", x*DISPLAY_CHAR_WIDTH, (y+2)*DISPLAY_CHAR_HEIGHT, 15);
}
/**
 * @}
 */
