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
char listbuf[80];
int listpos = 0;
int listcol = 0;
void listchr(char c)
{
    if (listpos == 0) {
	memset(listbuf, ' ', 79);
	listbuf[80] = 0;
    }
    if (c == '\n') {
	listcol++;
	if (listcol >= 4) {
	    listcol = 0;
	    listpos = 0;
//snapmem(listbuf,listbuf,80,0,"listbuf");
	    printf("%s\n", listbuf);
	    return;
	} else {
	    listpos = listcol * 16;
	}
    } else {
	listbuf[listpos++] = c;
    }
}

char getnetbyte()
{
    char c;
  try2:
    if (buflen == 0) {		/* get some data from network */
	buflen = recv(sock, buf, 512);
	if (buflen == 0)
	    return ('\0');
	readp = buf;
	buflen++;	/* correct byte counter */
    }

/* pick up a byte and return */

    while (c = *readp++) {
	buflen--;
	if (buflen == 0)
	    goto try2;
	switch (c) {
	case '\n':
	case '\r':
	    break;
	default:
	    return (c);
	    break;
	}
    }
    return (c);
}

void listname()
{
    char c;
    while ((c = getnetbyte()) != '"') {
	if (c != '\r')
	    listchr(c);
    }
    listchr('\n');
}


#define STATE0 '<'
#define STATE1 'a'
#define STATE2 'h'
#define STATE3 'r'
#define STATE4 'e'
#define STATE5 'f'
#define STATE6 '='
#define STATE7 7

/* useing a stream based decoder because the packet size delivered
is arbrary.  */

void htdecode()
{
    char c;
    while (c = getnetbyte()) {
	switch (c) {
	case STATE0:
	    state = STATE1;
	    break;
	case STATE1:
	    if (state == STATE1) {
		state = STATE2;
	    } else {
		state = 0;
	    }
	    break;
	case STATE2:
	    if (state == STATE2) {
		state = STATE3;
	    } else {
		state = 0;
	    }
	    break;
	case STATE3:
	    if (state == STATE3) {
		state = STATE4;
	    } else {
		state = 0;
	    }
	    break;
	case STATE4:
	    if (state == STATE4) {
		state = STATE5;
	    } else {
		state = 0;
	    }
	    break;
	case STATE5:
	    if (state == STATE5) {
		state = STATE6;
	    } else {
		state = 0;
	    }
	    break;
	case STATE6:
	    if (state == STATE6) {
		state = STATE7;
	    } else {
		state = 0;
	    }
	    break;
	case ' ':		/* skip all this */
	case '\n':
	case '\0':
	case '\t':
	case '\r':
	    break;
	default:		/* any other stuff */
	    state = 0;
	    break;
	}
	if (state == STATE7) {
	    getnetbyte();	/* '"' */
	    listname();
	    state = 0;
	}
    }
}

void writes(int fd, char *p)
{
    fd = 0;
    puts(p);
}

void xflush(void)
{
    if (send(sock, buf, buflen) != buflen) {
	printf("ERROR:send");
	exit(1);
    }
    buflen = 0;
}

void xwrites(const char *p)
{
    printf("%s", p);
    int l = strlen(p);
    if (l + buflen > 512)
	xflush();
    memcpy(buf + buflen, p, l);
    buflen += l;
}

int xread(void)
{
    int len;

    /* The first call we return the stray bytes from the line parser */
    if (bufend != buf && bufend > readp) {
	len = bufend - readp;
	memcpy(buf, readp, (int) (bufend - readp));
	bufend = buf;
	readp = buf;
	return len;
    }
    len = recv(sock, buf, 512);
    if (len < 0) {
	printf("ERROR:read %d\n", len);
	exit(1);
    }
    return len;
}

