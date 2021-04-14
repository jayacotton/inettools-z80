
//******************************************************************************
//
//! \file spi.h
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
		
#include <stdlib.h>

/* the assembly code has been modified to accept the 
#define base addresses selected below */

/* 2 types of spi bus cards */

#define BITBANGOR 1
#define SHIFTOUT 2

#define shift_select_0 0x8		/* select offboard spi bus */
#define shift_select_1 0x10
#define shift_select_2 0x20
#define shift_base 	0x5c		/* 1c is default */
#define _shift_wrtr 	shift_base	/* write data and transfer */
#define _shift_rdtr 	shift_base+1	/* read data and transfer */
#define _shift_rdntr 	shift_base	/* read data no transfer */
#define _shift_ctrl_wr 	shift_base+2
#define _shift_status 	shift_base+2
#define shift_cd0	1		/* IN/OUT:SD_OPREG:0 = CD0, PMOD pull CD0 low */

#define spi_base 0xc0	// default base address for spi bus
#define spi_sel   spi_base+1	// select address
#define spi_data  spi_base	// data port
#define spi_clock spi_base	// clock port
#define clock_bit 0x80
#define mosi_bit  0x40
#define miso_bit  4
#define select_0  0
#define select_1  1
#define select_2  2
#define select_3  3

extern unsigned char spi_data_bit;

#define SELSLAV(a) spi_select(a)
#define CLOCK(a)   outp(spi_clock,((a &clock_bit)|spi_data_bit))
#define MOSI(a)    {outp(spi_data ,a&mosi_bit); spi_data_bit = a&mosi_bit;}
#define MISO()     inp (spi_data)
#define SEL0() SELSLAV(0);
#define SEL1() SELSLAV(1);
#define SEL2() SELSLAV(2);
#define SELNONE() SELSLAV(3);
#define MOSI_HIGH() MOSI(0xff);
#define CLOCK_HIGH() CLOCK(0xff);
#define MOSI_LOW() MOSI(0);
#define CLOCK_LOW() CLOCK(0);

#define SPI_SCLK_LOW_TIME 1
#define SPI_SCLK_HIGH_TIME 1

extern void spi_init();
extern void spi_out(unsigned char);
extern void spi_select(unsigned char);
extern unsigned char spi_in();
extern void spi_sclk(unsigned char);
extern void spi_delay(unsigned int);
extern unsigned char spi_byte_io(unsigned char);
extern unsigned char exchange(unsigned char);
extern void spi_write(unsigned char * ,int);
extern void spi_read(unsigned char *, int);
extern unsigned short spi_burst_read(unsigned char *, unsigned short);

#define SDChannel		3
#define ENETChannel		2
#define IDELChannel		4
#define SDCS1			0x10
#define ENETCS0			0x8
#define	FRAMCS2			0x20
#define PARK			1
#define spi_exchange(val) 	spi_byte_io(val);
#define IINCHIP_SpiInit()	spi_init();
#define SpiInit()		spi_init();
//#define IINCHIP_CSoff()		spi_select(1);
#define IINCHIP_SpiSendData(val) spi_byte_io(val);
#define SpiSendData(val) 	spi_byte_io(val);
//#define IINCHIP_CSon()		spi_select(2);
//#define CSon()			spi_select(2);
#define CSoff()			spi_select(IDELChannel);
#define CSEther()		spi_select(ENETChannel);
#define CSSD()			spi_select(SDChannel);

#define IINCHIP_SpiRecvData()	spi_byte_io(0xff);
#define SpiRecvData()		spi_byte_io(0xff);

#define spi_start(i) 
#define spi_stop() 

#define SPI_ETHERNET_SETTINGS 1
