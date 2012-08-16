/**
 * \addtogroup apps
 * @{
 */

/**
 * \defgroup webclient Web client
 * @{
 *
 * This example shows a HTTP client that is able to download web pages
 * and files from web servers. It requires a number of callback
 * functions to be implemented by the module that utilizes the code:
 * webclient_datahandler(), webclient_connected(),
 * webclient_timedout(), webclient_aborted(), webclient_closed().
 */

/**
 * \file
 * Implementation of the HTTP client.
 * \author Adam Dunkels <adam@dunkels.com>
 */

/*
 * Copyright (c) 2002, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: webclient.c,v 1.2 2006/06/11 21:46:37 adam Exp $
 *
 */

#include "uip.h"
#include "uiplib.h"
#include "webclient.h"
#include "resolv.h"
#include "stdio.h"

#include <string.h>

#define WEBCLIENT_TIMEOUT 100

#define WEBCLIENT_STATE_STATUSLINE 0
#define WEBCLIENT_STATE_HEADERS    1
#define WEBCLIENT_STATE_DATA       2
#define WEBCLIENT_STATE_CLOSE      3

#define HTTPFLAG_NONE   0
#define HTTPFLAG_OK     1
#define HTTPFLAG_MOVED  2
#define HTTPFLAG_ERROR  3


#define ISO_nl       0x0a
#define ISO_cr       0x0d
#define ISO_space    0x20

#define TWITTER_USER	"user"
#define TWITTER_PASS	"pass"
// "user:pass" en base64
#define TWITTER_AUTH	"base64(user:pass)"

static struct webclient_state s;

/*-----------------------------------------------------------------------------------*/
char *
webclient_mimetype(void)
{
  return s.mimetype;
}
/*-----------------------------------------------------------------------------------*/
char *
webclient_filename(void)
{
  return s.file;
}
/*-----------------------------------------------------------------------------------*/
char *
webclient_hostname(void)
{
  return s.host;
}
/*-----------------------------------------------------------------------------------*/
unsigned short
webclient_port(void)
{
  return s.port;
}
/*-----------------------------------------------------------------------------------*/
void
webclient_init(void)
{

}
/*-----------------------------------------------------------------------------------*/
static void
init_connection(void)
{
  s.state = WEBCLIENT_STATE_STATUSLINE;

  char length[8];
  snprintf(length, 8, "%d", strlen(s.post));

  // Tamaño de lo que vamos a enviar
  s.getrequestleft = strlen(http_post) + strlen(s.file) + 1 + strlen(http_11) + strlen(http_crnl) +
  	strlen(twitter_auth) + strlen(TWITTER_AUTH) + strlen(http_crnl) +
  	strlen(twitter_agent) + strlen(http_crnl) +
  	strlen(http_host) + strlen("api.supertweet.net") + strlen(http_crnl) +
  	strlen(http_accept) + strlen(http_crnl) +
  	strlen(twitter_length) + strlen(length) + strlen(http_crnl) +
  	strlen(twitter_type) + strlen(http_crnl) + strlen(http_crnl) +
  	strlen(s.post);
  s.getrequestptr = 0;

  s.httpheaderlineptr = 0;
}
/*-----------------------------------------------------------------------------------*/
void
webclient_close(void)
{
  s.state = WEBCLIENT_STATE_CLOSE;
}
/*-----------------------------------------------------------------------------------*/
unsigned char
webclient_get(char *host, u16_t port, char *file)
{
  struct uip_conn *conn;
  uip_ipaddr_t *ipaddr;
  static uip_ipaddr_t addr;
  
  /* First check if the host is an IP address. */
  ipaddr = &addr;
  if(uiplib_ipaddrconv(host, (unsigned char *)addr) == 0) {
    ipaddr = (uip_ipaddr_t *)resolv_lookup(host);
    
    if(ipaddr == NULL) {
      return 0;
    }
  }
  
  conn = uip_connect(ipaddr, htons(port));
  
  if(conn == NULL) {
    return 0;
  }
  
  s.port = port;
  strncpy(s.file, file, sizeof(s.file));
  strncpy(s.host, host, sizeof(s.host));
  
  init_connection();
  return 1;
}
/*-----------------------------------------------------------------------------------*/
unsigned char
webclient_post(char *host, u16_t port, char *file, char *post)
{
  struct uip_conn *conn;
  uip_ipaddr_t *ipaddr;
  static uip_ipaddr_t addr;

  /* First check if the host is an IP address. */
  ipaddr = &addr;
  if(uiplib_ipaddrconv(host, (unsigned char *)addr) == 0) {
    ipaddr = (uip_ipaddr_t *)resolv_lookup(host);

    if(ipaddr == NULL) {
      return 0;
    }
  }

  conn = uip_connect(ipaddr, htons(port));

  if(conn == NULL) {
    return 0;
  }

  s.port = port;
  strncpy(s.file, file, sizeof(s.file));
  strncpy(s.host, host, sizeof(s.host));
  strncpy(s.post, post, sizeof(s.post));

  init_connection();
  return 1;
}
/*-----------------------------------------------------------------------------------*/
static char *
copy_string(char *dest,
	    const char *src, unsigned char len)
{
  strncpy(dest, src, len);
  return dest + len;
}

