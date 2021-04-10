//******************************************************************************
//
//! \file htget.c
//! \brief wget Implemtation file.
//! \details htget.c is from the fuzix project.
//!  https://github.com/EtchedPixels/FUZIX/tree/master/Applications/netd/htget.c
//!
//! \version 1.0
//! \date 4/13/2019
//! \par Revision history
//! The original code is from the fuzix project.  
//!     04/13/2019  Modified to work with W5500 and CP/M using z88dk
//!     5/8,2019    Modified to include CTC timer and instrumentation
//! \author Alan Cox
//! \copyright
//!  I did not find a copyright file but I blame Alan for all the
//!  good stuff in this file.
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "socket.h"
//#include <netinet/in.h>
//#include <arpa/inet.h>
#include "netdb.h"
#include "wizchip_conf.h"
#include "ethernet.h"
#include "dhcp.h"
#include "dns.h"
#include "socket.h"
#include "spi.h"
#include "w5500.h"
#include "trace.h"
#include "ctype.h"
#include "ltime.h"

static int sock;
static char buf[512];
static char *bufend = buf;
static char *readp = buf;
static int buflen = 0;

uint32_t bytecount;
uint32_t timecount;
uint32_t lastcount;

void
writes (int fd, const char *p)
{
  puts_cons (p);
}

void
xflush (void)
{
  if (send (sock, buf, buflen) != buflen)
    {
      printf ("ERROR:send");
      exit (1);
    }
  buflen = 0;
}

void
xwrites (const char *p)
{
  printf ("%s", p);
  int l = strlen (p);
  if (l + buflen > 512)
    xflush ();
  memcpy (buf + buflen, p, l);
  buflen += l;
}

int
xread (void)
{
  int len;

  /* The first call we return the stray bytes from the line parser */
  if (bufend != buf && bufend > readp)
    {
      len = bufend - readp;
      memcpy (buf, readp, (int)(bufend - readp));
      bufend = buf;
      readp = buf;
      return len;
    }
  len = recv (sock, buf, 512);
  if (len < 0)
    {
      printf ("ERROR:read %d\n", len);
      exit (1);
    }
  return len;
}

int
xreadline (void)
{
  int len;

  if (readp != buf && bufend > readp)
    {
      memcpy (buf, readp, (int)(bufend - readp));
      bufend -= (readp - buf);
    }
  readp = buf;

  len = recv (sock, buf + buflen, 512 - buflen);
  if (len < 0)
    {
      printf ("ERROR:socket read");
      exit (1);
    }
  buflen += len;
  bufend += len;

  while (readp < bufend)
    {
      if (*readp == '\r' && readp[1] == '\n')
	{
	  *readp++ = '\n';
	  *readp++ = 0;
	  len = readp - buf;
	  buflen -= len;
	  return len;
	}
      readp++;
    }
  writes (2, "htpget: overlong/misformatted header\n");
  exit (1);
return 0;
}

struct wiz_NetInfo_t gWIZNETINFO;
unsigned char mac[6] = { 0x98, 0x76, 0xb6, 0x11, 0x00, 0xc4 };

