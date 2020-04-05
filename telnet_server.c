/* telnet deamon for cp/m systems */

#include <stdio.h>
#include <stdlib.h>
#include "ethernet.h"
#include "wizchip_conf.h"
#include "trace.h"
#include "socket.h"

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
//LED(j);
	}while(j != 4); /* if zero  loop */
//LED(xbyte);
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
//	byte_con('\n');
#asm
		ld	c,'\n'
#endasm
		CONOUT('\n');
//	byte_con('\r'); 
		CONOUT('\r');
}
void bahr()
{
//LED(255);
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
//LED(254);
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
//LED(7);
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
//LED(0);
}
unsigned char CONST()
{
#asm
	push	hl
	push	de
	push	bc
#endasm
text_con("In CONST");
//LED(64);
	if(getSn_RX_RSR (Socket)){
#asm
	pop	bc
	pop	de
	pop	hl
	ld	a,255	; ff means there is data
	ret
#endasm
	} else {
#asm
	pop	bc
	pop	de
	pop	hl
	ld	a,0	; zero means no data
	cp	a,0	; force z flag
	ret
#endasm
	}
}
unsigned char CONIN()
{
#asm
	push	hl
	push	de
	push	bc
#endasm
text_con("In CONIN");
//LED(32);
	while(!recv (Socket, &x_x, 1));
LED(x_x);
#asm
	pop	bc
	pop	de
	pop	hl
	ld	a,(_x_x)
	and	a,7fh 
	ret
#endasm
}
void CONOUT(unsigned char x)
{
#asm
	push	hl
	push	de
	push	bc
	ld	a,c
	ld	(_x_x),a
#endasm
//LED(x_x);
  send (Socket, &x_x, 1);
#asm
	pop	bc
	pop	de
	pop	hl
#endasm
}
main ()
{
unsigned char dummy[30];
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
//  printf ("inet %d.%d.%d.%d ", ip[0], ip[1], ip[2], ip[3]);
//  printf ("23\n");
	CONINIT();
  Port = 23;			/* make it hard coded for now */
  Socket = 2;			/* hard code this also */
  socket (Socket, Sn_MR_TCP, Port, 0);
  while (listen (Socket) != SOCK_OK);
//LED(14);
/* wait here for a telnet session to start */
  while (getSn_RX_RSR (Socket) == 0)
    {
    }
recv(Socket,dummy,27);
//LED(5);
  send (Socket, "CP/M 2.2 Telnet Deamon\n\r", 24);
/* loop */
text_con("testing the CONOUT code");
#ifdef NEVER
exit(0);
j = 0;
  while (1)
    {				/* should be looking for an exit condition */
//LED(6);
//LED(j++);
if(j >= 64000) exit(0);
/* monitor the ethernet for incoming data, and send it
t
      len = getSn_RX_RSR (Socket);
      if (len)
	{
	  recv (Socket, byte, 1);
	  switch (byte[0])
	    {
	    case 255:
	      {
		recv (Socket, &byte, 1);
		switch (byte)
		  {
		  case '\r':{
//LED(7);
exit(0);
		  }
			break;
		  case 240:
		  case 241:
		  case 242:
		  case 243:
		  case 244:
		  case 245:
		  case 246:
		  case 247:
		  case 248:
		  case 249:
		  case 250:
		  case 251:
		  case 252:
		  case 253:
		  case 254:
		   recv (Socket, &byte, 1);
		    break;
		  }
		   break;
	      }
		break:
	    case '\r':
//LED(7);
exit(0);
			break;
		default:
	      break;
	    default:
		break;
	    }
/* monitor the console port and send data to the ethernet */
/* end of loop */
	}
#endif
}
