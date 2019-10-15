//*****************************************************************************
//
//  Modified for cpm and z88dk by Jay Cotton  3/25/19
//
//! \file w5500.c
//! \brief W5500 HAL Interface.
//! \version 1.0.2
//! \date 2013/10/21
//! \par  Revision history
//!       <2015/02/05> Notice
//!        The version history is not updated after this point.
//!        Download the latest version directly from GitHub. Please visit the our GitHub repository for ioLibrary.
//!        >> https://github.com/Wiznet/ioLibrary_Driver
//!       <2014/05/01> V1.0.2
//!         1. Implicit type casting -> Explicit type casting. Refer to M20140501
//!            Fixed the problem on porting into under 32bit MCU
//!            Issued by Mathias ClauBen, wizwiki forum ID Think01 and bobh
//!            Thank for your interesting and serious advices.
//!       <2013/12/20> V1.0.1
//!         1. Remove warning
//!         2. WIZCHIP_READ_BUF spi_write in case _WIZCHIP_IO_MODE_SPI_FDM_
//!            for loop optimized(removed). refer to M20131220
//!       <2013/10/21> 1st Release
//! \author MidnightCow
//! \copyright
//!
//! Copyright (c)  2013, WIZnet Co., LTD.
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
#include "w5500.h"
#include "spi.h"
#include "trace.h"

unsigned char w5500_present;

#define _W5500_SPI_VDM_OP_          0x00
#define _W5500_SPI_FDM_OP_LEN1_     0x01
#define _W5500_SPI_FDM_OP_LEN2_     0x02
#define _W5500_SPI_FDM_OP_LEN4_     0x03

#if   (_WIZCHIP_ == 5500)
////////////////////////////////////////////////////

void
writeMR (unsigned char val)
{
  WIZCHIP_WRITE (MR, val);
}

unsigned char
readMR ()
{
  return WIZCHIP_READ (MR);
}

unsigned char
readVERSIONR_W5500 ()
{
  return WIZCHIP_READ (VERSIONR);
}

unsigned char
softReset ()
{

  int count;
  int mr;
  count = 0;

  writeMR (0x80);
  do
    {
      mr = readMR ();
      if (mr == 0)
	return 1;
      spi_delay (1000);
    }
  while (++count < 20);
  return 0;

}

void
WIZCHIP_PRESENT ()
{
  unsigned char version;

  w5500_present = 0;		// no yet ...
  if (!softReset ())
    return 0;
  writeMR (0x08);
  if (readMR () != 0x08)
    return 0;
  writeMR (0x10);
  if (readMR () != 0x10)
    return 0;
  writeMR (0);
  if (readMR () != 0)
    return 0;
  version = readVERSIONR_W5500 ();
  if (version != 4)
    return 0;
  w5500_present++;		// found it

}

unsigned char
readPHYCFGR_W5500 ()
{
  return WIZCHIP_READ (PHYCFGR);
}

unsigned char
w5500_getLinkStatus ()
{
  int phystat;

  phystat = readPHYCFGR_W5500 ();
  if (phystat != 1)
    return LINK_ON;
  return LINK_OFF;
}

unsigned char
WIZCHIP_READ (uint32_t AddrSel)
{
  unsigned char ret;

  TRACE ("->WIZCHIP_READ");

  CSEther ();

  AddrSel |= (_W5500_SPI_READ_ | _W5500_SPI_VDM_OP_);

  SpiSendData ((AddrSel & 0x00FF0000) >> 16);
  SpiSendData ((AddrSel & 0x0000FF00) >> 8);
  SpiSendData ((AddrSel & 0x000000FF) >> 0);
  ret = SpiRecvData ();

  CSoff ();
  TRACE ("<-WIZCHIP_READ");
  return ret;
}

void
WIZCHIP_WRITE (uint32_t AddrSel, unsigned char wb)
{

  CSEther ();

  AddrSel |= (_W5500_SPI_WRITE_ | _W5500_SPI_VDM_OP_);

  SpiSendData ((AddrSel & 0x00FF0000) >> 16);
  SpiSendData ((AddrSel & 0x0000FF00) >> 8);
  SpiSendData ((AddrSel & 0x000000FF) >> 0);
  SpiSendData (wb);

  CSoff ();
}