// Prepara los datos para enviar mediante POST
static void
senddata(void)
{
  u16_t len;
  char *getrequest;
  char *cptr;

  if(s.getrequestleft > 0) {
    cptr = getrequest = (char *)uip_appdata;

    // POST
    cptr = copy_string(cptr, http_post, strlen(http_post));
    cptr = copy_string(cptr, s.file, strlen(s.file));
    *cptr++ = ISO_space;
    cptr = copy_string(cptr, http_11, strlen(http_11));
    cptr = copy_string(cptr, http_crnl, strlen(http_crnl));

    // Auth
    cptr = copy_string(cptr, twitter_auth, strlen(twitter_auth));
    cptr = copy_string(cptr, TWITTER_AUTH, strlen(TWITTER_AUTH));
    cptr = copy_string(cptr, http_crnl, strlen(http_crnl));

    // User-Agent
    cptr = copy_string(cptr, twitter_agent, strlen(twitter_agent));
    cptr = copy_string(cptr, http_crnl, strlen(http_crnl));

    // Host
    // Como se ha usado la ip a la hora de crear el socket, aqui hay que
    // pasar la URL, si no da error
    cptr = copy_string(cptr, http_host, strlen(http_host));
    cptr = copy_string(cptr, "api.supertweet.net", strlen("api.supertweet.net"));
    cptr = copy_string(cptr, http_crnl, strlen(http_crnl));

    // Accept
    cptr = copy_string(cptr, http_accept, strlen(http_accept));
    cptr = copy_string(cptr, http_crnl, strlen(http_crnl));

    // Content-length
    cptr = copy_string(cptr, twitter_length, strlen(twitter_length));
    char length[8];
    snprintf(length, 8, "%d", strlen(s.post));
    cptr = copy_string(cptr, length, strlen(length));
    cptr = copy_string(cptr, http_crnl, strlen(http_crnl));

    // Content-type
    cptr = copy_string(cptr, twitter_type, strlen(twitter_type));
    cptr = copy_string(cptr, http_crnl, strlen(http_crnl));
    cptr = copy_string(cptr, http_crnl, strlen(http_crnl));

    // Post
    cptr = copy_string(cptr, s.post, strlen(s.post));

    len = s.getrequestleft > uip_mss()?
      uip_mss():
      s.getrequestleft;
    uip_send(&(getrequest[s.getrequestptr]), len);
  }
}

