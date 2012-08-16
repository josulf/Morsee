#ifndef SPECSERVER_APP_H_
#define SPECSERVER_APP_H_

#include <uip/uipopt.h>
#include <stdint.h>

/************************************************/
/* Type definitions								*/
/************************************************/

/************************************************/
/* Constants									*/
/************************************************/
#define	SPECIAL_SERVER_PORT	8080

/************************************************/
/* Global vars									*/
/************************************************/

/************************************************/
/* Prototypes									*/
/************************************************/
void specserver_app_init(void);
void specserver_appcall(void);
char*	i2dec(char *buf, int number, int width);
char*	i2hex(char *buf, uint32_t number, int width);

#endif /*SPECSERVER_APP_H_*/
