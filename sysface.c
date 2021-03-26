/* this code interfaces to RomWBW and exposes the system calls 
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "sysface.h"

#ifdef FRAM
#include "fram.h"
#endif

#define bf_sysver	0xF1	// BIOS: VER function
#define bf_sysget	0xF8	// HBIOS: SYSGET function
#define bf_sysgettimer	0xD0	// TIMER subfunction
#define bf_sysgetsecs	0xD1	// SECONDS subfunction
#define EpochBuf 0		// start of epoch buffer
#define UptimeBuf EpochBuf+5	// start of uptime delta buffer.
#define TZbuf  UptimeBuf+5	// start of time zone buffer.
#define TZtxt  TZbuf+5
unsigned int count;
int lvers;
int laddr;
unsigned char lval;
unsigned long ltime;
unsigned char TOD_BUF[8];  /* bcd value of date and time */
unsigned char *lbuffer;
/*
Table of timer source names
if you add to this list be sure to adjust the code
below.
*/
char *types[5] =
{"DS1302", "BQ4845P", "SIMH", "INT TIMER", "NULL"};

/* Get time in UNIXEPOCH, its a 32 bit number good till
2038.
ntp provides seconds since midnight jan 1 1900 as a 64 bit number.
32 bits are seconds, and the other 32 are fractions of a second.
*/

/* set the time zone data (into nvram) */
void SetTZtxt(char *txt)
{
#ifdef FRAM
	FramSetTZText(txt);
#else
int i;
int addr;
	for(i=0;i< 4;i++){
		addr = TZtxt+(i<<1);
		SetNvram(addr,txt[i]);	
	}
#endif
}
char TZName[6];

char *GetTZtxt()
{
#ifdef FRAM
	memset(TZName,0,6);
	FramGetTZText((unsigned char*)TZName);
	return &TZName;
#else
int i;
int addr;
	memset(TZName,0,6);
	for(i=0;i<4;i++){
		addr = TZtxt+((i)<<1);
		TZName[i] = GetNvram(addr);	
	}
	return &TZName;
#endif
}
void SetTZ(long zone)
{
#ifdef FRAM
	FramSetTZ(zone);
#else
int i;
long lzone;
int addr;
	lzone = zone;
	for(i=0;i<4;i++){
		addr = TZbuf + (i<<1);
		SetNvram(addr,(unsigned char )lzone & 0xff);
		lzone = lzone >> 8;
	}
#endif
}
long GetTZ()
{
#ifdef FRAM
	return(FramGetTZ());
#else
int i;
long res;
int addr;
	res = 0;
	for(i=0;i<4;i++){
		res = res << 8;
		addr = TZbuf + ((3-i)<<1);
		res |= 0xff & GetNvram(addr);
	}
	return res;
#endif
}
/* get and test the bios version number */
int TestBIOS()
{
#asm
	ld	bc,$f100
	rst	08
	ld	(_lvers),de
#endasm
	return ((lvers >= 0x3000)? 1:0);
}
/* get and set the nvram storage used to keep the 
current value of EPOCH from ntp time server.
*/
void EpochSet(unsigned long epoch)
{
unsigned long lepoch;
#ifdef FRAM
	FramSetEpoch(epoch);
#else
int i;
int addr;
	lepoch = epoch;
#ifdef DEBUG
printf("%lu\n",lepoch);
#endif
/* store the current epoch time value */
	for(i=0;i<4;i++){
		addr = EpochBuf + ((i)<<1);
		SetNvram(addr,(unsigned char )lepoch & 0xff);
		lepoch = lepoch >> 8;
	}
/* save the current uptime in seconds */ 
#endif
	lepoch = GetUptime(1);
	SetDeltaUptime(lepoch);
}

