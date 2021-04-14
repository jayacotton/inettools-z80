//******************************************************************************
//
//! \file spi.c
//! \brief SPI bus APIs Implemtation file.
//! \details API is based on the SPI spec as found at wikipedia
//!	found at https://en.wikipedia.org/wiki/Serial_Peripheral_Interface
//!
//! \version 2.0
//! \date 4/13/2019
//! \par Revision history
//! 	7/5/2018  First version published at sourceforge.com
//!  		https://sourceforge.net/projects/spi-bus-z80-rc2014/
//!
//!	4/13/2019 Updated code to include the following new features
//!		1.  Parameterized the assembly code to allow c programmers
//!			to set the i/o device address for the spi bus card.
//!		2.  Added suport code for H/W shift register SPI bus card
//!			version 2.0.
//!		3.  Added autodetect code to allow automatic support of 
//!			the bit ganger spi card, and the h/w shift register
//!			SPI card.
//!		4.  Added support for the W5500 network controller.
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

#include "spi.h"
#include "trace.h"

unsigned char spi_data_bit;
unsigned char spi_channel;
unsigned char _spi_sel;
unsigned char _spi_data;

int spi_unit;

unsigned char shift_wrtr_;
unsigned char shift_rdtr_;
unsigned char shift_rdntr_;
unsigned char shift_ctrl_wr_;
unsigned char shift_status_;

#ifdef __SDCC
#define ASM __asm
#else
#define ASM #asm
#endif

#ifdef __SDCC
#define ENDASM __endasm
#else
#define ENDASM #endasm
#endif

/* probe code is not working */

//! spi_probe is called by spi_init.  It figures out what the
//! bus h/w is and configures for that h/w.
int
spi_probe ()
{
TRACE("");
    return SHIFTOUT;
#ifdef NEVER
return BITBANGOR;
  if (inp (spi_base) == 204)
    return BITBANGOR;
  if (inp (shift_base) == 63)
    return SHIFTOUT;
  return 0;
#endif
}

//! spi_init, is called by the user level code.  If configures the
//! i/o port addresses for the h/w based on the results of spi_probe
//! initalizes the bus as needed.

void
spi_init ()
{

TRACE("");
  spi_unit = spi_probe ();
  if (spi_unit == BITBANGOR){
//	_spi_sel = spi_sel;
//	_spi_data = spi_data;	// clock and data same port
  }
  else if (spi_unit == SHIFTOUT){
	shift_wrtr_ 	= _shift_wrtr;
	shift_rdtr_ 	= _shift_rdtr;
	shift_rdntr_ 	= _shift_rdntr;
	shift_ctrl_wr_ 	= _shift_ctrl_wr;
	shift_status_ 	= _shift_status;
  }
  else
  {
      printf ("Probe failed\n");
      exit (2);
  }
  if (spi_unit == BITBANGOR)
  {
      spi_data_bit = 0;
      spi_sclk (0);
	CSoff();
  }else{
	CSoff();
  }
}

//! spi_select is has been rewritten to take advantage of the
//! h/w shift out select logic. 
//! spi_select, is called to set the CS bit for the desired bus.
//! It is called by the user level code, and internally.  The
//! code assumes that SPI bus 3 (the forth bus) is not connected
//! to a device.  The support code at the user level uses a
//! macro to turn the select on and off.
unsigned char cvtr[5] = { 0,PARK,ENETCS0,SDCS1,FRAMCS2};
unsigned char cvtr_res;
void
spi_select (unsigned char channel)
{
TRACE("");
  if (spi_unit == BITBANGOR){
  }
  else if(spi_unit == SHIFTOUT) // shift register 
  {
	spi_channel = channel;
	cvtr_res = cvtr[channel];
ASM;
	ld	a,(_shift_ctrl_wr_)
	ld	c,a
	ld	a,(_cvtr_res)
	out	(c),a	
ENDASM;
  }else
	printf("Must call spi_init() first\n");
}

//! spi_sclk is used internally to set the clock level to 
//! one or zero as needed.
void
spi_sclk (unsigned char state)
{
  if(spi_unit == SHIFTOUT) return;
  if (state)
    {
ASM;
	ld	a,(__spi_data)
	ld	c,a
      	ld a, (_spi_data_bit) 
	or a, 0x80 
	out (c), a	; CLOCK (0xff);
ENDASM;
    }
  else
    {
ASM;
      ; CLOCK (0);
	ld	a,(__spi_data)
	ld	c,a
      	ld a, (_spi_data_bit) 
	out (c), a
ENDASM;
    }
}

//! spi_delay,  wait time in micro seconds.....
//!the smallest wait time will be ~10usec
//!
//!The smaller the wait time the less accurate
//!the delay will be.
//!
//!But at larger number, say, 100 or more microseconds
//!the error is neglegible.
void
spi_delay (unsigned int time)
{
ASM;
  	ld hl, 2	; 10 1.356 
	add hl, sp	; 11 1.492 
	ld e, (hl)	; 7 .949 
	inc hl		; 6 .8138 
	ld d, (hl)	; 7 .949
; now decrement waittime until zero 
	ex de, hl	; 4 .5425
.spi_l1 dec hl		; 6 .8138 
	ld a, h		; 9 1.221 
	or l		; 4 .5425 
	jr nz,spi_l1	; 12 / 7 1.628 / .949
ENDASM;
}

