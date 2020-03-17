//*****************************************************************************
//
//! \file socket.c
//! \brief SOCKET APIs Implements file.
//! \details SOCKET APIs like as Berkeley Socket APIs. 
//! \version 1.0.3
//! \date 2013/10/21
//! \par  Revision history
//!       <2015/02/05> Notice
//!        The version history is not updated after this point.
//!        Download the latest version directly from GitHub. Please visit the our GitHub repository for ioLibrary.
//!        >> https://github.com/Wiznet/ioLibrary_Driver
//!       <2014/05/01> V1.0.3. Refer to M20140501
//!         1. Implicit type casting -> Explicit type casting.
//!         2. replace 0x01 with PACK_REMAINED in recvfrom()
//!         3. Validation a destination ip in connect() & sendto(): 
//!            It occurs a fatal error on converting unint32 address if uint8* addr parameter is not aligned by 4byte address.
//!            Copy 4 byte addr value into temporary uint32 variable and then compares it.
//!       <2013/12/20> V1.0.2 Refer to M20131220
//!                    Remove Warning.
//!       <2013/11/04> V1.0.1 2nd Release. Refer to "20131104".
//!                    In sendto(), Add to clear timeout interrupt status (Sn_IR_TIMEOUT)
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
#include "socket.h"
#include "trace.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "dns.h"
#include "ethernet.h"
#include "netdb.h"

//M20150401 : Typing Error
//#define SOCK_ANY_PORT_NUM  0xC000;
#define SOCK_ANY_PORT_NUM  0xC000

static uint16_t sock_any_port = SOCK_ANY_PORT_NUM;
static uint16_t sock_io_mode = 0;
static uint16_t sock_is_sending = 0;

static uint16_t sock_remained_size[_WIZCHIP_SOCK_NUM_] = { 0, 0, };

//M20150601 : For extern declaration
//static uint8_t  sock_pack_info[_WIZCHIP_SOCK_NUM_] = {0,};
uint8_t sock_pack_info[_WIZCHIP_SOCK_NUM_] = { 0, };

//

#if _WIZCHIP_ == 5200
static uint16_t sock_next_rd[_WIZCHIP_SOCK_NUM_] = { 0, };
#endif

//A20150601 : For integrating with W5300
#if _WIZCHIP_ == 5300
uint8_t sock_remained_byte[_WIZCHIP_SOCK_NUM_] = { 0, };	// set by wiz_recv_data()
#endif


#define CHECK_SOCKNUM()   \
   do{                    \
      if(sn > _WIZCHIP_SOCK_NUM_) return SOCKERR_SOCKNUM;   \
   }while(0);             \

#define CHECK_SOCKMODE(mode)  \
   do{                     \
      if((getSn_MR(sn) & 0x0F) != mode) return SOCKERR_SOCKMODE;  \
   }while(0);              \

#define CHECK_SOCKINIT()   \
   do{                     \
      if((getSn_SR(sn) != SOCK_INIT)) return SOCKERR_SOCKINIT; \
   }while(0);              \

#define CHECK_SOCKDATA()   \
   do{                     \
      if(len == 0) return SOCKERR_DATALEN;   \
   }while(0);              \