void
WIZCHIP_READ_BUF (uint32_t AddrSel, unsigned char *pBuf, uint16_t len)
{
  uint16_t i;
  uint8_t *p;
  uint16_t rs;

  CSEther ();

  AddrSel |= (_W5500_SPI_READ_ | _W5500_SPI_VDM_OP_);

  SpiSendData ((AddrSel & 0x00FF0000) >> 16);
  SpiSendData ((AddrSel & 0x0000FF00) >> 8);
  SpiSendData ((AddrSel & 0x000000FF) >> 0);
#ifdef SPI_BURST_READ
  printf ("burst_read %d\n", len);
  p = pBuf;
  rs = spi_burst_read (p, len);
  p += rs;
  if (rs < len)
    {
      len = len - rs;
      printf ("read more %d\n", len);
      while (len > 0)
	{
	  rs = spi_burst_read (p, len);
	  len = len - rs;
	  printf ("read more %d\n", len);
	}
    }
#else
  for (i = 0; i < len; i++)
    pBuf[i] = SpiRecvData ();
#endif
  CSoff ();
}

void
WIZCHIP_WRITE_BUF (uint32_t AddrSel, unsigned char *pBuf, uint16_t len)
{
  uint16_t i;

  CSEther ();

  AddrSel |= (_W5500_SPI_WRITE_ | _W5500_SPI_VDM_OP_);

  SpiSendData ((AddrSel & 0x00FF0000) >> 16);
  SpiSendData ((AddrSel & 0x0000FF00) >> 8);
  SpiSendData ((AddrSel & 0x000000FF) >> 0);
  for (i = 0; i < len; i++)
    SpiSendData (pBuf[i]);

  CSoff ();
}


uint16_t
getSn_TX_FSR (unsigned char sn)
{
  uint16_t val = 0, val1 = 0;

  do
    {
      val1 = WIZCHIP_READ (Sn_TX_FSR (sn));
      val1 =
	(val1 << 8) + WIZCHIP_READ (WIZCHIP_OFFSET_INC (Sn_TX_FSR (sn), 1));
      if (val1 != 0)
	{
	  val = WIZCHIP_READ (Sn_TX_FSR (sn));
	  val =
	    (val << 8) +
	    WIZCHIP_READ (WIZCHIP_OFFSET_INC (Sn_TX_FSR (sn), 1));
	}
    }
  while (val != val1);
  return val;
}


uint16_t
getSn_RX_RSR (unsigned char sn)
{
  uint16_t val = 0, val1 = 0;

  do
    {
      val1 = WIZCHIP_READ (Sn_RX_RSR (sn));
      val1 =
	(val1 << 8) + WIZCHIP_READ (WIZCHIP_OFFSET_INC (Sn_RX_RSR (sn), 1));
      if (val1 != 0)
	{
	  val = WIZCHIP_READ (Sn_RX_RSR (sn));
	  val =
	    (val << 8) +
	    WIZCHIP_READ (WIZCHIP_OFFSET_INC (Sn_RX_RSR (sn), 1));
	}
    }
  while (val != val1);
  return val;
}

void
wiz_send_data (unsigned char sn, unsigned char *wizdata, uint16_t len)
{
  uint16_t ptr = 0;
  uint32_t addrsel = 0;

  if (len == 0)
    return;
  ptr = getSn_TX_WR (sn);
  addrsel = ((uint32_t) ptr << 8) + (WIZCHIP_TXBUF_BLOCK (sn) << 3);
  WIZCHIP_WRITE_BUF (addrsel, wizdata, len);

  ptr += len;
  setSn_TX_WR (sn, ptr);
}

void
wiz_recv_data (unsigned char sn, unsigned char *wizdata, uint16_t len)
{
  uint16_t ptr = 0;
  uint32_t addrsel = 0;

  if (len == 0)
    return;
  ptr = getSn_RX_RD (sn);
  addrsel = ((uint32_t) ptr << 8) + (WIZCHIP_RXBUF_BLOCK (sn) << 3);
  WIZCHIP_READ_BUF (addrsel, wizdata, len);
  ptr += len;

  setSn_RX_RD (sn, ptr);
}


void
wiz_recv_ignore (unsigned char sn, uint16_t len)
{
  uint16_t ptr = 0;

  ptr = getSn_RX_RD (sn);
  ptr += len;
  setSn_RX_RD (sn, ptr);
}

#endif
