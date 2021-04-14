/* htlist files on the server  

This is a hacked version of htget.c from fuzix.  

The intent is to have a command that just copies files from the
http server to the CP/M system.  The server address is known and
does not change, nor will the port change.  

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "socket.h"
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
#include "inet.h"


static int sock;
static char buf[512];
static char *bufend = buf;
static char *readp = buf;
static int buflen = 0;

void
writes (int fd, char *p)
{
  fd = 0;
  puts(p);
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
      memcpy (buf, readp, (int) (bufend - readp));
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
      memcpy (buf, readp, (int) (bufend - readp));
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
FILE *fp;
void
main ()
{
  uint16_t port = 80;
  char *pp;
  int code;
  int len;
  uint8_t looped = 0;

  InetGetMac (gWIZNETINFO.mac);

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
  DNS_init (SOCK_DNS, DNS_buffer);
  DNS_run (gWIZNETINFO.dns, "server", HostAddr);
  sock = socket (0, Sn_MR_TCP, SOCK_STREAM, 0);
  if (sock == -1)
    {
      printf ("ERROR:socket");
      exit (1);
    }
  if (connect (sock, HostAddr, port) < 0)
    {
      printf ("ERROR:connect");
      exit (1);
    }

/* there be bugs here.  If a port is specified 
	things can get sideways */

  sprintf (dnsname, "%d.%d.%d.%d", HostAddr[0], HostAddr[1],
	   HostAddr[2], HostAddr[3]);

  xwrites ("GET /");
  xwrites (" HTTP/1.1\r\n");
  xwrites ("Host: ");
  xwrites (dnsname);
  xwrites ("\r\nUser-Agent: cp/m htlist v0 \r\nConnection: close\r\n\r\n");
  xflush ();

  do
    {
      xreadline ();
      errno = 0;
      pp = strchr (buf, ' ');
      if (pp == NULL)
	{
	  writes (2, "get: invalid reply\n");
	  writes (2, buf);
	  exit (1);
	}
      pp++;
      code = strtoul (pp, &pp, 10);
      if (code < 100 || *pp++ != ' ')
	{
	  writes (2, "get: invalid reply\n");
	  writes (2, buf);
	  exit (1);
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

  /* FIXME: if we saw a Transfer-Encoding: chunked" we need to do this
     bit differently */
  if (code == 200)
    {
      writes (2, readp);
      while ((len = recv (sock, buf, 512)) > 0)
	{
	  writes (2, buf);
	}
    }
  putchar ('\n');
  putchar ('\r');
  sock_close (sock);
}
