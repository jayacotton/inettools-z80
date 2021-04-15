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

/* decode and print the desired text from the server.

records like this are imbedded in a lot of text noise.  So the scanner needs to be clever
<td><a href="DIG.COM">DIG.COM</a></td><td align="right">2021-04-11 20:29  </td><td align="right"> 31K</td>
What we want to get is.  This we can printf....
DIG.COM/t2021-04-11 20:29/t31K/n 

Because we can get partial records, we need to make a character based statemachine to decode
the input strings.  

One state machine ring to rule them all
<td><a href="A.COM">A.COM</a></td>
<td><a href="CUBE.COM">CUBE.COM</a></td>
<td><a href="DATE.COM">DATE.COM</a></td>
<td><a href="DIG.COM">DIG.COM</a></td>
<td><a href="DIGIT.COM">DIGIT.COM</a></td>

*/
int state = 0;			//init state machine to 0
#define START 0
#define PRALUDE 12
void
htdecode (int len, unsigned char *p)
{
  unsigned char *cc;
  cc = p;
  while (len--)
    {
      switch (*cc++)
	{
	case '<':
	  if (state == START)
	    state++;
	  else if (state == 4)
	    state++;
	  break;
	case 't':
	  if (state == 1)
	    state++;
	  break;
	case 'd':
	  if (state == 2)
	    state++;
	  break;
	case '>':
	  if (state == 3)
	    state++;
	  break;
	case 'a':
	  if (state == 5)
	    state++;
	  break;
	case ' ':
	  if (state == 6)
	    state++;
	  break;
	case 'h':
	  if (state == 7)
	    state++;
	  break;
	case 'r':
	  if (state == 8)
	    state++;
	  break;
	case 'e':
	  if (state == 9)
	    state++;
	  break;
	case 'f':
	  if (state == 10)
	    state++;
	  break;
	case '=':
	  if (state == 11)
	    state++;
	  goto check;
	  break;
	default:
	  state = 0;
	  break;
	}
    check:
      if (state == PRALUDE)
	{
	  cc++;
	  while (len)
	    {
	      if (*cc == '"')
		{
		  cc++;
		  state = START;
		  printf ("\n");
		  goto check;
		}
	      else
		{
		  putchar (*cc++);
		  len--;
		  if (len == 0)
		    return;
		}
	    }
	}
    }
}

void
writes (int fd, char *p)
{
  fd = 0;
  puts (p);
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
char *p;
unsigned char run_user_applications;
void
main (int argc, char *argv[])
{
  uint16_t port = 80;
  char *pp;
  int code;
  int len;
  uint8_t looped = 0;

  skip_dns = 0;
  InetGetMac (gWIZNETINFO.mac);
  memset (dnsname, 0, 80);
  if (argc > 1)
    {
      strcat (dnsname, argv[2]);
// check to see if its a an ip address
      if (isdigit (dnsname[0]))
	{
	  while (p = strchr (dnsname, '.'))
	    *p = ' ';
	  sscanf (dnsname, "%d %d %d %d",
		  &HostAddr[0], &HostAddr[1], &HostAddr[2], &HostAddr[3]);
	  skip_dns = 1;
	}
    }else
		strcat(dnsname,"server");

printf("listing %s\n",dnsname);

  TRACE ("Ethernet_begin");
  if (Ethernet_begin (mac) == 0)
    {
      if (Ethernet_hardwareStatus () == EthernetNoHardware)
	printf ("Can't find the ethernet h/w\n");
      if (Ethernet_linkStatus () == LinkOFF)
	printf ("Plug in the cable\n");
    }
  Ethernet_localIP (gWIZNETINFO.ip);	// get my ip address
  Ethernet_localDNS (gWIZNETINFO.dns);	// get the ip address of the dns server
  if (skip_dns == 0)
    {
      DNS_init (SOCK_DNS, DNS_buffer);
      if(DNS_run (gWIZNETINFO.dns, dnsname, HostAddr)==0)	// get the host server address
	{
		printf("%s not found\n",dnsname);
		return;
	}
    }
  sock = socket (0, Sn_MR_TCP, SOCK_STREAM, 0);		// make a socket
  if (sock == -1)
    {
      printf ("ERROR:socket");
      exit (1);
    }
  if (connect (sock, HostAddr, port) < 0)		// connect to socket
    {
      printf ("ERROR:connect");
      exit (1);
    }

/* there be bugs here.  If a port is specified 
	things can get sideways */

  sprintf (dnsname, "%d.%d.%d.%d", HostAddr[0], HostAddr[1],
	   HostAddr[2], HostAddr[3]);

  xwrites ("GET /index.html");
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
	  htdecode (len, buf);
	}
    }
  putchar ('\n');
  putchar ('\r');
  sock_close (sock);
}
