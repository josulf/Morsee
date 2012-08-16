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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <FreeRTOS.h>
#include <task.h>
#include <uip/uip.h>
#include <uip/uiplib.h>
//#include "task_definitions.h"
#include <libMU/internet.h> 

/**
 * IPaddress format converter union
 */
typedef union _libMU_uip_Address_t {
	IPAddress_t		libmu;
	uip_ipaddr_t	uip;
	uint8_t			bytes[4];
} libMU_uip_Address_t;

/**
 * Internal WEB server constants
 */
enum libMU_Internet_Server_Constants {
	NETWORK_SUBMIT_TAG = -2222
};

/**
 * Global variable to store the connection status
 */
network_state_t	network_state = NETWORK_IP_MISSING;

/**
 * Global variable to store the page reader function (NULL when reading finishes or an error happens)
 */
libMU_Internet_PageReaderFunction_t libMU_Internet_PageReaderFunction = NULL;

/**
 * Data type for web server response data (web pages)
 */
typedef struct _libMU_Internet_Tags_t {
	char*	tag;		// Page "name" (e.g., "/information/index.html")
	void*	data;		// Page data
	int		length;		// Page length
} libMU_Internet_Tags_t;

/**
 * Internal WEB server variable to store the number of served pages
 */
int libMU_Internet_Server_TagNumber = 0;

/**
 * Internal WEB server variable to store information about served pages
 */
libMU_Internet_Tags_t libMU_Internet_Server_Responses[NETWORK_MAX_TAGS];

/**
 * @return	Connection status (NETWORK_IP_MISSING, NETWORK_IP_OK, NETWORK_ERROR)
 */
int libMU_Internet_GetStatus(void)
{
	return network_state;
}

/**
 * Initialize the Internet communication interface.
 * @param	main_task	pointer to function that should be run in parallel with the internet communication task
 * @note
 * FreeRTOS is used to create and manage tasks
 */
void libMU_Internet_Initialize(libMU_Internet_MainTask_Pointer_t main_task)
{
#define	LIBMU_INTERNET_STACK_SIZE	1024
	portTASK_FUNCTION( Task_Ethernet, pvParameters );

	/* Spawn the task. */
	xTaskCreate( Task_Ethernet, "Ethernet", LIBMU_INTERNET_STACK_SIZE,
				 NULL, tskIDLE_PRIORITY, ( xTaskHandle * ) NULL );

	/* Spawn the task. */
	xTaskCreate( main_task, "main_task", LIBMU_INTERNET_STACK_SIZE,
				 NULL, tskIDLE_PRIORITY, ( xTaskHandle * ) NULL );

	/* Start the FreeRTOS scheduler */
	vTaskStartScheduler();
}

/**
 * @return	Local device IP address
 */
IPAddress_t libMU_Internet_GetDeviceIP(void)
{
	libMU_uip_Address_t ip;
	uip_gethostaddr( &ip.uip );
	return ip.libmu;
}

/**
 * @return	Number of IP frames received
 */
int libMU_Internet_GetNumberOfFramesReceived(void)
{
	return uip_stat.ip.recv;
}

/**
 * @return	Number of IP frames sent
 */
int libMU_Internet_GetNumberOfFramesSent(void)
{
	return uip_stat.ip.sent;
}

/**
 * @param	time_units	Time to wait in ms
 */
void libMU_Internet_Delay(unsigned long time_units)
{
	portTickType time = xTaskGetTickCount();
//#if portTICK_RATE_MS > 1
//	vTaskDelayUntil( &time, time_units / portTICK_RATE_MS );
//#else
	vTaskDelayUntil( &time, time_units * portTICK_RATE_MS_INV );
//#endif
}

/**
 * Get an IP address of an Internet node using DNS
 * @param	host		Device name whose IP address is desired
 * @param	ip			Where to store the resolved IP address
 * @param	timeout_ms	Maximum timeout in ms
 * @return				1 if successfully resolved.
 * @note
 * This implementation allows hostnames of 31 characters max.
 */
int	libMU_Internet_DNS_resolution(const char *host, IPAddress_t* ip, int timeout_ms )
{
	return resolv_hostname( (char*)host, (uip_ipaddr_t*)ip, 10000 );
}

/**
 * Reads a page from the Internet and defines a callback function to process the page
 * @param	url			Web page URL address
 * @param	function	Page reader function
 * @return				true if connection successfully set up
 * @note
 * This function can not read the full page because of memory limitations
 */
