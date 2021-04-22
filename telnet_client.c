//******************************************************************************
//! \file telnet_client.c
//! \brief Telnet is a terminal connection program.
//! \details Telnet program makes a TCP connection to a server computer
//! from a client computer and allows the client to perform operations on the
//! server computer. 
//!	see https://en.wikipedia.org/wiki/Telnet for details.	
//!
//! \version 1.0
//! \date 4/13/2019
//! \par Revision history
//! 	7/5/2018  
//!  		Based on code found at: http://l3net.wordpress.com/2012/12/09/a-simple-telnet-client	
//!		published at https://sourceforge.net/projects/simpletelnet on 10/16/2013.
//!		Can't find a license file or an author.
//!
//! 	4/13/2019 modified to work with CP/M and W5500 code base.
//!
//! \author Jay Cotton
//! \copyright
//!
//! Copyright (c)  2018-2019, Jay Cotton.
//! All rights reserved.
//!
//! Redistribution and use in source and binary forms, with or without
//! modification, are permitted provided that the following conditions
//! are met:
//!
//!     * Redistributions of source code must retain the above copyright
//! notice, this list of conditions and the following disclaimer.
//!     * Redistributions in binary form must reproduce the above copyright
//! notice, this list of conditions and the following disclaimer in the
//! documentation and/or other materials provided with the distribution.
//!     * Neither the name of the <ORGANIZATION> nor the names of its
//! contributors may be used to endorse or promote products derived
//! from this software without specific prior written permission.
//!
//! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//! AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//! ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
//! LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//! CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//! SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//! INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//! CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//! ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
//! THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

/* http://l3net.wordpress.com/2012/12/09/a-simple-telnet-client/ */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "ethernet.h"
#include "dhcp.h"
#include "dns.h"
#include "socket.h"
#include "spi.h"
#include "w5500.h"
#include "trace.h"

#define DO 0xfd
#define WONT 0xfc
#define WILL 0xfb
#define DONT 0xfe
#define CMD 0xff
#define CMD_ECHO 1
#define CMD_WINDOW_SIZE 31

unsigned char tmp2[10] = { 255, 250, 31, 0, 80, 0, 24, 255, 240 };
unsigned char tmp1[10] = { 255, 251, 31 };

void
negotiate (int sock, unsigned char *buf, int len)
{
  int i;

  if (buf[1] == DO && buf[2] == CMD_WINDOW_SIZE)
    {
      if (send (sock, tmp1, 3) < 0)
	exit (1);

      if (send (sock, tmp2, 9) < 0)
	exit (1);
      return;
    }

  for (i = 0; i < len; i++)
    {
      if (buf[i] == DO)
	buf[i] = WONT;
      else if (buf[i] == WILL)
	buf[i] = DO;
    }

  if (send (sock, buf, len) < 0)
    exit (1);
}

unsigned char dnsname[80];
wiz_NetInfo gWIZNETINFO;
unsigned char DNS_buffer[256];
unsigned char skip_dns;
unsigned char run_user_applications;
extern char HostAddr[4];
unsigned char mac[6] = { 0x98, 0x76, 0xb6, 0x11, 0x00, 0xc4 };


#define SOCK_DNS 6
#define BUFLEN 2048
int
main (int argc, char *argv[])
{
  int sock;
  unsigned char buf[BUFLEN + 1];
  int len;
  int i;
  int nready;
  int rv;
  unsigned char *p;

  skip_dns = 0;
  if (argc < 2 || argc > 4)
    {
      printf ("Usage: %s address [port]\n", argv[0]);
      return 1;
    }
  int port = 23;
  if (argc == 3)
    port = atoi (argv[2]);
  memset (dnsname, 0, 80);
  memcpy (gWIZNETINFO.mac, mac, 6);
  if (argc > 1)
    {
      strcat (dnsname, argv[1]);
      if (isdigit (dnsname[0]))
	{
	  for (i = 0; i < 4; i++)
	    {
	      if (p = (unsigned char *)strchr (dnsname, '.'))
		{
		  *p = ' ';
		}
	    }
	  sscanf (dnsname, "%d %d %d %d",
		  &HostAddr[0], &HostAddr[1], &HostAddr[2], &HostAddr[3]);
	  skip_dns = 1;
	}
    }
  TRACE ("Ethernet_begin");
  if (Ethernet_begin (mac) == 0)
    {
      if (Ethernet_hardwareStatus () == EthernetNoHardware)
	printf ("Can't find the ethernet h/w\n");
      if (Ethernet_linkStatus () == LinkOFF)
	printf ("Plug in the cable\n");
    }
  Ethernet_localIP (gWIZNETINFO.ip);
  Ethernet_localDNS (gWIZNETINFO.dns);
  if (skip_dns == 0)
    {
      TRACE ("DNS_init");
      DNS_init (SOCK_DNS, DNS_buffer);	// share the data buffer ??
      TRACE ("DNS_run");
      if (DNS_run (gWIZNETINFO.dns, dnsname, HostAddr) == 0)
	exit (0);
    }
  TRACE ("Socket");
  //Create socket  
  sock = socket (0, Sn_MR_TCP, SOCK_STREAM, 0);
  if (sock == -1)
    {
      printf ("Could not create socket. Error %d", sock);
      return 1;
    }
  TRACE ("Connect");
  //Connect to remote server
  if ((rv = connect (sock, HostAddr, port)) < 0)
    {
      printf ("connect failed. Error %d", rv);
      return 1;
    }
  puts_cons ("Connected...\n");

  while (1)
    {
      nready = getSn_RX_RSR (sock);
      if (nready < 0)
	{
	  //printf ("select. Error\n");
	  return 1;
	}
      else if (nready > 0)
	{
	  // start by reading a single byte
	  if ((rv = recv (sock, buf, 1)) < 0)
	    return 1;
	  else if (rv == 0)
	    {
	      //printf ("Connection closed by the remote end\n\r");
	      return 0;
	    }

	  if (buf[0] == CMD)
	    {
	      // read 2 more bytes
	      len = recv (sock, buf + 1, 2);
	      if (len < 0)
		return 1;
	      else if (len == 0)
		{
		 // printf ("Connection closed by the remote end\n\r");
		  return 0;
		}
	      negotiate (sock, buf, 3);
	    }
	  else
	    {
	      fputc_cons (*buf);
// try to speed up the server to client connection.
	      nready = getSn_RX_RSR (sock);
	      if (nready <= BUFLEN)
		{
		  recv (sock, buf, nready);
		  p = buf;
		  while (nready--)
		    {
		      fputc_cons (*p++);
		    }
		}
	    }
	}

      if (bdos (CPM_ICON, 0))
	{
	  buf[0] = fgetc_cons ();	//fgets(buf, 1, stdin);
	  if (send (sock, buf, 1) < 0)
	    return 1;
	  if (buf[0] == '\n')	// with the terminal in raw mode we need to force a LF
	    fputc_cons ('\r');
	}
    }
  sock_close (sock);
  return 0;
}
