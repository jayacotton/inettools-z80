//******************************************************************************
//
//! \file ctcdriver.c
//! \brief CTC timer Implemtation file.
//! \details Generate timer interrupts that we can keep track of and
//! and not clobber sio interrupts.
//!	
//! CTC driver is used to generate periodic interrupts for 
//! consistent timing. 
//! 
//! See UNIX time.h for ideas about where this is going to 
//! end up.
//! 
//! 
//! This code must do several things during the init phase,
//! 
//! 	1. need to identify the operating system we are
//! 	running on.  The choises are cp/m or zdos.
//! 
//! 	2. Seting the ctc to genetate 50 hz interupts
//! 
//! 	3. we have an RTC and an ethernet board so....
//! 	this code can set the time on the rtc from 
//! 	ntp.  This will need to be a secondary init process.
//! 
//! We need to hijack the interrupt vector in order to add
//! our code to the CP/M system.  
//! 
//!
//! \version 1.0
//! \date 4/19/2019
//! \par Revision history
//!
//!	4/20/2019	Added timer function calls
//!	4/19/2019	First version
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


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "time.h"
#include "ctc.h"
#ifdef LOADER
#include "loader.h"
#include "trace.h"
#endif
unsigned char ctc_base;
unsigned char ctc_0;
unsigned char ctc_1;
unsigned char ctc_2;
unsigned char ctc_3;
uint32_t *counter;

#if TEST
uint32_t first;
main()
{
int stat;
        stat = ctc_vet(); /* turns out we don't care */
        if(stat == CPM)printf("Running on CPM\n");
        else           printf("Running on ZSDOS\n");

// this sequence is order specific.  Should wrap it
        ctc_init(); /* set up and start ctc timer */
	*counter = 0;
        time_init();    /* setup timer code */
//

// wait for ever here.
printf("Wait for interrupts\n");
	first = *counter;
	while(1){
		if((*counter - first) <= 1000)
			ctc_exit();
		if(bdos (CPM_ICON, 0))
			ctc_exit();
	}
}
#endif
#if CLOCK 
//! this is a TOD program used to test the ctc/time code. 
int stat;
int hr;
int min;
int sec;
char buffer[80];
extern void ntplib(int *, int *, int *, char *,int);
main()
{

int i;
	stat = ctc_vet(); /* turns out we don't care */
	if(stat == CPM)printf("Running on CPM\n");
	else           printf("Running on ZSDOS\n");

	ctc_init(); /* set up and start ctc timer */
	*counter = 0;
	time_init();	/* setup timer code */

	i = 0;
	hr = 12;
	min = 30;
	sec = 0;
// go get the time from ntp server
	ntplib(&hr,&min,&sec,buffer,0);
	while(1){
		time_wait(1);
		if(++sec >= 60){
			sec = 0;
			if(++min >= 60){
				ntplib(&hr,&min,&sec,buffer,1);
				min = 0;
				if(++hr >= 24){
					hr = 0;
				}
			}
		}
		printf("%02d:%02d:%02d\r",hr,min,sec);
		if(bdos (CPM_ICON, 0))
			ctc_exit();
	}
	ctc_exit();
}
#endif
/* be extra mindfull to call this exit code when
shuting down, it restores the interrupt vectors */
void ctc_exit()
{
// unchain the driver from the bdos
	
#asm
	di
	ld	a,(_ctc_0)
	ld	c,a
	ld	a,03h
	out	(c),a	; turn off interrupts
	inc	c
	out	(c),a
	inc	c
	out	(c),a
	inc	c
	out	(c),a
; unchain the driver
	ld	hl,(_chain)
	ld	b,BF_SYSINT
	ld	c,BF_SYSINT_SET
	ld	a,(_vidx)
	ld	e,a
	rst	8
	ei
	jp	0	; reload ccp
#endasm
}

/* and last, vet the os */
/* this turns out to be a bit of a challange, ... */
unsigned int GetVer()
{
#asm
	ld	c,0xc	; Get Version Number
	call	5	; hl is the version
#endasm
}
unsigned int GetDos()
{
#asm
	ld	c,48
	call	5
#endasm
}
void GetInt()
{
#asm
	ld	b,BF_SYSINT
	ld	e,0
	ld	c,BF_SYSINT_INFO
	rst	8
	ld	a,d
	and	3
	ld	(_ImMode),a
	ld	a,e
	ld	(_IntVectors),a
#endasm
}
unsigned int CheckHbios()
{
#asm
	ld	b,BF_SYSVER
	ld	c,0
	rst	8
	ex	de,hl
#endasm
}
uint8_t ImMode;
uint8_t IntVectors;