/* this is not like bsd sockets at all.  */
int8_t
socket (uint8_t sn, uint8_t protocol, uint16_t port, uint8_t flag)
{
  TRACE ("socket");
  CHECK_SOCKNUM ();
  switch (protocol)
    {
    case Sn_MR_TCP:
      {
	uint32_t taddr;
	getSIPR ((uint8_t *) & taddr);
	if (taddr == 0)
	  return SOCKERR_SOCKINIT;
      }
    case Sn_MR_UDP:
    case Sn_MR_MACRAW:
    case Sn_MR_IPRAW:
      break;
    default:
      return SOCKERR_SOCKMODE;
    }
// was 4
  TRACE ("socket flag");
  if ((flag & 0x06) != 0)
    return SOCKERR_SOCKFLAG;

  if (flag != 0)
    {
      switch (protocol)
	{
	case Sn_MR_TCP:
	  TRACE ("socket flag");
	  if ((flag & (SF_TCP_NODELAY | SF_IO_NONBLOCK)) == 0)
	    return SOCKERR_SOCKFLAG;

	  break;
	case Sn_MR_UDP:
	  TRACE ("socket flag");
	  if (flag & SF_IGMP_VER2)
	    {
	      if ((flag & SF_MULTI_ENABLE) == 0)
		return SOCKERR_SOCKFLAG;
	    }
	  if (flag & SF_UNI_BLOCK)
	    {
	      TRACE ("socket flag");
	      if ((flag & SF_MULTI_ENABLE) == 0)
		return SOCKERR_SOCKFLAG;
	    }
	  break;
	default:
	  break;
	}
    }
  sock_close (sn);
  setSn_MR (sn, (protocol | (flag & 0xF0)));
  if (!port)
    {
      port = sock_any_port++;
      if (sock_any_port == 0xFFF0)
	sock_any_port = SOCK_ANY_PORT_NUM;
    }
  setSn_PORT (sn, port);
  setSn_CR (sn, Sn_CR_OPEN);
  while (getSn_CR (sn));
  sock_io_mode &= ~(1 << sn);
  sock_io_mode |= ((flag & SF_IO_NONBLOCK) << sn);
  sock_is_sending &= ~(1 << sn);
  sock_remained_size[sn] = 0;
  sock_pack_info[sn] = PACK_COMPLETED;
  while (getSn_SR (sn) == SOCK_CLOSED);
  return (int8_t) sn;
}

int8_t
sock_close (uint8_t sn)
{
  CHECK_SOCKNUM ();
//A20160426 : Applied the erratum 1 of W5300
#if   (_WIZCHIP_ == 5300)
  //M20160503 : Wrong socket parameter. s -> sn 
  //if( ((getSn_MR(s)& 0x0F) == Sn_MR_TCP) && (getSn_TX_FSR(s) != getSn_TxMAX(s)) ) 
  if (((getSn_MR (sn) & 0x0F) == Sn_MR_TCP)
      && (getSn_TX_FSR (sn) != getSn_TxMAX (sn)))
    {
      uint8_t destip[4] = { 0, 0, 0, 1 };
      // TODO
      // You can wait for completing to sending data;
      // wait about 1 second;
      // if you have completed to send data, skip the code of erratum 1
      // ex> wait_1s();
      //     if (getSn_TX_FSR(s) == getSn_TxMAX(s)) continue;
      // 
      //M20160503 : The socket() of close() calls close() itself again. It occures a infinite loop - close()->socket()->close()->socket()-> ~
      //socket(s,Sn_MR_UDP,0x3000,0);
      //sendto(s,destip,1,destip,0x3000); // send the dummy data to an unknown destination(0.0.0.1).
      setSn_MR (sn, Sn_MR_UDP);
      setSn_PORTR (sn, 0x3000);
      setSn_CR (sn, Sn_CR_OPEN);
      while (getSn_CR (sn) != 0);
      while (getSn_SR (sn) != SOCK_UDP);
      sendto (sn, destip, 1, destip, 0x3000);	// send the dummy data to an unknown destination(0.0.0.1).
    };
#endif
  setSn_CR (sn, Sn_CR_CLOSE);
  /* wait to process the command... */
  while (getSn_CR (sn));
  /* clear all interrupt of the socket. */
  setSn_IR (sn, 0xFF);
  //A20150401 : Release the sock_io_mode of socket n.
  sock_io_mode &= ~(1 << sn);
  //
  sock_is_sending &= ~(1 << sn);
  sock_remained_size[sn] = 0;
  sock_pack_info[sn] = 0;
  while (getSn_SR (sn) != SOCK_CLOSED);
  return SOCK_OK;
}

int8_t
listen (uint8_t sn)
{
  CHECK_SOCKNUM ();
  CHECK_SOCKMODE (Sn_MR_TCP);
  CHECK_SOCKINIT ();
  setSn_CR (sn, Sn_CR_LISTEN);
  while (getSn_CR (sn));
  while (getSn_SR (sn) != SOCK_LISTEN)
    {
      sock_close (sn);
      return SOCKERR_SOCKCLOSED;
    }
  return SOCK_OK;
}