unsigned char byte_in;
unsigned char bit;
unsigned char lbyte_out;
int i;
unsigned char l_len;
unsigned char *l_buffer;
/* from spi spec wiki page */

//! spi_byte_io, is called by the user level code to read and write
//! bytes on the SPI bus.  This code can read and write at the same
//! time.  Some h/w my not support the feature.

//! read spi buss in burst mode.  Note: we can only read
//! 256 bytes, so return 256 or less bytes read.

#ifdef NOTNOW
unsigned int spi_burst_read(unsigned char *buffer, unsigned int len)
{
	l_buffer = buffer;
	if(len > 256){
		l_len = 256;
	}else{
		l_len = len;
	} 
ASM;
	ld	c,_shift_rdtr 
	ld	hl,(_l_buffer)
	ld	a,(_l_len)
	ld	b,a
	inir
ENDASM;
	return ((unsigned int)l_len);
}
#endif
unsigned char
spi_byte_io (unsigned char byte_out)
{
  	lbyte_out = byte_out;
  if(spi_unit == SHIFTOUT){
ASM;
	di	
	ld	a,(_lbyte_out)
	out	(_shift_wrtr),a
	in	a,(_shift_rdntr)
	ld	(_byte_in),a
	ei
ENDASM;
  } else {
	i = 8;
  	byte_in = 0;
  	bit = 0x80;
ASM;
  ; assembly language version of the byte exchanger
  ; written with hopes in speeding up the i / o process
  ; please note that the default clock rate for SD cards
  ; is 25 mhz, about 3 times faster that the base clock rate
  ; of the cpu.so no worry about out running the card.
  ; MOSI send a bit out.
	ld 	a,(__spi_data)
	ld	c,a
._M3 	ld a,(_lbyte_out)	; get the output data byte 
	ld b,a 
	ld a,(_bit)		; get the bit mask 
	and a, b		; isolate the bit 
	jr z, _M1 
	ld a, 040h		; make sure we keep a copy 
				; of the data bit for latter.
._M1 	ld (_spi_data_bit) , a	; write out the data bit 
	out (c), a		; CLOCK to on
; turn on the clock bit, and keep the data bit set to its current state
    	or 80h 
	out (c), a		; MISO check for incomeing bit nop
; 8 microseconds, for the slave to set its MISO bit.
	in a, (c)		; then read the data bit
; decode and store a received data bit 
	and 4 
	jr z, _M2		; a is ether bit or zero 
	ld a, (_bit)
._M2 	ld b, a			; build up a byte here 
	ld a, (_byte_in) 
	or b 
	ld (_byte_in), a	; CLOCK to off
; turn off the clock bit, but keep the data bit at its current state
    	ld a, (_spi_data_bit) 
	out (c), a		; now keep house 
	ld hl, _bit		; shift the bit mask 
	srl (hl)		; count down to zero.
	ld hl, _i		; count the bit 
	dec (hl) 
	jr nz, _M3
ENDASM;
    }
    return byte_in;

}
//! spi_write, write a buffer of data to the spi bus.
void
spi_write (unsigned char *buf, int len)
{
  while (len--)
    spi_byte_io (*buf++);
}
//! spi_read, read a buffer of data from the spi bus.
void
spi_read (unsigned char *buf, int len)
{
  int i;
  i = 0;
  while (len--)
    buf[i++] = spi_byte_io (0xff);
}

#ifdef FRAM
//! NEW SPI code for use with FRAM expecting to migrate some functions
void SpiSelect(unsigned char Port)
{
	spi_select(Port);
}
void SpiCommand(unsigned char Cmd)
{
	lbyte_out = Cmd;
ASM;
	ld	a,(_lbyte_out)
	out	(5ch),a
ENDASM;
}
void SpiWrite(unsigned char Byte)
{
	lbyte_out = Byte;
ASM;
	ld	a,(_lbyte_out)
	out	(5ch),a
ENDASM;
}
void SpiWrite16(unsigned int Addr)
{
	lbyte_out = (Addr>>8)&0xff;
ASM;
	ld	a,(_lbyte_out)
	out	(5ch),a
ENDASM;
	lbyte_out = (Addr)&0xff;
ASM;
	ld	a,(_lbyte_out)
	out	(5ch),a
ENDASM;
}
// for vauge reasons we must write an ff to the bus
// and read back data right behind it....
unsigned char SpiRead()
{
	lbyte_out = 0xff;
ASM;
	ld	a,(_lbyte_out)
	out	(5ch),a
	in	a,(5ch)
	ld	(_byte_in),a
ENDASM;
	return byte_in;	
}
#endif