int xreadline(void)
{
    int len;

    if (readp != buf && bufend > readp) {
	memcpy(buf, readp, (int) (bufend - readp));
	bufend -= (readp - buf);
    }
    readp = buf;

    len = recv(sock, buf + buflen, 512 - buflen);
    if (len < 0) {
	printf("ERROR:socket read");
	exit(1);
    }
    buflen += len;
    bufend += len;

    while (readp < bufend) {
	if (*readp == '\r' && readp[1] == '\n') {
	    *readp++ = '\n';
	    *readp++ = 0;
	    len = readp - buf;
	    buflen -= len;
	    return len;
	}
	readp++;
    }
    writes(2, "htpget: overlong/misformatted header\n");
    exit(1);
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
void main(int argc, char *argv[])
{
    uint16_t port = 80;
    char *pp;
    int code;
    int len;
    uint8_t looped = 0;

    state = STATE0;
    skip_dns = 0;
    InetGetMac(gWIZNETINFO.mac);
    memset(dnsname, 0, 80);
    if (argc > 1) {
	strcat(dnsname, argv[1]);
// check to see if its a an ip address
	if (isdigit(dnsname[0])) {
	    while (p = strchr(dnsname, '.'))
		*p = ' ';
	    sscanf(dnsname, "%d %d %d %d",
		   &HostAddr[0], &HostAddr[1], &HostAddr[2], &HostAddr[3]);
	    skip_dns = 1;
	}
    } else
	strcat(dnsname, "server");

    printf("listing %s\n", dnsname);

    TRACE("Ethernet_begin");
    if (Ethernet_begin(mac) == 0) {
	if (Ethernet_hardwareStatus() == EthernetNoHardware)
	    printf("Can't find the ethernet h/w\n");
	if (Ethernet_linkStatus() == LinkOFF)
	    printf("Plug in the cable\n");
    }
    Ethernet_localIP(gWIZNETINFO.ip);	// get my ip address
    Ethernet_localDNS(gWIZNETINFO.dns);	// get the ip address of the dns server
    if (skip_dns == 0) {
	DNS_init(SOCK_DNS, DNS_buffer);
	if (DNS_run(gWIZNETINFO.dns, dnsname, HostAddr) == 0)	// get the host server address
	{
	    printf("%s not found\n", dnsname);
	    return;
	}
    }
    sock = socket(0, Sn_MR_TCP, SOCK_STREAM, 0);	// make a socket
    if (sock == -1) {
	printf("ERROR:socket");
	exit(1);
    }
    if (connect(sock, HostAddr, port) < 0)	// connect to socket
    {
	printf("ERROR:connect");
	exit(1);
    }

/* there be bugs here.  If a port is specified 
	things can get sideways */

    sprintf(dnsname, "%d.%d.%d.%d", HostAddr[0], HostAddr[1],
	    HostAddr[2], HostAddr[3]);

    xwrites("GET /");
    xwrites(" HTTP/1.1\r\n");
    xwrites("Host: ");
    xwrites(dnsname);
    xwrites
	("\r\nUser-Agent: cp/m htlist v0 \r\nConnection: close\r\n\r\n");
    xflush();

    do {
	xreadline();
	errno = 0;
	pp = strchr(buf, ' ');
	if (pp == NULL) {
	    writes(2, "get: invalid reply\n");
	    writes(2, buf);
	    exit(1);
	}
	pp++;
	code = strtoul(pp, &pp, 10);
	if (code < 100 || *pp++ != ' ') {
	    writes(2, "get: invalid reply\n");
	    writes(2, buf);
	    exit(1);
	}

	do {
	    xreadline();
	}
	while (*buf != '\n');

	/* A 100 code means "I'm thinking please wait then a header cycle then
	   a real header and has a blank line following */
	if (code == 100)
	    xreadline();
    }
    while (code == 100 && !looped++);

    /* FIXME: if we saw a Transfer-Encoding: chunked" we need to do this
       bit differently */
    if (code == 200) {
#ifdef NEVER
	while ((len = recv(sock, buf, 512)) > 0) {
	    htdecode(len, buf);
	}
#else
	htdecode();
#endif
    }
    printf("%s\n", listbuf);
    sock_close(sock);
}
