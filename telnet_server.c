/* telnet deamon for cp/m systems */

#include <stdio.h>
#include <stdlib.h>
#include "ethernet.h"
#include "wizchip_conf.h"
#include "trace.h"
#include "socket.h"
#include "telnetd.h"
#include <malloc.h>

/* since cp/m does not have login/password protocol and
there are no user accounts (as in modern os's) the setup
and binding should be very simple.  i.e. start the daemon
and connect via telnet */

wiz_NetInfo gWIZNETINFO;
unsigned char run_user_applications;

unsigned char mac[6] = { 0x98, 0x76, 0xb6, 0x11, 0x00, 0xc4 };
unsigned char ip[4];

int Socket;
int Port;

int j;
int len;
unsigned char byte;
unsigned char cmdbuf[12];
unsigned int do_loop;

/* find and store the stuff that is located in the
BIOS jump table for the console io functions. */

unsigned int konst;
unsigned int xst1;
unsigned int conin;
unsigned int xst2;
unsigned int conout;
unsigned int xst3;
unsigned int rootof;
unsigned char x_x;
extern void CONOUT(unsigned char x);

void byte_con(unsigned char xbyte)
{
	/* get status from wr0 */
	do{
		outp(0x88,0);
		j = inp(0x88);
		j &= 4;
	}while(j != 4); /* if zero  loop */
	/* if not zero write byte */
	outp(0x89,xbyte);
}
void text_con(unsigned char *text)
{
	while(*text != '\0'){
		//byte_con(*text++);
		x_x = *text++;
#asm
		ld	a,(_x_x)
		ld	c,a
#endasm
		CONOUT(x_x);
	}
#asm
		ld	c,'\n'
#endasm
		CONOUT('\n');
		CONOUT('\r');
}
void bahr()
{
#asm
	di	
	ld	hl,(_konst)
	ld	de,(_xst1)
	ld	(hl),e
	inc	hl
	ld	(hl),d
	ld	hl,(_conin)
	ld	de,(_xst2)
	ld	(hl),e
	inc	hl
	ld	(hl),d
	ld	hl,(_conout)
	ld	de,(_xst3)
	ld	(hl),e
	inc	hl
	ld	(hl),d
	ei
#endasm
	exit(0);
}

extern unsigned char CONST();
extern unsigned char CONIN();
extern void CONOUT(unsigned char);
 