int ctc_vet()
{
unsigned int version;
unsigned int intinfo;
int ret;
	version = GetDos(); /* 5311 = zsdos v1.1 */
			    /* 0 = cp/m */
	if(version == 0x5311) ret = ZSDOS;
	else		    ret = CPM;

	version = GetVer();	
	if(version != 0x22){
		printf("Can't run, need version 2.2 \n");
		exit(1);
	}
	version = CheckHbios();
	if(version < 0x2910){
		printf("Can't run this on older RomWbW.  Need 2.9.1 or newer\n");
		exit(1);
	}
	GetInt();
	if(ImMode != 2) {
		printf("Can't run, need IM 2\n");
	}
	return (ret);
}
#ifndef LOADER
void int_hand()
{
#asm
;	di
	push	hl
	push	de
	push	bc
	push	af
	ld	hl,(_counter)
	ld	de,(_counter+2)
	inc	l
	jr	nz,lb2
	inc	h
	jr	nz,lb2
	inc	de
.lb2 	ld	(_counter),hl
	ld	(_counter+2),de
#endasm
	time_dec();
#asm
	pop	af
	pop	bc
	pop	de
	pop	hl
	ei
	reti
#endasm
}
#else
#endif
uint16_t *ctcint;
uint32_t ctx;
uint8_t vidx;
uint16_t chain;

uint16_t Get_Vector(int index)
{
	vidx = index;
#asm
	ld	a,(_vidx)
	ld	e,a
	ld	b,BF_SYSINT
	ld	c,BF_SYSINT_GET
	rst	8
#endasm
}
void Set_Vector(int index,uint16_t *serv)
{
	vidx = index;
	ctcint = serv;
#asm
	ld	hl,(_ctcint)
	ld	b,BF_SYSINT
	ld	c,BF_SYSINT_SET
	ld	a,(_vidx)
	ld	e,a
	di
	rst	8
	ld	(_chain),hl
	ei
#endasm
}
extern struct driver_head *head;
extern struct driver_file Head;
char *f = "CTCINT.HEX";
char *m = "Loadable CTC interupt handler";
void	ctc_swapaddr()
{
#ifdef LOADER
	if(!loader(f,m))
		ctc_exit();
	Set_Vector(2,GetServ());	// fix this real soon
	counter = GetCount();
	head->chain = (void *)chain;
        callback0 = (struct callback *)head->p0;
        callback1 = (struct callback *)head->p1;
        callback2 = (struct callback *)head->p2;
printf("0x%04x 0x%04x 0x%04x\n",callback0, callback1, callback2);
snapmem(1234,head,Head.size,0,"driver dump");
snapmem(1234,0,32,0,"bdos");
printf("my vector 0x%04x\n",Get_Vector(2));
#else
	Set_Vector(2,int_hand);
	counter = ctx;
#endif
}
/*
 Channel control register:
 Bit 7 = Interrupt: 1 = enable, 0 = disable
 Bit 6 = Mode: 1 = counter, 0 = timer
 Bit 5 = Prescaler (timer mode only): 1 = 256, 0 = 16
 Bit 4 = Edge selection: 1 = rising, 0 = falling
 Bit 3 = Time trigger (timer mode only): 1 = input, 0 = auto
 Bit 2 = Time constant: 1 = value follow, 0 = does not
 Bit 1 = Reset: 1 = software reset, 0 = continue
 Bit 0 = Control/vestor: 1 = control, 0 = vector
*/

//! Arm the ctc interrupts for 10ms pulse
void ctc_arm()
{
#asm
	di
	ld	a,(_ctc_2)
	ld	c,a
	ld	a,4h		; mode 2 int vector 
	out	(c),a		
; Interrupts, Precaler 256, rising edge, tc follows, control
	ld	a,181
; time constant.
	ld	b,144
	out	(c),a
	out	(c),b
	ei
#endasm
}

/* initialize the ctc chip to generate 50hz interrupts */
void ctc_init()
{
	ctc_swapaddr();	
	ctc_base = _CTC_BASE;
	ctc_0 = _CTC_0;
	ctc_1 = _CTC_1;
	ctc_2 = _CTC_2;
	ctc_3 = _CTC_3;
//	ctc_arm();
}