int8_t
connect (uint8_t sn, uint8_t * addr, uint16_t port)
{
  uint32_t taddr;
  CHECK_SOCKNUM ();
  TRACE ("connect");
  if ((getSn_MR (sn) & 0x0f) != Sn_MR_TCP)
    {
      if ((getSn_MR (sn) & 0x0f) != Sn_MR_UDP)
	{
	  return SOCKERR_SOCKMODE;
	}
    }
  if ((getSn_SR (sn) != SOCK_INIT))
    {
      if ((getSn_SR (sn) != SOCK_UDP))
	{
	  return SOCKERR_SOCKINIT;
	}
    }
  taddr = ((uint32_t) addr[0] & 0x000000FF);
  taddr = (taddr << 8) + ((uint32_t) addr[1] & 0x000000FF);
  taddr = (taddr << 8) + ((uint32_t) addr[2] & 0x000000FF);
  taddr = (taddr << 8) + ((uint32_t) addr[3] & 0x000000FF);
  if (taddr == 0xFFFFFFFF || taddr == 0)
    return SOCKERR_IPINVALID;

  if (port == 0)
    return SOCKERR_PORTZERO;
  setSn_DIPR (sn, addr);
  setSn_DPORT (sn, port);
  setSn_CR (sn, Sn_CR_CONNECT);
  TRACE ("connect");
  while (getSn_CR (sn));
  TRACE ("connect");
  if (sock_io_mode & (1 << sn))
    return SOCK_BUSY;
  TRACE ("connect");
  while (getSn_SR (sn) != SOCK_ESTABLISHED)
    {
      if (getSn_IR (sn) & Sn_IR_TIMEOUT)
	{
	  setSn_IR (sn, Sn_IR_TIMEOUT);
	  return SOCKERR_TIMEOUT;
	}

      if (getSn_SR (sn) == SOCK_CLOSED)
	{
	  return SOCKERR_SOCKCLOSED;
	}
    }

  TRACE ("connect");
  return SOCK_OK;
}

//int8_t
//disconnect (uint8_t sn)
//{
//  CHECK_SOCKNUM ();
//  CHECK_SOCKMODE (Sn_MR_TCP);
//  setSn_CR (sn, Sn_CR_DISCON);
//  /* wait to process the command... */
//  while (getSn_CR (sn));
//  sock_is_sending &= ~(1 << sn);
//  if (sock_io_mode & (1 << sn))
//    return SOCK_BUSY;
//  while (getSn_SR (sn) != SOCK_CLOSED)
 //   {
  //    if (getSn_IR (sn) & Sn_IR_TIMEOUT)
//	{
//	  sock_close (sn);
//	  return SOCKERR_TIMEOUT;
//	}
 //   }
 // return SOCK_OK;
//}

int32_t
send (uint8_t sn, uint8_t * buf, uint16_t len)
{
  uint8_t tmp = 0;
  uint16_t freesize = 0;

  CHECK_SOCKNUM ();
  //CHECK_SOCKMODE (Sn_MR_TCP);
  if ((getSn_MR (sn) & 0x0f) != Sn_MR_TCP)	// added so support udp
    {
      if ((getSn_MR (sn) & 0x0f) != Sn_MR_UDP)
	{
	  return SOCKERR_SOCKMODE;
	}
    }
  CHECK_SOCKDATA ();
  if ((getSn_MR (sn) & 0x0f) == Sn_MR_TCP)	// added so support udp
    {
      tmp = getSn_SR (sn);
      if (tmp != SOCK_ESTABLISHED && tmp != SOCK_CLOSE_WAIT)
	return SOCKERR_SOCKSTATUS;
    }
  if (sock_is_sending & (1 << sn))
    {
      tmp = getSn_IR (sn);
      if (tmp & Sn_IR_SENDOK)
	{
	  setSn_IR (sn, Sn_IR_SENDOK);
	  sock_is_sending &= ~(1 << sn);
	}
      else if (tmp & Sn_IR_TIMEOUT)
	{
	  sock_close (sn);
	  return SOCKERR_TIMEOUT;
	}
      else
	return SOCK_BUSY;
    }
  freesize = getSn_TxMAX (sn);
  if (len > freesize)
    len = freesize;		// check size not to exceed MAX size.
  while (1)
    {
      freesize = getSn_TX_FSR (sn);
      if ((getSn_MR (sn) & 0x0f) == Sn_MR_TCP)	// added so support udp
	{
	  tmp = getSn_SR (sn);
	  if ((tmp != SOCK_ESTABLISHED) && (tmp != SOCK_CLOSE_WAIT))
	    {
	      sock_close (sn);
	      return SOCKERR_SOCKSTATUS;
	    }
	}
      if ((sock_io_mode & (1 << sn)) && (len > freesize))
	return SOCK_BUSY;
      if (len <= freesize)
	break;
    }
  wiz_send_data (sn, buf, len);

  setSn_CR (sn, Sn_CR_SEND);
  /* wait to process the command... */
  while (getSn_CR (sn));
  sock_is_sending |= (1 << sn);
  return (int32_t) len;
}