int skip_dns;
char DNS_buffer[256];
char dnsname[80];
extern unsigned char HostAddr[4];
unsigned char run_user_applications;
int
main (int argc, char *argv[])
{
  struct sockaddr_in sin;
  struct hostent *h;
  uint16_t port = 80;
  char *pp;
  unsigned char *p;
  char *fp;
  int i;

  int of;
  int code;
  int len;
  uint8_t looped = 0;
  int stat;

  skip_dns = 0;

  if (argc != 3)
    {
      writes (2, "wget url file\n");
	exit(0);
    }
  if (strncmp (argv[1], "HTTP://", 7))
    {
      writes (2, "wget: only http:// is supported.\n");
	exit(1);
    }
  argv[1] += 7;

  fp = strchr (argv[1], '/');
  if (fp)
    *fp++ = 0;

  pp = strrchr (argv[1], ':');
  if (pp)
    {
      *pp++ = 0;
      port = atoi (pp);
      if (port == 0)
	{
	  writes (2, "wget: invalid port\n");
		exit(1);
	}
    }
  memset (dnsname, 0, 80);
  memcpy (gWIZNETINFO.mac, mac, 6);
  strcat (dnsname, argv[1]);
  if (isdigit (dnsname[0]))
    {
      for (i = 0; i < 4; i++)
	if (p = (unsigned char *) strchr (dnsname, '.'))
	  *p = ' ';
      sscanf (dnsname, "%d %d %d %d",
	      &HostAddr[0], &HostAddr[1], &HostAddr[2], &HostAddr[3]);
      skip_dns = 1;
    }
  TRACE ("Ethernet_begin");
  if (Ethernet_begin (mac) == 0)
    {
      if (Ethernet_hardwareStatus () == EthernetNoHardware)
	printf ("Can't find the ethernet h/w\n");
      if (Ethernet_linkStatus () == LinkOFF)
	printf ("Plug in the cable\n");
	exit(1);
    }
  Ethernet_localIP (gWIZNETINFO.ip);
  Ethernet_localDNS (gWIZNETINFO.dns);
  if (skip_dns == 0)
    {
      TRACE ("DNS_init");
      DNS_init (SOCK_DNS, DNS_buffer);	// share the data buffer ??
      TRACE ("DNS_run");
      if (DNS_run (gWIZNETINFO.dns, dnsname, HostAddr) == 0)
	exit(1);
    }

  sock = socket (0, Sn_MR_TCP, SOCK_STREAM, 0);
  if (sock == -1)
    {
      printf ("ERROR:socket");
	exit(1);
    }
  if (connect (sock, HostAddr, port) < 0)
    {
      printf ("ERROR:connect");
	exit(1);
    }

/* there be bugs here.  If a port is specified 
things can get sideways */

  if (port == 80)
    {
      fp = argv[2];
    }
  else
    {
      fp = argv[3];
    }

  xwrites ("GET /");
  if (fp)
    xwrites (fp);
  xwrites (" HTTP/1.1\r\n");
  xwrites ("Host: ");
  xwrites (argv[1]);
  if (pp)
    {
      xwrites (":");
      xwrites (pp);
    }
  xwrites ("\r\nUser-Agent: Fuzix-htget/0.1\r\nConnection: close\r\n\r\n");
  xflush ();

  do
    {
      xreadline ();
      errno = 0;
      pp = strchr (buf, ' ');
      if (pp == NULL)
	{
	  writes (2, "wget: invalid reply\n");
	  writes (2, buf);
	exit(1);
	}
      pp++;
      code = strtoul (pp, &pp, 10);
      if (code < 100 || *pp++ != ' ')
	{
	  writes (2, "wget: invalid reply\n");
	  writes (2, buf);
	exit(1);
	}

      if (code != 200)
	writes (2, buf);
      do
	{
	  xreadline ();
	  if (code != 200)
	    writes (2, buf);
	}
      while (*buf != '\n');

      /* A 100 code means "I'm thinking please wait then a header cycle then
         a real header and has a blank line following */
      if (code == 100)
	xreadline ();
    }
  while (code == 100 && !looped++);

  of = open (argv[2], O_WRONLY | O_CREAT, 0666);
  if (of == -1)
    {
      printf ("ERROR:%s\n", argv[2]);
	exit(1);
    }
  /* FIXME: if we saw a Transfer-Encoding: chunked" we need to do this
     bit differently */
  if (code == 200)
    {
      if (write (of, readp, (int)(bufend - readp)) < 0)
	{
	  printf ("ERROR:write");
	exit(1);
	}
      while ((len = recv (sock, buf, 512)) > 0)
	{
	  if (write (of, buf, len) != len)
	    {
	      printf ("ERROR:write");
		exit(1);
	    }
		printf(".");
	}
    }
  close (of);
  printf("\n");
  sock_close (sock);
// stats
  return 0;
}