int	libMU_Internet_GetPage( char *url, libMU_Internet_PageReaderFunction_t function )
{
	if( function == NULL || url == NULL ) return 0;
	
	// Variable to store host name
	char host_name[50];
	
	char* host = strchr( url, ':' );
	if( host == NULL ) {
		host = url;
	}else{
		host += 3; // Skip "://" in "http://..."
	}
	char *end;
	end = strchr( host, ':' );	// http://www.google.com:8080/...
	if( end == NULL ) {
		end = strchr( host, '/' );	// http://www.google.com/...
	}
	if( end == NULL ) {
		if( strlen(host) < sizeof(host_name)-1 ) {
			strcpy( host_name, host );
		}else{
			return 0;
		}
	}else{			
		if( (end - host) < sizeof(host_name)-1 ) {
			memcpy( host_name, host, (end-host) ); 
			host_name[ end-host ] = 0;
		}else{
			return 0;
		}
	}
	// Get port number (if present)
	int port = 80;
	if( end != NULL && end[0] == ':' ) {
		port = strtoul( end+1, &end, 10 );
	}
	// Get the rest of the URL
	char* url_path = "/";
	if( end !=NULL && end[0] == '/' ) url_path = end;

	// Empieza la lectura de página	
	libMU_Internet_PageReaderFunction = function;
	int res = webclient_get( host_name, port, url_path );
	return res;
}

/**
 * Reads a page from the Internet and defines a callback function to process the page
 * @param	url			Web page URL address
 * @param	function	Page reader function
 * @return				true if connection successfully set up
 * @note
 * This function can not read the full page because of memory limitations
 */
int	libMU_Internet_PostPage( char *url, char *post, libMU_Internet_PageReaderFunction_t function )
{
	if( function == NULL || url == NULL ) return 0;

	// Variable to store host name
	char host_name[100];

	char* host = strchr( url, ':' );
	if( host == NULL ) {
		host = url;
	}else{
		host += 3; // Skip "://" in "http://..."
	}
	char *end;
	end = strchr( host, ':' );	// http://www.google.com:8080/...
	if( end == NULL ) {
		end = strchr( host, '/' );	// http://www.google.com/...
	}
	if( end == NULL ) {
		if( strlen(host) < sizeof(host_name)-1 ) {
			strcpy( host_name, host );
		}else{
			return 0;
		}
	}else{
		if( (end - host) < sizeof(host_name)-1 ) {
			memcpy( host_name, host, (end-host) );
			host_name[ end-host ] = 0;
		}else{
			return 0;
		}
	}
	// Get port number (if present)
	int port = 80;
	if( end != NULL && end[0] == ':' ) {
		port = strtoul( end+1, &end, 10 );
	}
	// Get the rest of the URL
	char* url_path = "/";
	if( end !=NULL && end[0] == '/' ) url_path = end;

	// Empieza la lectura de página
	libMU_Internet_PageReaderFunction = function;
	// No he conseguido hacer que resuelva el dominio, asi que pasa directamente la ip y listo
	int res = webclient_post( "66.180.175.246", port, url_path, post);
	return res;
}

/**
 * @return true if has finished reading the page (or an error has happened)
 * @note
 * See libMU_Internet_GetPage() and webclient_datahandler()
 */
int	libMU_Internet_IsPageReadingFinished( void )
{
	return ( libMU_Internet_PageReaderFunction == NULL );
}

/**
 * Set page for the local web server at port 80, that is used as a response when it receives
 * a GET command with the corresponding tag
 * @param	tag		Page identifier name (e.g., "/index.html")
 * @param	data	Web page data (if NULL then it erases the corresponding tag)
 * @param	len		Web page data length (if 0 then it erases the corresponding tag)
 * @return			true if the page has been stored
 */
int libMU_Internet_Server_SetPage( char* tag, char* data, int len )
{
	if( libMU_Internet_Server_TagNumber >= NETWORK_MAX_TAGS || tag == NULL ) return 0;
	if( len < 0 && len != NETWORK_SUBMIT_TAG ) return 0;
	int i;
	
	// Busca si existe anteriormente
	for( i = 0; i < libMU_Internet_Server_TagNumber; i++) {
		if( strcmp( libMU_Internet_Server_Responses[i].tag, tag ) == 0 ) break;
	}
	if( data == NULL || len == 0 ) {
		// Si la entrada no encontrada no borrar
		if( i == libMU_Internet_Server_TagNumber ) return 0;
		// Borrar entrada actual
		libMU_Internet_Server_TagNumber--;
		for(; i < libMU_Internet_Server_TagNumber; i++) {
			libMU_Internet_Server_Responses[i] = libMU_Internet_Server_Responses[i+1];
		}
		// Borrar última entrada
		libMU_Internet_Server_Responses[i].tag = NULL;
		libMU_Internet_Server_Responses[i].data = NULL;
		libMU_Internet_Server_Responses[i].length = 0;
	}else{
		// Añadir entrada
		libMU_Internet_Server_Responses[i].tag = tag;
		libMU_Internet_Server_Responses[i].data = data;
		libMU_Internet_Server_Responses[i].length = len;
		libMU_Internet_Server_TagNumber++;
	}
	return 1;
}

