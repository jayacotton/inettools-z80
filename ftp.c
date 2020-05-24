/* 
Design and codeing by Jay Cotton 4/8/2019

<MIT license>

FTP for cpm using the RFC959 as the design document.

The objective is to get the smallest ftp client possible without giving up 
any of the things that we really need to process a complete data transfer.
A comment about z80/cpm and z88dk.  Parameter passing is slow, and local
storage uses a lot of space.  Much faster to use global storage as much
as possible and therby speed things up.

An important point.  FTP uses 2 tcp connections, one for commands and
the other for data.  With this methode, its possible to transfer data
and talk to the server at the same time.  (not real likely with cpm
but we will give it a try).

Primary objectivs:

	get a connection to the ftp server.
	set the directory level at the server
		client directories don't exist, but we
		have drive letters.
	list files at the server
		perhapes list file on the client end also.
	transfer a file from the server.
		ascii and binary file types.	
	gracefully exit and shutdown.


The FTP client is devided into 3 basic parts.

	1.  user interface
		This module also connects to the console 
		device.
	2.  User PI  (ftp command interpreter)
		The interpreter will have as few commands as 
		possible to deal with.
	3.  User DPT (Data comunication with the server)
		This code interfaces with the local file
		system.  In this case CP/M.  To make files
		and transfer data into them.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <net/hton.h>
#include "wizchip_conf.h"
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
#define BUFLEN 2048
#define FTP_CMD_PORT 21
#define FTP_DATA_PORT 20

#define pack2(var, off) \
        (((var[(off) + 0] & 0xff) << 8) | ((var[(off) + 1] & 0xff) << 0))
typedef enum
{
  ABOR,				/* abort */
  ACCT,				/* account */
  ALLO,				/* allocate */
  APPE,				/* append */
  CDUP,				/* change to parent directory */
  CWD,				/* change working directory */
  DELE,				/* delete */
  HELP,				/* help */
  LIST,				/* list (files) */
  MKD,				/* make directory */
  MODE,				/* transfer mode */
  NLST,				/* name list */
  NOOP,				/* do nothing */
  PASS,				/* password */
  PASV,				/* passive */
  PORT,				/* data port */
  PWD,				/* print working directory */
  QUIT,				/* logout */
  REIN,				/* reinitialize */
  REST,				/* restart */
  RETR,				/* retrieve */
  RMD,				/* remove directory */
  RNFR,				/* rename from */
  RNTO,				/* rename to */
  SITE,				/* site parameters */
  SMNT,				/* structure mount */
  STAT,				/* status */
  STOR,				/* store */
  STOU,				/* store unique */
  STRU,				/* file structure */
  SYST,				/* system */
  TYPE,				/* representation type */
  USER				/* user name */
} cmd_nr;

unsigned char tmp2[10] = { 255, 250, 31, 0, 80, 0, 24, 255, 240 };
unsigned char tmp1[10] = { 255, 251, 31 };

#define FILEOUT 1
#define CONSOLE 2

int direct;
int run_user_applications;
struct wiz_NetInfo_t gWIZNETINFO;
unsigned char mac[6] = { 0x98, 0x76, 0xb6, 0x11, 0x00, 0xc4 };

FILE *out_file;

unsigned char buf[BUFLEN + 1];

unsigned char dnsname[80];
extern unsigned char HostAddr[4];
int sock;
int port;
int data_port;
int data_sock;

unsigned char DNS_buffer[256];	/* smaller ??? */
int skip_dns;

extern int UI ();
extern int Getindex (char *);
extern int PI (char);
extern int DTP ();

void
main (int argc, char **argv)
{
  int rv;
  int i;
  unsigned char *p;

  /* get the ipaddres of the name of the server */
  /* get my dhcp address and the dns server */
  skip_dns = 0;
  direct = CONSOLE;

  if (argc < 2 || argc > 4)
    {
      printf ("Usage: %s address [port]\n", argv[0]);
      return 1;
    }
  port = FTP_CMD_PORT;
  data_port = 0;
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
	    if (p = (unsigned char *) strchr (dnsname, '.'))
	      *p = ' ';
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
      exit (0);
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

  /* establish a connection to the ftp server */
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

/* punt to the UI for all the good stuff */

  UI ();
  sock_close (sock);
}

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