void CONINIT()
{
#asm
	ld	hl,(1)		; pick up the bios jump table
	ld	(_rootof),hl
	ld	de,4		; up 4 bytes
	add	hl,de
	ld	de,hl		; copy to addr to de
	ld	a,(hl)		; get low byte of const addr
	inc	hl
	ld	h,(hl)		; get high byte of const addr
	ld	l,a
	ld	(_konst),hl	; store const addr 
	ld	hl,de		; get base to hl
	ld	(_xst1),hl
	ld	de,3		; up 3 more bytes
	add	hl,de
	ld	de,hl		; save the base
	ld	a,(hl)		; get low byte of conin
	inc	hl
	ld	h,(hl)		; get hi byte of conin
	ld	l,a
	ld	(_conin),hl	; store conin addr
	ld	hl,de		; get base to hl
	ld	(_xst2),hl
	ld	de,3		; up 3 more bytes
	add	hl,de	
	ld	(_xst3),hl
	ld	a,(hl)		; get low byte of conout
	inc	hl
	ld	h,(hl)		; get high byte of counout
	ld	l,a
	ld	(_conout),hl	; save conout addr
#endasm
/* now that we have copied the pointers we can get the
contest of memory from those locations and save it for
returning later */
#asm
	di
	ld	de,_CONST
	ld	hl,(_xst1)
	ld	(hl),e
	inc	hl
	ld	(hl),d
	ld	de,_CONIN
	ld	hl,(_xst2)
	ld	(hl),e
	inc	hl
	ld	(hl),d
	ld	de,_CONOUT
	ld	hl,(_xst3)
	ld	(hl),e
	inc	hl
	ld	(hl),d
	ei	
#endasm
}
int z;
unsigned char CONST()
{
#asm
	di
	push	ix
	push	iy
	push	hl
	push	de
	push	bc
#endasm
//LED(0x80);
	do_loop = 0;
	if(getSn_RX_RSR (Socket)){
#asm
	pop	bc
	pop	de
	pop	hl
	pop	iy
	pop	ix
	ld	a,255	; ff means there is data
	ei
	ret
#endasm
	} else {
#asm
	pop	bc
	pop	de
	pop	hl
	pop	iy
	pop	ix
	ld	a,0	; zero means no data
	cp	a,0	; force z flag
	ei
	ret
#endasm
	}
return z;
}
unsigned char CONIN()
{
#asm
	di
	push	hl
	push	de
	push	bc
	push	ix
	push	iy
#endasm
//LED(0x40);
	do_loop = 0;
	do{
		z = recv (Socket, &x_x, 1);
	}while(z==0);
#asm
	pop	iy
	pop	ix
	pop	bc
	pop	de
	pop	hl
	ld	a,(_x_x)
	and	a,7fh 
	ei
#endasm
	return (unsigned char )z;
}
void CONOUT(unsigned char x)
{
#asm
	di
	push	ix
	push	iy
	push	hl
	push	de
	push	bc
	ld	a,c
	ld	(_x_x),a
#endasm
//LED(0x20);
	do_loop = 0;
        send (Socket, &x_x, 1);
#asm
	pop	bc
	pop	de
	pop	hl
	pop	iy
	pop	ix
	ei
#endasm
}
main ()
{
/* setup conole connect.  remember we want to fail back to
sio console */
/* consider using a different console device and then just 
assign the console to that device. */


/* set up ethernet device */
/* remember default socket is 23 */
/* start by getting the ip address we got from dhcp server */
/* print the address to we know what the ip and port will be */
  if (Ethernet_begin (mac) == 0)
    {
      TRACE ("error checking ");
      if (Ethernet_hardwareStatus () == EthernetNoHardware)
	printf ("Can't find the ethernet h/w\n");
      if (Ethernet_linkStatus () == LinkOFF)
	printf ("Plug in the cable\n");
      exit (0);
    }
  Ethernet_localIP (ip);
  printf ("inet %d.%d.%d.%d ", ip[0], ip[1], ip[2], ip[3]);
  printf ("23\n");

  CONINIT();
  Port = 23;			/* make it hard coded for now */
  Socket = 2;			/* hard code this also */
/* make a socket */
  socket (Socket, Sn_MR_TCP, Port, 0);
/* get a listen set up */
  while (listen (Socket) != SOCK_OK);
/* wait here for a telnet session to start */
  while (getSn_RX_RSR (Socket) == 0)
    {
    }
  send (Socket, "CP/M 2.2 Telnet Deamon\n\r", 24);
/* loop */

text_con("testing the CONOUT code");

SNAP(0xe603,32,0);	
  do_loop = 1;
  while (do_loop)
    {				/* should be looking for an exit condition */
/* commands that the telnet client sends 
DO SUPPRESS GO AHEAD, WILL TERMINAL TYPE, WILL NAWS, 
WILL TSPEED, WILL LFLOW, WILL LINEMODE, WILL NEW-ENVIRON, 
DO STATUS, WILL XDISPLOC 
others will just be dumped for now */
      len = getSn_RX_RSR (Socket);
      if (len)
	{
	  recv (Socket, &byte, 1);
	  switch (byte)
	    {
	    case 255:
	      {
		recv (Socket, &byte, 1);
		switch (byte)
		  {
		  case '\r':
                  {
			do_loop = 0;
			break;
		  }
		  case DO:
			recv (Socket, &byte, 1);
			cmdbuf[0] = IAC;
			cmdbuf[1] = WONT;
			cmdbuf[2] = byte;
			send(Socket,cmdbuf,3);
			break;			
		  case WILL:
			recv (Socket, &byte, 1);
			cmdbuf[0] = IAC;
			cmdbuf[1] = DO;
			cmdbuf[2] = byte;
			send(Socket,cmdbuf,3);
			break;			
		  case WONT:
			recv (Socket, &byte, 1);
			break;
	          case DONT:
			recv (Socket, &byte, 1);
			break;
	          default:	/* all the stuff we are dumping goes here */
		}
	      }
		break;
	    case '\r':
			do_loop = 0;
			break;
	    default:
	      break;
	    }
/* monitor the console port and send data to the ethernet */
/* end of loop */
	}
    }
return 0;
}

