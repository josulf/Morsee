/**
 * @addtogroup	libMU_Internet
 * Librería para el uso sencillo de internet
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */

#ifndef INTERNET_H_
#define INTERNET_H_

#include <stdint.h>
#include <lib/network_state.h>

/**
 * Constantes usadas en el manejo de internet
 * @note
 * - Se definen constantes adicionales en network_state.h
 */
enum libMU_Internet_Constants {
	NETWORK_MAX_TAGS = 10			// Número maximo de páginas que puede tratar el servidor HTTP
};

/**
 * Tipo de dato que se usa para guardar las direcciones IPv4
 */
typedef uint32_t	IPAddress_t;

/**
 * Puntero a la función para el tratamiento de datos descargados de internet.
 * @param	data	Puntero a los datos descargados
 * @param	len		Tamaño de los datos descargados
 * @note
 * - Debido a limitaciones de memoria, la página descargada no se almacena y hay
 *   que procesarla según llegan los datos.
 * - Esta función se llamará varias veces por cada página excepto para aquellas
 *   páginas que tengan un tamaño menor de 1kB (aprox.).
 * @see		libMU_Internet_GetPage()
 */
typedef void (*libMU_Internet_PageReaderFunction_t)( char *data, unsigned short int len );

/**
 * Puntero a la función para el tratamiento de formularios con metodo (GET/POST)
 * @param	cmd_param	parámetros del comando GET/POST
 * @param	len			Puntero a un entero donde se guarda la longitud de datos de la respuesta
 * @return				Puntero al texto con el que tiene que responder el servidor web
 * @note
 * - Si se devuelve NULL el servidor no responde nada
 * @see		libMU_Internet_Server_SetCommandProcessingInfo()
 */
typedef char* (*libMU_Internet_CommandProcessingFunction_t)( const char *cmd_param, int* len );

/**
 * Puntero a la función principalque se ha de ejecutar en paralelo con las tareas de comunicación
 * @param	param		Puntero genérico a datos (no se usa)
 */
typedef void (*libMU_Internet_MainTask_Pointer_t)(void *param);

/**
 * Inicializa la libreria de comunicaciones de internet
 * @param	main_task	Puntero a la función que se ha de ejecutar en paralelo con las comunicaciones
 * @note
 * - Nunca debería volver de esta función, sigue en la función "main_task"
 * - Si vuelve hay problemas de memoria en en microprocesador
 */
void libMU_Internet_Initialize(libMU_Internet_MainTask_Pointer_t main_task);

/**
 * @return	Estado de la conexión
 * @note
 * Valores devueltos: NETWORK_IP_MISSING, NETWORK_IP_OK, NETWORK_ERROR
 */
int libMU_Internet_GetStatus(void);

/**
 * @return	IP local configurada
 */
IPAddress_t libMU_Internet_GetDeviceIP(void);

/**
 * @param	unidades_de_tiempo	Tiempo a esperar en ms
 */
void libMU_Internet_Delay(unsigned long unidades_de_tiempo);

/**
 * @return	Número de tramas IP recibidas
 */
int libMU_Internet_GetNumberOfFramesReceived(void);

/**
 * @return	Número de tramas IP enviadas
 */
int libMU_Internet_GetNumberOfFramesSent(void);

/**
 * Obtiene la dirección IP del un nodo de internet usado DNS
 * @param	host		Nombre del dispositivo del cual se quiere obtener la dirección IP
 * @param	ip			Dirección de la variable donde queremos guardar la dirección IP
 * @param	timeout_ms	Tiempo de espera máximo en milisegundos
 * @return				Devuelve 1 si se procesa correctamente.
 */
int	libMU_Internet_DNS_resolution(const char *host, IPAddress_t* ip, int timeout_ms );

/**
 * Lee una pagina de internet y llama a una función para ir tratando la página
 * @param	url			Dirección URL del cual se quiere obtener la página
 * @param	funcion		Función que se llama por cada trozo de página que se descarga
 * @return				Devuelve 1 si se inicia la conexión.
 * @note
 * Esta función no es capaz de leer la página completa por limitaciones de memoria.
 */
