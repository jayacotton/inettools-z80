/* GET <FILENAME>

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


static int sock;
static char buf[512];
static char *bufend = buf;
static char *readp = buf;
static int buflen = 0;

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
      memcpy (buf, readp, bufend - readp);
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
      memcpy (buf, readp, bufend - readp);
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
int
main (int argc, char *argv[])
{
  struct sockaddr_in sin;
  struct hostent *h;
  uint16_t port = 80;
  char *pp;
  char *p;
  int i;

  int of;
  int code;
  int len;
  uint8_t looped = 0;

  if (argc != 2)
    {
      printf ("GET <FILENAME>\n");
      exit (1);
    }
  memcpy (gWIZNETINFO.mac, mac, 6);
  fp = fopen ("MYGET.CFG", "r");
  fscanf (fp, "%d.%d.%d.%d", &HostAddr[0], &HostAddr[1], &HostAddr[2],
	  &HostAddr[3]);
  fclose (fp);
  TRACE ("Ethernet_begin");
//DHCP_reset_static(mac);
//Display_Net_Conf();
  if (Ethernet_begin (mac) == 0)
    {
      if (Ethernet_hardwareStatus () == EthernetNoHardware)
	printf ("Can't find the ethernet h/w\n");
      if (Ethernet_linkStatus () == LinkOFF)
	printf ("Plug in the cable\n");
    }
  Ethernet_localIP (gWIZNETINFO.ip);
  Ethernet_localDNS (gWIZNETINFO.dns);
//Display_Net_Conf();

  sock = socket (0, Sn_MR_TCP, SOCK_STREAM, 0);
  if (sock == -1)
    {
      printf ("ERROR:socket");
      exit (1);
    }
//  printf ("HOST:  %d.%d.%d.%d\n",
 //         HostAddr[0], HostAddr[1],
  //        HostAddr[2], HostAddr[3]);
//printf("Sock: %d\n",sock);
//printf("Port: %d\n",port);
  if (connect (sock, HostAddr, port) < 0)
    {
      printf ("ERROR:connect");
      exit (1);
    }

/* there be bugs here.  If a port is specified 
things can get sideways */

  p = argv[1];
  sprintf (dnsname, "%d.%d.%d.%d", HostAddr[0], HostAddr[1],
	   HostAddr[2], HostAddr[3]);

  xwrites ("GET /");
  if (p)
    xwrites (p);
  xwrites (" HTTP/1.1\r\n");
  xwrites ("Host: ");
  xwrites (dnsname);
  xwrites ("\r\nUser-Agent: Fuzix-htget/0.1\r\nConnection: close\r\n\r\n");
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

  remove (argv[1]);
  of = open (argv[1], O_WRONLY | O_CREAT, 0666);
  if (of == -1)
    {
      printf ("ERROR:%s\n", argv[2]);
      exit (1);
    }
  /* FIXME: if we saw a Transfer-Encoding: chunked" we need to do this
     bit differently */
  if (code == 200)
    {
      if (write (of, readp, bufend - readp) < 0)
	{
	  printf ("ERROR:write");
	  exit (1);
	}
      while ((len = recv (sock, buf, 512)) > 0)
	{
	  if (write (of, buf, len) != len)
	    {
	      printf ("ERROR:write");
	      exit (1);
	    }
	  fputc_cons ('.');
	}
    }
  fputc_cons ('\n');
  fputc_cons ('\r');
  close (of);
  sock_close (sock);
  return 0;
}