int32_t
recv (uint8_t sn, uint8_t * buf, uint16_t len)
{
  uint8_t tmp = 0;
  uint16_t recvsize = 0;
  CHECK_SOCKNUM ();
  //CHECK_SOCKMODE (Sn_MR_TCP);
  if ((getSn_MR (sn) & 0x0f) != Sn_MR_TCP)	// added so support udp
    {
      if ((getSn_MR (sn) & 0x0f) != Sn_MR_UDP)
	{
	  return SOCKERR_SOCKMODE;
	}
    }
  CHECK_SOCKDATA ();

  recvsize = getSn_RxMAX (sn);
  if (recvsize < len)
    len = recvsize;

  while (1)
    {
      recvsize = getSn_RX_RSR (sn);
      if ((getSn_MR (sn) & 0x0f) == Sn_MR_TCP)	// added so support udp
	{
	  tmp = getSn_SR (sn);
	  if (tmp != SOCK_ESTABLISHED)
	    {
	      if (tmp == SOCK_CLOSE_WAIT)
		{
		  if (recvsize != 0)
		    break;
		  else if (getSn_TX_FSR (sn) == getSn_TxMAX (sn))
		    {
		      sock_close (sn);
		      return SOCKERR_SOCKSTATUS;
		    }
		}
	      else
		{
		  sock_close (sn);
		  return SOCKERR_SOCKSTATUS;
		}
	    }
	}
      if ((sock_io_mode & (1 << sn)) && (recvsize == 0))
	return SOCK_BUSY;
      if (recvsize != 0)
	break;
    };

  if (recvsize < len)
    len = recvsize;
  wiz_recv_data (sn, buf, len);
  setSn_CR (sn, Sn_CR_RECV);
  while (getSn_CR (sn));

  return (int32_t) len;
}

int32_t
sendto (uint8_t sn, uint8_t * buf, uint16_t len, uint8_t * addr,
	uint16_t port)
{
  uint8_t tmp = 0;
  uint16_t freesize = 0;
  uint32_t taddr;

  CHECK_SOCKNUM ();
  switch (getSn_MR (sn) & 0x0F)
    {
    case Sn_MR_UDP:
    case Sn_MR_MACRAW:
    case Sn_MR_IPRAW:
      break;
    default:
      return SOCKERR_SOCKMODE;
    }
  CHECK_SOCKDATA ();
  taddr = ((uint32_t) addr[0]) & 0x000000FF;
  taddr = (taddr << 8) + ((uint32_t) addr[1] & 0x000000FF);
  taddr = (taddr << 8) + ((uint32_t) addr[2] & 0x000000FF);
  taddr = (taddr << 8) + ((uint32_t) addr[3] & 0x000000FF);
  if ((taddr == 0) && ((getSn_MR (sn) & Sn_MR_MACRAW) != Sn_MR_MACRAW))
    return SOCKERR_IPINVALID;
  if ((port == 0) && ((getSn_MR (sn) & Sn_MR_MACRAW) != Sn_MR_MACRAW))
    return SOCKERR_PORTZERO;
  tmp = getSn_SR (sn);

  setSn_DIPR (sn, addr);
  setSn_DPORT (sn, port);
  freesize = getSn_TxMAX (sn);
  if (len > freesize)
    len = freesize;		// check size not to exceed MAX size.
  while (1)
    {
      freesize = getSn_TX_FSR (sn);
      if (getSn_SR (sn) == SOCK_CLOSED)
	return SOCKERR_SOCKCLOSED;
      if ((sock_io_mode & (1 << sn)) && (len > freesize))
	return SOCK_BUSY;
      if (len <= freesize)
	break;
    };
  wiz_send_data (sn, buf, len);

  setSn_CR (sn, Sn_CR_SEND);
  /* wait to process the command... */
  while (getSn_CR (sn));
  while (1)
    {
      tmp = getSn_IR (sn);
      if (tmp & Sn_IR_SENDOK)
	{
	  setSn_IR (sn, Sn_IR_SENDOK);
	  break;
	}
      else if (tmp & Sn_IR_TIMEOUT)
	{
	  setSn_IR (sn, Sn_IR_TIMEOUT);
	  return SOCKERR_TIMEOUT;
	}
    }
  return (int32_t) len;
}