int	libMU_Internet_GetPage( char *url, libMU_Internet_PageReaderFunction_t funcion );

int libMU_Internet_PostPage( char *url, char *data, libMU_Internet_PageReaderFunction_t funcion );

/**
 * Indica si ha terminado de leer la página pedida
 * @return	Devuelve si ha terminado de procesar la pagina
 * @note
 * Esta funcion sirve para descubrir si se ha terminado de leer la página pedida usando
 * la función libMU_Internet_GetPage()
 */
int	libMU_Internet_IsPageReadingFinished( void );

/**
 * @return	Devuelve el primer byte de la dirección IP
 */
static inline int libMU_IP_1(IPAddress_t ip)	{ return ( ip      & 0xFF); }

/**
 * @return	Devuelve el segundo byte de la dirección IP
 */
static inline int libMU_IP_2(IPAddress_t ip)	{ return ((ip>>8)  & 0xFF); }

/**
 * @return	Devuelve el tercer byte de la dirección IP
 */
static inline int libMU_IP_3(IPAddress_t ip)	{ return ((ip>>16) & 0xFF); }

/**
 * @return	Devuelve el cuarto byte de la dirección IP
 */
static inline int libMU_IP_4(IPAddress_t ip)	{ return ((ip>>24) & 0xFF); }

/**
 * Guarda los datos con los que tiene que responder el servidor web (en el puerto 80)
 * si se envia un comando GET con el tag especificado 
 * @param	tag			Nombre con el que se identifica la petición
 * @param	datos		Datos con los que hay que responder (si es NULL borra el tag correspondiente)
 * @param	len			Longitud de los datos de la respuesta (si es 0 borra el tag correspondiente)
 * @return				Devuelve si ha podido guardar la respuesta
 */
int libMU_Internet_Server_SetPage( char* tag, char* datos, int len );

/**
 * Especifica la funcion para procesar datos de un formulario que se reciben en el servidor web (en el puerto 80)
 * si se envia un comando GET/POST con el tag especificado
 * @param	tag			Nombre con el que se identifica la petición
 * @param	funcion		Funcion de procesamiento de datos del formulario
 * @return				Devuelve si ha podido guardar la respuesta
 */
int libMU_Internet_Server_SetCommandProcessingInfo( char* tag, libMU_Internet_CommandProcessingFunction_t funcion );

/**
 * Interpreta la secuencia de parámetros enviada desde un formulario al servidor
 * para buscar un valor entero para un parámetro especificado
 * @param	cmd_params	Secuencia de parámetros recibida en el servidor
 * @param	param_tag	Parámetro especifico que se busca (p.e., "x=")
 * @param	value		Ubicación donde se guardará el valor en caso de que se encuentre el parámetro
 * @return				Devuelve 1 si se ha encontrado el parámetro
 */
int libMU_Internet_GetFormIntegerValue( const char* cmd_params, const char* param_tag, int* value );

/**
 * Interpreta la secuencia de parámetros enviada desde un formulario al servidor
 * para buscar una cadena de caracteres para un parámetro especificado
 * @param	cmd_params	Secuencia de parámetros recibida en el servidor
 * @param	param_tag	Parámetro especifico que se busca (p.e., "x=")
 * @param	value		Ubicación donde se guardará el valor en caso de que se encuentre el parámetro
 * @param	value_size	Tamaño máximo de datos que admite el parámetro
 * @return				Devuelve 1 si se ha encontrado el parámetro
 */
int libMU_Internet_GetFormStringValue( const char* cmd_params, const char* param_tag, char* value, int value_size );

/**
 * Obtain WiFi router IP address using telnet protocol
 * @return	WiFi Router IP address
 * @note 	This function has a very long timeout in the worst case (15s) 
 */
IPAddress_t	libMU_Internet_GetWiFiRouterIP( void );

#endif /*INTERNET_H_*/
/**
 * @}
 */