/**
 * Set callback function for form parameter processing that is used as a response when it receives
 * a GET/POST command with the corresponding tag
 * @param	tag		Command processing page identification name (e.g., "/cmd")
 * @param	fun		Callback function to process form parameters
 * @return			true if the function has been stored
 * @note
 */
int libMU_Internet_Server_SetCommandProcessingInfo( char* tag, libMU_Internet_CommandProcessingFunction_t fun )
{
	char* f = (char*)fun;
	return libMU_Internet_Server_SetPage( tag, f, NETWORK_SUBMIT_TAG );
}

/**
 * Get an integer parameter value from a form parameter list given
 * with the GET/POST method
 * @param	cmd_params	GET/POST method parameter list
 * @param	param_tag	Parameter to search (e.g. "x=")
 * @param	result		Pointer to integer where the result will be stored if the parameter is found
 * @return				true if parameter found
 */
int libMU_Internet_GetFormIntegerValue( const char* cmd_params, const char* param_tag, int* result )
{
	char* p = strstr( cmd_params, param_tag ); unsigned int num = 0; int sign = 1;
	if( p != NULL && ( p[-1]=='&' || p == cmd_params ) ) {
		p += strlen(param_tag);
		if( *p == '-' ) {
			sign = -1; p++;
		}
		while( *p >= '0' && *p <= '9' ) {
			num *= 10; num += *p - '0'; p++;
		}
		*result = num*sign;
		return 1;
	}
	return 0;
}

/**
 * Get a string parameter value from a form parameter list given
 * with the GET/POST method
 * @param	cmd_params	GET/POST method parameter list
 * @param	param_tag	Parameter to search (e.g. "x=")
 * @param	result		Pointer to the character array where the result will be stored if the parameter is found
 * @param	result_size	Maximum number of characters available at the given address pointer
 * @return				True if parameter found
 */
#define	TAG_MAX_DIGIT	2
int libMU_Internet_GetFormStringValue( const char* cmd_params, const char* param_tag, char* result, int result_size )
{
	char* p = strstr( cmd_params, param_tag ); result_size--; // Leave space for string ending character '\0'
	if( p != NULL && ( p[-1]=='&' || p == cmd_params ) ) {
		int i, num_digit; char c, d;
		p += strlen( param_tag );
		for( i = 0; *p && *p != '&' && i < result_size; i++, p++ ) {
			switch(*p) {
			case '%':
				num_digit = 0; c = 0;
				while( isxdigit( p[1] ) && num_digit < TAG_MAX_DIGIT ) {
					c <<= 4; p++; d = p[0]; num_digit++;
					if( d > '9' ) d -= ('A' - 10); else d -= '0';
					if( d > 0xF ) d -= ('a' - 'A');
					c += d;
				}
				if( num_digit > 0 ) result[i] = c; else result[i] = '%';
				break;
			case '+': result[i] = ' '; break;
			default:  result[i] = *p;
			}
		}
		result[i] = 0;
		return 1;
	}
	return 0;
}

/**
 * Internal function used by the web server to compare to URL paths
 * @param	str1	1st URL path
 * @param	str2	2nd URL path
 * @return			0 if equal paths
 * @note
 * The comparison DOES NOT take the parameters into account
 */
int httpd_fs_strcmp(const char *str1, const char *str2)
{
	int i = 0;
	for(;;) {
		if( str2[i] == 0 ) {
			if( str1[i] == 0 || str1[i] == '?' || str1[i] == '\r' || str1[i] == '\n' ) {
				return 0;
			}
			return 1;
		}
		if( str1[i] == 0 ) {
			if( str2[i] == '?' || str2[i] == '\r' || str2[i] == '\n' ) {
				return 0;
			}
			return 1;
		}
		if( str1[i] != str2[i] ) return 1;
		i++;
	}
}