int32_t
recvfrom (uint8_t sn, uint8_t * buf, uint16_t len, uint8_t * addr,
	  uint16_t * port)
{
  uint8_t mr;
  uint8_t head[8];
  uint16_t pack_len = 0;

  CHECK_SOCKNUM ();
  switch ((mr = getSn_MR (sn)) & 0x0F)
    {
    case Sn_MR_UDP:
    case Sn_MR_IPRAW:
    case Sn_MR_MACRAW:
      break;
    default:
      return SOCKERR_SOCKMODE;
    }
  CHECK_SOCKDATA ();
  if (sock_remained_size[sn] == 0)
    {
      while (1)
	{
	  pack_len = getSn_RX_RSR (sn);
	  if (getSn_SR (sn) == SOCK_CLOSED)
	    return SOCKERR_SOCKCLOSED;
	  if ((sock_io_mode & (1 << sn)) && (pack_len == 0))
	    return SOCK_BUSY;
	  if (pack_len != 0)
	    break;
	};
    }
  switch (mr & 0x07)
    {
    case Sn_MR_UDP:
      if (sock_remained_size[sn] == 0)
	{
	  wiz_recv_data (sn, head, 8);
	  setSn_CR (sn, Sn_CR_RECV);
	  while (getSn_CR (sn));
	  addr[0] = head[0];
	  addr[1] = head[1];
	  addr[2] = head[2];
	  addr[3] = head[3];
	  *port = head[4];
	  *port = (*port << 8) + head[5];
	  sock_remained_size[sn] = head[6];
	  sock_remained_size[sn] = (sock_remained_size[sn] << 8) + head[7];
	  sock_pack_info[sn] = PACK_FIRST;
	}
      if (len < sock_remained_size[sn])
	pack_len = len;
      else
	pack_len = sock_remained_size[sn];
      len = pack_len;
      //
      // Need to packet length check (default 1472)
      //
      wiz_recv_data (sn, buf, pack_len);	// data copy.
      break;
    case Sn_MR_MACRAW:
      if (sock_remained_size[sn] == 0)
	{
	  wiz_recv_data (sn, head, 2);
	  setSn_CR (sn, Sn_CR_RECV);
	  while (getSn_CR (sn));
	  // read peer's IP address, port number & packet length
	  sock_remained_size[sn] = head[0];
	  sock_remained_size[sn] =
	    (sock_remained_size[sn] << 8) + head[1] - 2;
	  if (sock_remained_size[sn] > 1514)
	    {
	      sock_close (sn);
	      return SOCKFATAL_PACKLEN;
	    }
	  sock_pack_info[sn] = PACK_FIRST;
	}
      if (len < sock_remained_size[sn])
	pack_len = len;
      else
	pack_len = sock_remained_size[sn];
      wiz_recv_data (sn, buf, pack_len);
      break;
      //#if ( _WIZCHIP_ < 5200 )
    case Sn_MR_IPRAW:
      if (sock_remained_size[sn] == 0)
	{
	  wiz_recv_data (sn, head, 6);
	  setSn_CR (sn, Sn_CR_RECV);
	  while (getSn_CR (sn));
	  addr[0] = head[0];
	  addr[1] = head[1];
	  addr[2] = head[2];
	  addr[3] = head[3];
	  sock_remained_size[sn] = head[4];
	  //M20150401 : For Typing Error
	  //sock_remaiend_size[sn] = (sock_remained_size[sn] << 8) + head[5];
	  sock_remained_size[sn] = (sock_remained_size[sn] << 8) + head[5];
	  sock_pack_info[sn] = PACK_FIRST;
	}
      //
      // Need to packet length check
      //
      if (len < sock_remained_size[sn])
	pack_len = len;
      else
	pack_len = sock_remained_size[sn];
      wiz_recv_data (sn, buf, pack_len);	// data copy.
      break;
      //#endif
    default:
      wiz_recv_ignore (sn, pack_len);	// data copy.
      sock_remained_size[sn] = pack_len;
      break;
    }
  setSn_CR (sn, Sn_CR_RECV);
  /* wait to process the command... */
  while (getSn_CR (sn));
  sock_remained_size[sn] -= pack_len;
  if (sock_remained_size[sn] != 0)
    {
      sock_pack_info[sn] |= PACK_REMAINED;
    }
  else
    sock_pack_info[sn] = PACK_COMPLETED;
  return (int32_t) pack_len;
}


