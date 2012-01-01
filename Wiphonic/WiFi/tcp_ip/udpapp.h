
/******************************************************************************

  Filename:		udpapp.h
  Description:	UDP app for the WiShield 1.0

 ******************************************************************************

  TCP/IP stack and driver for the WiShield 1.0 wireless devices

  Copyright(c) 2009 Async Labs Inc. All rights reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc., 59
  Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  Contact Information:
  <asynclabs@asynclabs.com>

   Author               Date        Comment
  ---------------------------------------------------------------
   AsyncLabs			07/11/2009	Initial version

 *****************************************************************************/

#ifndef __UDPAPP_H__
#define __UDPAPP_H__

#include "pt.h"
#include "psock.h"

/*
typedef struct socket_app_state {
  char name;
} uip_tcp_appstate_t; */

// TCP
void tcp_appcall(unsigned tcp_chan);
// #ifndef UIP_APPCALL
#define UIP_APPCALL tcp_appcall
// #endif /* UIP_APPCALL */

typedef struct socket_app_state
{
  struct psock p;
  char inputbuffer[20];
  // char name[20];
  int state;
} uip_tcp_appstate_t;

typedef struct udpapp_state
{
	struct pt pt;
	char state;
	char inputbuf[10];
} uip_udp_appstate_t;

void udpapp_appcall(unsigned tx_mp3);
#ifndef UIP_UDP_APPCALL
#define UIP_UDP_APPCALL udpapp_appcall
#endif /* UIP_UDP_APPCALL */

void udpapp_init(void);
void tcp_app_init(void);

#endif /* __UDPAPP_H__ */