/**
 * Internal callback function used by the web server to obtain information
 * about a file specified by an URL path
 * @param	name	URL path of the file to open
 * @param	file	file information returned by the function (data location & size)
 * @return			1 if file is found
 * @note
 * - It uses the server web page information array (libMU_Internet_Server_Responses)
 */
int httpd_fs_open(const char *name, struct httpd_fs_file *file)
{
	libMU_Internet_Tags_t* f = libMU_Internet_Server_Responses;
	libMU_Internet_Tags_t* end = libMU_Internet_Server_Responses + libMU_Internet_Server_TagNumber;
	for(; f < end; f++ ) {
		if( httpd_fs_strcmp( name, f->tag ) == 0 ) {
			int len = 0; char *data = NULL;
			if( f->length == NETWORK_SUBMIT_TAG ) {
				libMU_Internet_CommandProcessingFunction_t fun;
				fun = (libMU_Internet_CommandProcessingFunction_t)( f->data );
				httpd_script_param [ sizeof( httpd_script_param ) -  1 ] = 0;
				data = fun( httpd_script_param, &len );
			}else{
				data = f->data;
				len = f->length;
			}
			if( data == NULL || len < 0 ) return 0;
			file->data = data;
			file->len = len;
			return 1;
		}
	}
	return 0;
}

/**
 * Obtain WiFi router IP address using telnet protocol
 * @return	WiFi Router IP address
 * @note 	This function has a very long timeout in the worst case (15s) 
 */
IPAddress_t	libMU_Internet_GetWiFiRouterIP( void )
{
	#define	RESP_SIZE	20
	libMU_uip_Address_t wifi_ip; char* p;
	uip_ipaddr_t	telnet_server;
	char tl_msg1[RESP_SIZE] = "1";
	char tl_msg2[RESP_SIZE] = "2";
	char tl_msg3[RESP_SIZE] = "3";

	uip_getdraddr( &telnet_server );	// Telnet server in the default router (WiFi bridge)			
    if( !telnet_client_open( &telnet_server, TELNET_SERVER_PORT ) )
    	return 0;
    if( !telnet_client_cmd ( "auth dpac dpac\r\n", tl_msg1, RESP_SIZE, 10000 ) )
    	return 0;
    if( !telnet_client_cmd ( "wl-ip\r\n", tl_msg2, RESP_SIZE, 5000 ) )
    	return 0;
    telnet_client_cmd ( "quit\r\n", tl_msg3, RESP_SIZE, 500 );
	p = strchr( tl_msg2, '\r' ); 
	if( p != NULL ) *p = 0;
	if( !uiplib_ipaddrconv( tl_msg2, (uint8_t*)wifi_ip.uip ) ) 
		return 0;
   	return wifi_ip.libmu;
}

/**
 * Internal callback function used by the DHCP client to configure the TCPIP stack.
 * Specifically: The device IP address, netmask, router IP address, DNS server IP address
 * @param	s		DHCP client state information structure
 * @note
 * - This function only exists if we are using the DHCP client, i.e., not a static IP
 *   configuration.
 */
#ifndef USE_STATIC_IP
void dhcpc_configured(const struct dhcpc_state *s)
{
    uip_sethostaddr	( s->ipaddr );
    uip_setnetmask	( s->netmask );
    uip_setdraddr	( s->default_router );
	resolv_conf		( s->dnsaddr );
	network_state	= NETWORK_IP_OK;
}
#endif

/**
 * Internal callback function used by the UIP web client.
 * Called when a data packet is received
 * @param	data	Pointer to the packet data
 * @param	len		Length of the packet data
 * @note
 * This function calls the user page reader function
 *   "libMU_Internet_PageReaderFunction"
 */
void webclient_datahandler( char *data, unsigned short int len )
{
	if( data == NULL || len == 0 ) libMU_Internet_PageReaderFunction = NULL;	// Fin de la comunicación
	if( libMU_Internet_PageReaderFunction == NULL ) return;
	libMU_Internet_PageReaderFunction( data, len );
}

/**
 * Internal callback function used by the UIP web client.
 * Called when connection is aborted (error reading a page, finish)
 */
void webclient_aborted(void) { libMU_Internet_PageReaderFunction = NULL; }

/**
 * Internal callback function used by the UIP web client.
 * Called when connection is closed (finished reading a page)
 */
void webclient_closed(void) { libMU_Internet_PageReaderFunction = NULL; }

/**
 * Internal callback function used by the UIP web client.
 * Called when connection is established
 */
void webclient_connected(void) {}

/**
 * Internal callback function used by the UIP web client.
 * Called when connection is timed out
 */
void webclient_timedout(void) {}

/**
 * @}
 */