/* get the UNIXEPOCH time in seconds since the last NTP time
check.

UNIXEPOCH + Delta uptime.
	Delta uptime is current uptime + (delta at ntp check)
	delta at ntp check is uptime at ntp (stored in nvram) - current uptime.
*/
long EpochGet()
{
int i;
long res;
int addr;
	res = 0;
#ifdef FRAM
	res = FramGetEpoch();	
#else
	for(i=0;i<4;i++){
		res = res << 8;
		addr = EpochBuf + ((3-i)<<1);
		res |= 0xff & GetNvram(addr);
	}
#endif
#ifdef DEBUG
printf("%lu\n",res);
#endif
/* Epoch time is UNIXEPOC + Delta seconds - timezone */
	res += GetDeltaUptime();
	res += GetTZ();
	return res;
}
/* get and set the uptime seconds

Don't forget the collect the delta uptime from the
nvram to add into the current uptime value....  
assumes someone set uptime from ntp

This code is really aimed at ds1302 only.
*/
void SetNvram(int addr,unsigned char val)
{
	laddr = addr;
	lval	= val;
#ifdef DEBUG
printf("Set addr %x to %x\n",laddr,lval);
#endif
#asm	
	ld	b,$23
	ld	a,(_laddr)
	ld	c,a
	ld	a,(_lval)
	ld	e,a
	rst	08
#endasm
}
unsigned char GetNvram(int addr)
{
	laddr	= addr;
#asm
	ld	b,$22
	ld	a,(_laddr)
	ld	c,a
	rst	08
	ld	a,e
	ld	(_lval),a
#endasm
#ifdef DEBUG
printf("Get addr %x to %x\n",laddr,lval);
#endif
	return (lval);
}

void SetDeltaUptime(unsigned long uptime)
{
#ifdef FRAM
	FramSetDeltaUptime(uptime);
#else
int i;
int addr;
	for(i=0;i<4;i++){
		addr = UptimeBuf + (i<<1);
		SetNvram(addr,(unsigned char )uptime & 0xff);
		uptime = uptime >> 8;
	}
#endif
}

/* return a copy of the uptime that was stored in the nvram */
unsigned long GetDeltaUptime()
{
int i;
unsigned long res;
int addr;
	res = 0;
#ifdef FRAM
	res = FramGetDeltaUptime();
#else
	for(i=0;i<4;i++){
		res = res << 8;
		addr = UptimeBuf + ((3-i)<<1);
		res |= (unsigned char )GetNvram(addr);
	}
#endif
	res = GetUptime(1) - res;
	return (res);
}
unsigned long GetUptime(int flag)
{
#asm
	ld	bc,$f8d1
	rst	08
	ld	(_ltime+2),de
	ld	(_ltime),hl
#endasm
	if(flag)
		return (ltime);
/* secs since last ntp */
	return (ltime - GetDeltaUptime());
}
/* when setting the uptime value, save the 
delta time in the nvram so that uptime can
be computed accuratly */

void SetUptime(unsigned long time)
{
	ltime = time;
#asm
	ld	bc,$f9d1
	ld	de,(_ltime+2)
	ld	hl,(_ltime)
	rst	08
#endasm
}
/* get and set the time of day
*/
void SetTOD(unsigned char *buffer)
{
	lbuffer = buffer;
#asm
	ld	b,$21
	ld	hl,(_lbuffer)
	rst	08
#endasm
}
unsigned char *GetTOD()
{
#asm
	ld	b,$20
	ld	hl,_TOD_BUF
	rst	08
#endasm
	return TOD_BUF;
}

/*
Return the count of the number of RTC units in the system.  
This could be as high as 2 but most likely 1 or 0 
*/

unsigned int RTCCount()
{
#asm
				; f8 is sysget and 20 is subfunction
				; get rtc count 
	ld	bc,$f820	; get rtc count 0 = none
	rst	08		; go get it.
	cp	0		; is there an error
	jp	z,_Cnt		; zero is error
	jp	_rtcDone	; bail out.	
._Cnt	ld	a,e		; save the count value
._rtcDone ld	(_count),a	; in count.

#endasm
	return (count);
}

/*
Return a pointer to the name of the timer source
*/
unsigned char *RTCType()
{
	if(!TestBIOS()){
		printf("Must be RomWBW Version 3.0 or newer\n");
		exit(0);
	}
	if(!RTCCount()){		/* be padantic */
		return (types[4]);
	}
#asm
				; index of type is zero.... (bug source)
	ld	bc,$2800	; get rtc device type
	rst	08		; go do it.
	ld	a,d		; make a copy
	ld	(_count),a	; go count (overload, bug source)
#endasm
	return (types[count>>4]	); /* return a string pointer. */
}