//int8_t
//ctlsocket (uint8_t sn, ctlsock_type cstype, void *arg)
//{
//  uint8_t tmp = 0;
//  CHECK_SOCKNUM ();
//  switch (cstype)
//    {
//    case CS_SET_IOMODE:
//      tmp = *((uint8_t *) arg);
//      if (tmp == SOCK_IO_NONBLOCK)
//	sock_io_mode |= (1 << sn);
//      else if (tmp == SOCK_IO_BLOCK)
//	sock_io_mode &= ~(1 << sn);
//      else
//	return SOCKERR_ARG;
 //     break;
//    case CS_GET_IOMODE:
//      //M20140501 : implict type casting -> explict type casting
//      //*((uint8_t*)arg) = (sock_io_mode >> sn) & 0x0001;
//      *((uint8_t *) arg) = (uint8_t) ((sock_io_mode >> sn) & 0x0001);
//      //
//      break;
//    case CS_GET_MAXTXBUF:
//      *((uint16_t *) arg) = getSn_TxMAX (sn);
//      break;
//    case CS_GET_MAXRXBUF:
//      *((uint16_t *) arg) = getSn_RxMAX (sn);
//      break;
//    case CS_CLR_INTERRUPT:
//      if ((*(uint8_t *) arg) > SIK_ALL)
//	return SOCKERR_ARG;
//      setSn_IR (sn, *(uint8_t *) arg);
//      break;
//    case CS_GET_INTERRUPT:
//      *((uint8_t *) arg) = getSn_IR (sn);
//      break;
//    case CS_SET_INTMASK:
//      if ((*(uint8_t *) arg) > SIK_ALL)
//	return SOCKERR_ARG;
//      setSn_IMR (sn, *(uint8_t *) arg);
//      break;
//    case CS_GET_INTMASK:
//      *((uint8_t *) arg) = getSn_IMR (sn);
//      break;
//    default:
//      return SOCKERR_ARG;
//    }
//  return SOCK_OK;
//}

//int8_t
//setsockopt (uint8_t sn, sockopt_type sotype, void *arg)
//{
//  CHECK_SOCKNUM ();
//  switch (sotype)
//    {
//    case SO_TTL:
//      setSn_TTL (sn, *(uint8_t *) arg);
//      break;
//    case SO_TOS:
//      setSn_TOS (sn, *(uint8_t *) arg);
//      break;
//   case SO_MSS:
//      setSn_MSSR (sn, *(uint16_t *) arg);
//      break;
//    case SO_DESTIP:
//      setSn_DIPR (sn, (uint8_t *) arg);
//      break;
//    case SO_DESTPORT:
//      setSn_DPORT (sn, *(uint16_t *) arg);
//      break;
//    case SO_KEEPALIVESEND:
//      CHECK_SOCKMODE (Sn_MR_TCP);
//      if (getSn_KPALVTR (sn) != 0)
//	return SOCKERR_SOCKOPT;
//      setSn_CR (sn, Sn_CR_SEND_KEEP);
//      while (getSn_CR (sn) != 0)
//	{
//	  // M20131220
//	  //if ((tmp = getSn_IR(sn)) & Sn_IR_TIMEOUT)
//	  if (getSn_IR (sn) & Sn_IR_TIMEOUT)
//	    {
//	      setSn_IR (sn, Sn_IR_TIMEOUT);
//	      return SOCKERR_TIMEOUT;
//	    }
//	}
 //     break;
