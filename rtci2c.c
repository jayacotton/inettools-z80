#include <stdio.h>
#include "spi.h"
#include "rtci2c.h"

void
rtc_wr_unprotect ()
{
	rtc_write(7,0);
}

void
rtc_wr_protect ()
{
	rtc_write(7,0x80);
}

void
rtc_reset_off ()
{
#asm
	ld	a,mask_data+mask_rd+mask_rst
	out	(ds1302_port),a
#endasm
	spi_delay(1);
}

void
rtc_reset_on ()
{
#asm
	ld	a,mask_data+mask_rd
	out	(ds1302_port),a
#endasm
	spi_delay(1);
}

void
rtc_wr (uint8_t value) {
  int i;
  for (i = 0; i < 8; i++)
    {
      if (value & (1<<i))
	{// one bit
#asm
	ld	a,mask_rst+mask_data 	; turn on data bit
	out	(ds1302_port),a
#endasm
		// may need delay
	spi_delay(5);
#asm
	ld	a,mask_rst+mask_clk+mask_data  ; turn on clock bit
	out	(ds1302_port),a
#endasm
	spi_delay(5);
#asm
	ld	a,mask_rst	; turn off data bit
	out	(ds1302_port),a
#endasm
	}
      else
	{// zero bit
#asm
	ld	a,mask_rst	; data bit zero
	out	(ds1302_port),a
#endasm
		// may need delay
	spi_delay(5);
#asm
	ld	a,mask_rst+mask_clk	; turn on clock	
	out	(ds1302_port),a
#endasm
	spi_delay(5);
	}
	// may need delay
    }
}
uint8_t rtc_n;
uint8_t rtc_rd()
{
uint8_t i;

	rtc_n = 0;
#asm
	ld	b,1
	ld	c,0
#endasm
	for(i=0;i<8;i++){
#asm
	ld	a,mask_rst+mask_rd
	out	(ds1302_port),a
	push	bc
#endasm
	spi_delay(5);
#asm
	pop	bc
	in	a,(ds1302_port)
	bit	0,a
	jp	z,_rtc_rd2
	ld	a,c
	add	a,b
	ld	c,a
._rtc_rd2 rlc	b
	ld	a,mask_rst+mask_clk+mask_rd
	out	(ds1302_port),a
	push	bc
#endasm
	spi_delay(5);
#asm
	pop	bc
#endasm
	}
#asm
	ld	a,c
	ld	(_rtc_n),a
#endasm
	return	rtc_n;
}
uint8_t rtc_read(uint8_t addr)
{
uint8_t n;
	rtc_reset_off();
	rtc_wr(128+(addr<<1)+1);
	n = rtc_rd();
	rtc_reset_off();
	return n;
}
void
rtc_write (uint8_t addr, uint8_t value)
{
  rtc_reset_off ();
  rtc_wr (128 + (addr << 1));
  rtc_wr (value);
  rtc_reset_on ();
}

void rtc_get_now( uint8_t *seconds, uint8_t *minutes, uint8_t *hours,
     uint8_t *wday, uint8_t *month, uint8_t *days, uint16_t *year)
{// burst read 
   rtc_reset_off();
   rtc_wr(0xbf);
   *seconds = rtc_rd();
   *minutes = rtc_rd();
   *hours   = rtc_rd();
   *days    = rtc_rd();
   *month   = rtc_rd();
   *wday    = rtc_rd();
   *year    = rtc_rd();
   rtc_reset_on();
}
void
rtc_set_now ( uint8_t seconds, uint8_t minutes, uint8_t hours,
     uint8_t wday, uint8_t month, uint8_t days, uint16_t year)
{
  rtc_reset_off();
  rtc_wr_unprotect ();
  rtc_wr(0xbe);
  rtc_wr (seconds);
  rtc_wr (minutes);
  rtc_wr (hours);
  rtc_wr (wday);
  rtc_wr (month);
  rtc_wr (days);
  rtc_wr ((uint8_t) (year-2000));
  rtc_wr_protect ();
  rtc_reset_on();
}