int
UI ()
{
  int nready;
  int rv;
  int len;
  unsigned char *p;
  int cmdnr;
  char cmdbuf[80];
  int i, k;

  k = 0;

  /* establish a telnet connection to the server
     for command and control */
  while (1)
    {
/* check for pending server commands text */
      nready = getSn_RX_RSR (sock);
      if (nready < 0)
	{
	  return 1;
	}
      else if (nready > 0)
	{
	  // start by reading a single byte
	  if ((rv = recv (sock, buf, 1)) < 0)
	    return 1;
	  else if (rv == 0)
	    return 0;

	  if (buf[0] == CMD)
	    {
	      // read 2 more bytes
	      len = recv (sock, buf + 1, 2);
	      if (len < 0)
		return 1;
	      else if (len == 0)
		return 0;
	      negotiate (sock, buf, 3);
	    }
	  else if (isdigit (buf[0]))
	    {
	      PI (buf[0]);
	    }
	  else
	    {
	      fputc_cons (*buf);
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
/* check the data socket for */
      if (data_sock)
	{
	  nready = getSn_RX_RSR (data_sock);
	  if (nready)
	    DTP ();
	}
/* check for pending keyboard characters */
      if (bdos (CPM_ICON, 0))
	{
/* we will have to do a blocking read of the
console in order to get the command text for
the lookup
NOTE:  in all cases we need to k=0 memset at the end of
a line of text. */
	  buf[0] = fgetc_cons ();
	  cmdbuf[k++] = buf[0];
	  if (buf[0] == '\n')
	    {
	      cmdnr = Getindex (cmdbuf);
	      switch (cmdnr)
		{
		case LIST:
		  direct = CONSOLE;
		  break;
		case PASV:
		  k = 0;
		  memset (cmdbuf, 0, 80);
		  break;
		case RETR:
		  direct = FILEOUT;
	printf("file name %s",&cmdbuf[5]);
		  out_file = fopen (&cmdbuf[5], "w");
		  k = 0;
		  memset (cmdbuf, 0, 80);
		  break;
		case MODE:
		  k = 0;
		  memset (cmdbuf, 0, 80);
		  break;
		default:
		  k = 0;
		  memset (cmdbuf, 0, 80);
		  direct = CONSOLE;
		}

	    }
	  fputc_cons (buf[0]);
	  if (send (sock, buf, 1) < 0)
	    return 1;
	  if (buf[0] == '\n')
	    fputc_cons ('\r');
	}
    }

  /* I think we will loop here collecting command
     strings and send them down to the PI for handling */

}

/* all the commands that are listed in the rfc.  Most of this
will be ignored.  Only the most esential commands will be implemented */

/* so, we will look for a command word in this list and return the
index of the command.  That will decode the command to an int.  SO
DO NOT REORDER THE TABLE */

char *cmds[33] = {
  "ABOR",			/* abort */
  "ACCT",			/* account */
  "ALLO",			/* allocate */
  "APPE",			/* append */
  "CDUP",			/* change to parent directory */
  "CWD",			/* change working directory */
  "DELE",			/* delete */
  "HELP",			/* help */
  "LIST",			/* list (files) */
  "MKD",			/* make directory */
  "MODE",			/* transfer mode */
  "NLST",			/* name list */
  "NOOP",			/* do nothing */
  "PASS",			/* password */
  "PASV",			/* passive */
  "PORT",			/* data port */
  "PWD",			/* print working directory */
  "QUIT",			/* logout */
  "REIN",			/* reinitialize */
  "REST",			/* restart */
  "RETR",			/* retrieve */
  "RMD",			/* remove directory */
  "RNFR",			/* rename from */
  "RNTO",			/* rename to */
  "SITE",			/* site parameters */
  "SMNT",			/* structure mount */
  "STAT",			/* status */
  "STOR",			/* store */
  "STOU",			/* store unique */
  "STRU",			/* file structure */
  "SYST",			/* system */
  "TYPE",			/* representation type */
  "USER"			/* user name */
};

/* find a command the the cmds table and return the
index number of the table.  At least this ugly
POS works. */

int
Getindex (char *cmd)
{
  int i;
  char lcmd[5];
  memset (lcmd, 0, 5);

  for (i = 0; i < 4; i++)
    {
      if (*cmd != ' ')
	lcmd[i] = *cmd++;
    }
  for (i = 0; i < 33; i++)
    {
      if (stricmp (cmds[i], lcmd) == 0)
	{
	  return i;
	}
    }
  return -1;
}

  /* set up the command process interpreter */
int
PI (char s)
{
  int icmd;
  char cmd_text[80];
  int len;
  int i;
  int rv;
  char *p;
  unsigned char addr[6];
  unsigned char port[2];
  char *parslist[6];

  len = recv (sock, buf + 1, 2);
  buf[3] = '\0';
  icmd = atoi (buf);

  switch (icmd)
    {
    case 125:
      /* data transfer starting */
      break;
    case 150:
      /* file status ok. about to open data connection */
      break;
    case 211:
      break;
    case 214:
      len = recv (sock, buf, BUFLEN);
      i = 0;
      while (len--)
	fputc_cons (buf[i++]);
      break;
    case 220:
      send (sock, "220  (CP/M FTP 1.0)", 18);
      break;
    case 226:
      /* closeing data connection, requested action complete */
/* lets make sure the buffers are empty befor we close the
connection */
      if (getSn_RX_RSR (data_sock))
	DTP ();
      if (direct == FILEOUT)
	{
	  direct = CONSOLE;
	  fclose (out_file);
	}
    case 250:
      /* frequested file action o.k. done */
      sock_close (data_sock);
      data_port = 0;
      break;
    case 226:
      /* directory list done */
      break;
    case 227:
      len = recv (sock, buf, BUFLEN);
// buf has a message like this :
//Entering Passive Mode (192,168,1,112,89,109).
      parslist[0] = p = strchr (buf, '(');
      parslist[0]++;
      p++;
      for (i = 0; i < 5; i++)
	{
	  p = strchr (p, ',');
	  *p = '\0';
	  parslist[i + 1] = ++p;
	}
      p = strchr (++p, ')');
      *p = '\0';
      for (i = 0; i < 6; i++)
	{
	  addr[i] = atoi (parslist[i]);
	}
      port[0] = addr[4];
      port[1] = addr[5];
      data_port = (pack2 (port, 0));
      data_sock = socket (1, Sn_MR_TCP, SOCK_STREAM, 0);
      if (data_sock == -1)
	{
	  printf (" Could not create data socket.Error %d ", data_sock);
	  return 1;
	}

      rv = connect (data_sock, addr, data_port);
      if (rv < 0)
	{
	  printf (" Data port connection fail. port %u %d \n", data_port, rv);
	  return 1;
	  data_port = 0;
	}
      break;
    case 421:
      printf (" Server is off line now \n");
      exit (0);
      break;
    case 230:
      printf (" User is logged in \n");
      break;
    case 530:
      printf (" User is not logged in \n");
      break;
    case 500:
    case 501:
    case 502:
    case 503:
    case 504:
      //printf (" Command not recognized \n");
      break;
    case 524:
      /* data transmission has begun */
      break;

    }
}

  /* create a data connection to the FTP server */
int
DTP ()
{
  int len;
  int i;
  len = getSn_RX_RSR (data_sock);

  len = recv (data_sock, buf, len);
  if (direct == CONSOLE)
    {
      i = 0;
      while (len--)
	fputc_cons (buf[i++]);
    }
  else if (direct == FILEOUT)
    {
      fwrite (buf, 1, len, out_file);
    }
}