//    case SO_KEEPALIVEAUTO:
//      CHECK_SOCKMODE (Sn_MR_TCP);
//      setSn_KPALVTR (sn, *(uint8_t *) arg);
//      break;
//    default:
//      return SOCKERR_ARG;
//    }
//  return SOCK_OK;
//}

//int8_t
//getsockopt (uint8_t sn, sockopt_type sotype, void *arg)
//{
//  CHECK_SOCKNUM ();
//  switch (sotype)
//    {
//    case SO_FLAG:
//      *(uint8_t *) arg = getSn_MR (sn) & 0xF0;
//      break;
//    case SO_TTL:
//      *(uint8_t *) arg = getSn_TTL (sn);
//      break;
//    case SO_TOS:
//      *(uint8_t *) arg = getSn_TOS (sn);
//      break;
//    case SO_MSS:
//      *(uint16_t *) arg = getSn_MSSR (sn);
//      break;
//    case SO_DESTIP:
//      getSn_DIPR (sn, (uint8_t *) arg);
//      break;
//    case SO_DESTPORT:
//      *(uint16_t *) arg = getSn_DPORT (sn);
//      break;
//    case SO_KEEPALIVEAUTO:
//      CHECK_SOCKMODE (Sn_MR_TCP);
//      *(uint16_t *) arg = getSn_KPALVTR (sn);
//      break;
//    case SO_SENDBUF:
//      *(uint16_t *) arg = getSn_TX_FSR (sn);
//      break;
//    case SO_RECVBUF:
//      *(uint16_t *) arg = getSn_RX_RSR (sn);
//      break;
//    case SO_STATUS:
//      *(uint8_t *) arg = getSn_SR (sn);
//      break;
//    case SO_REMAINSIZE:
//      if (getSn_MR (sn) & Sn_MR_TCP)
//	*(uint16_t *) arg = getSn_RX_RSR (sn);
//      else
//	*(uint16_t *) arg = sock_remained_size[sn];
 //     break;
 //   case SO_PACKINFO:
//      if ((getSn_MR (sn) == Sn_MR_TCP))
//	return SOCKERR_SOCKMODE;
//      *(uint8_t *) arg = sock_pack_info[sn];
//      break;
//    default:
//      return SOCKERR_SOCKOPT;
//    }
//  return SOCK_OK;
//}

/* add some bsd like functions */
/* avoiding malloc etc.... */

struct hostent HostEnt;
char HostName[80];		/* this needs to be redone */
char HostAddr[4];
char *HA;

/* assume a host name string or an IP address 
as an ascii string */

//struct hostent *
//gethostbyname (char *name)
//{
//  char *p;
//  unsigned char dns[4];
//  HA = HostAddr;
//  strncpy (HostName, name, 80);
//  HostEnt.h_name = HostName;
//  HostEnt.h_addr_list = &HA;
//  if (isdigit (name[0]))
//    {				// its an ip address string
//      while (p = strchr (HostName, '.'))
//	{
//	  *p = ' ';
//	}
 //     sscanf (HostName, "%d %d %d %d",
//	      &HostAddr[0], &HostAddr[1], &HostAddr[2], &HostAddr[3]);
 //     return &HostEnt;
  //  }
//  else
//    {				// its a real host name to look up.
//      Ethernet_localDNS (dns);	// make these globals to reduce call stack
//      if (DNS_run (dns, HostName, HostAddr) == 1)
//	return &HostEnt;
 //   }
 // return (struct hostent *) 0;
//}