/*-----------------------------------------------------------------------------------*/
static void
acked(void)
{
  u16_t len;
  
  if(s.getrequestleft > 0) {
    len = s.getrequestleft > uip_mss()?
      uip_mss():
      s.getrequestleft;
    s.getrequestleft -= len;
    s.getrequestptr += len;
  }
}
/*-----------------------------------------------------------------------------------*/
static u16_t
parse_statusline(u16_t len)
{
  char *cptr;
  
  while(len > 0 && s.httpheaderlineptr < sizeof(s.httpheaderline)) {
    s.httpheaderline[s.httpheaderlineptr] = *(char *)uip_appdata;
    uip_appdata = ((char *)uip_appdata) + 1;	// ++((char *)uip_appdata);
    --len;
    if(s.httpheaderline[s.httpheaderlineptr] == ISO_nl) {

      if((strncmp(s.httpheaderline, http_10,
		  sizeof(http_10) - 1) == 0) ||
	 (strncmp(s.httpheaderline, http_11,
		  sizeof(http_11) - 1) == 0)) {
	cptr = &(s.httpheaderline[9]);
	s.httpflag = HTTPFLAG_NONE;
	if(strncmp(cptr, http_200, sizeof(http_200) - 1) == 0) {
	  /* 200 OK */
	  s.httpflag = HTTPFLAG_OK;
	} else if(strncmp(cptr, http_301, sizeof(http_301) - 1) == 0 ||
		  strncmp(cptr, http_302, sizeof(http_302) - 1) == 0) {
	  /* 301 Moved permanently or 302 Found. Location: header line
	     will contain thw new location. */
	  s.httpflag = HTTPFLAG_MOVED;
	} else {
	  s.httpheaderline[s.httpheaderlineptr - 1] = 0;
	}
      } else {
	uip_abort();
	webclient_aborted();
	return 0;
      }
      
      /* We're done parsing the status line, so we reset the pointer
	 and start parsing the HTTP headers.*/
      s.httpheaderlineptr = 0;
      s.state = WEBCLIENT_STATE_HEADERS;
      break;
    } else {
      ++s.httpheaderlineptr;
    }
  }
  return len;
}
/*-----------------------------------------------------------------------------------*/
static char
casecmp(char *str1, const char *str2, char len)
{
  static char c;
  
  while(len > 0) {
    c = *str1;
    /* Force lower-case characters. */
    if(c & 0x40) {
      c |= 0x20;
    }
    if(*str2 != c) {
      return 1;
    }
    ++str1;
    ++str2;
    --len;
  }
  return 0;
}
/*-----------------------------------------------------------------------------------*/
static u16_t
parse_headers(u16_t len)
{
  char *cptr;
  static unsigned char i;
  
  while(len > 0 && s.httpheaderlineptr < sizeof(s.httpheaderline)) {
    s.httpheaderline[s.httpheaderlineptr] = *(char *)uip_appdata;
    uip_appdata = ((char *)uip_appdata) + 1;	// ++((char *)uip_appdata);
    --len;
    if(s.httpheaderline[s.httpheaderlineptr] == ISO_nl) {
      /* We have an entire HTTP header line in s.httpheaderline, so
	 we parse it. */
      if(s.httpheaderline[0] == ISO_cr) {
	/* This was the last header line (i.e., and empty "\r\n"), so
	   we are done with the headers and proceed with the actual
	   data. */
	s.state = WEBCLIENT_STATE_DATA;
	return len;
      }

      s.httpheaderline[s.httpheaderlineptr - 1] = 0;
      /* Check for specific HTTP header fields. */
      if(casecmp(s.httpheaderline, http_content_type,
		     sizeof(http_content_type) - 1) == 0) {
	/* Found Content-type field. */
	cptr = strchr(s.httpheaderline, ';');
	if(cptr != NULL) {
	  *cptr = 0;
	}
	strncpy(s.mimetype, s.httpheaderline +
		sizeof(http_content_type) - 1, sizeof(s.mimetype));
      } else if(casecmp(s.httpheaderline, http_location,
			    sizeof(http_location) - 1) == 0) {
	cptr = s.httpheaderline +
	  sizeof(http_location) - 1;
	
	if(strncmp(cptr, http_http, 7) == 0) {
	  cptr += 7;
	  for(i = 0; i < s.httpheaderlineptr - 7; ++i) {
	    if(*cptr == 0 ||
	       *cptr == '/' ||
	       *cptr == ' ' ||
	       *cptr == ':') {
	      s.host[i] = 0;
	      break;
	    }
	    s.host[i] = *cptr;
	    ++cptr;
	  }
	}
	strncpy(s.file, cptr, sizeof(s.file));
	/*	s.file[s.httpheaderlineptr - i] = 0;*/
      }


      /* We're done parsing, so we reset the pointer and start the
	 next line. */
      s.httpheaderlineptr = 0;
    } else {
      ++s.httpheaderlineptr;
    }
  }
  return len;
}
/*-----------------------------------------------------------------------------------*/
static void
newdata(void)
{
  u16_t len;

  len = uip_datalen();

  if(s.state == WEBCLIENT_STATE_STATUSLINE) {
    len = parse_statusline(len);
  }
  
  if(s.state == WEBCLIENT_STATE_HEADERS && len > 0) {
    len = parse_headers(len);
  }

  if(len > 0 && s.state == WEBCLIENT_STATE_DATA &&
     s.httpflag != HTTPFLAG_MOVED) {
    webclient_datahandler((char *)uip_appdata, len);
  }
}
/*-----------------------------------------------------------------------------------*/
void
webclient_appcall(void)
{
  if(uip_connected()) {
    s.timer = 0;
    s.state = WEBCLIENT_STATE_STATUSLINE;
    senddata();
    webclient_connected();
    return;
  }

  if(s.state == WEBCLIENT_STATE_CLOSE) {
    webclient_closed();
    uip_abort();
    return;
  }

  if(uip_aborted()) {
    webclient_aborted();
  }
  if(uip_timedout()) {
    webclient_timedout();
  }

  
  if(uip_acked()) {
    s.timer = 0;
    acked();
  }
  if(uip_newdata()) {
    s.timer = 0;
    newdata();
  }
  if(uip_rexmit() ||
     uip_newdata() ||
     uip_acked()) {
    senddata();
  } else if(uip_poll()) {
    ++s.timer;
    if(s.timer == WEBCLIENT_TIMEOUT) {
      webclient_timedout();
      uip_abort();
      return;
    }
        /*    senddata();*/
  }

  if(uip_closed()) {
    if(s.httpflag != HTTPFLAG_MOVED) {
      /* Send NULL data to signal EOF. */
      webclient_datahandler(NULL, 0);
    } else {
      if(resolv_lookup(s.host) == NULL) {
		resolv_query(s.host);
      }
      webclient_get(s.host, s.port, s.file);
    }
  }
}
/*-----------------------------------------------------------------------------------*/

/** @} */
/** @} */
