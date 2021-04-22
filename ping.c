//******************************************************************************
//
//! \file ping.c
//! \brief PING is used to test for the existance of a remote server.
//! \details Loosly based on the documents found at:
//!      https://tools.ietf.org/html/rfc2151 
//!
//! \version 2.0
//! \date 4/13/2019
//! \par Revision history
//!     4/13/2019  This version.  Copied from wiznet somewhere, now
//!             can't find the location.
//!             Modified extensivly for CP/M and z88dk.
//!             When I get milisecond timeing working, I will
//!             add loop time.
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

#include "ping.h"
#include "socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>
#include "ethernet.h"
#include "dns.h"
#include "spi.h"
#include "ltime.h"
#include <math.h>
#include "trace.h"
#include <malloc.h>


PINGMSGR PingRequest;		// Variable for Ping Request
PINGMSGR PingReply;		// Variable for Ping Reply
static uint16_t RandomID = 0x1234;
static uint16_t RandomSeqNum = 0x4321;
uint8_t ping_reply_received = 0;
uint8_t req = 0;
uint8_t rep = 0;

int run_user_applications;
struct wiz_NetInfo_t gWIZNETINFO;
unsigned char mac[6] = { 0x98, 0x76, 0xb6, 0x11, 0x00, 0xc4 };

unsigned char DNS_buffer[256];
unsigned char save_addr[4];
struct TICKSTORE t1;
struct TICKSTORE t2;

extern void wait_1ms (unsigned int);
void
main (int argc, char *argv[])
{
  unsigned char tmp;
  unsigned char ping_sockfd;
  unsigned char skip_dns;
  unsigned char remip[4];
  unsigned char dnsname[80];
  unsigned char ip[4];
  unsigned char dns[4];
#ifdef DEBUG
  unsigned int total;
  unsigned int max;
#endif
  char *p;
  ping_sockfd = 3;		// was so in an example
  skip_dns = 0;
  memset (dnsname, 0, 80);
  memcpy (gWIZNETINFO.mac, mac, 6);
#ifdef DEBUG
  mallinfo (&total, &max);
  printf ("total %d max %d\n", total, max);

  printf ("argc %d\n", argc);
  printf ("argv[0] %s\n", argv[0]);
  printf ("argv[1] %s\n", argv[1]);
#endif

  if (argc == 2)
    {
      strcat (dnsname, argv[1]);
      if (isdigit (dnsname[0]))
	{
	  while (p = strchr (dnsname, '.'))
	    {
	      *p = ' ';
	    }
	  sscanf (dnsname, "%d %d %d %d",
		  &remip[0], &remip[1], &remip[2], &remip[3]);
	  skip_dns = 1;
	}
    }
  else
    strcat (dnsname, "server");

  if (Ethernet_begin (mac) == 0)
    {
      if (Ethernet_hardwareStatus () == EthernetNoHardware)
	printf ("Can't find the ethernet h/w\n");
      if (Ethernet_linkStatus () == LinkOFF)
	printf ("Plug in the cable\n");
    }
  Ethernet_localIP (gWIZNETINFO.ip);
  Ethernet_localDNS (gWIZNETINFO.dns);
  run_user_applications = 0;
  if (skip_dns == 0)
    {
      DNS_init (SOCK_DNS, DNS_buffer);	// share the data buffer ??
      if (DNS_run (gWIZNETINFO.dns, dnsname, remip) == 1)
	{
	  run_user_applications = 1;
	}
      else
	{
	  run_user_applications = 0;
	}
    }
  else
    {
      run_user_applications = 1;
    }
  if (run_user_applications)
    {
      if (argc == 2)
	printf ("PING %s (%d.%d.%d.%d) \n", argv[1],
		remip[0], remip[1], remip[2], remip[3]);
      else
	printf ("PING (%d.%d.%d.%d) \n", remip[0],
		remip[1], remip[2], remip[3]);
      tmp = ping_auto (0, remip);
    }
}

uint8_t
ping_auto (uint8_t s, uint8_t * addr)
{
  uint8_t i;
  unsigned int len = 0;
  uint8_t cnt = 0;
  TRACE ("");
  for (i = 0; i <= 5; i++)
    {
      TRACE ("");
      switch (getSn_SR (s))
	{
	case SOCK_CLOSED:
	  sock_close (s);
	  IINCHIP_WRITE (Sn_PROTO (s), IPPROTO_ICMP);	// set ICMP Protocol
	  if (socket (s, Sn_MR_IPRAW, 3000, 0) != 0)
	    {			// open the SOCKET with IPRAW mode, if fail then Error
	      printf ("\r\n socket %d fail r\n", (0));
#ifdef PING_DEBUG
	      return SOCKET_ERROR;
#endif
	    }
	  /* Check socket register */
	  TRACE ("");
	  while (getSn_SR (s) != SOCK_IPRAW);
	  wait_1ms (1000);	// wait 1000ms
	  wait_1ms (1000);	// wait 1000ms
	  break;
	case SOCK_IPRAW:
	  TRACE ("");
	  ping_request (s, addr);
	  req++;
	  while (1)
	    {
	      if ((len = getSn_RX_RSR (s)) > 0)
		{
		  ping_reply (s, addr, len);
		  rep++;
		  break;
		}
	      else if (cnt > 100)
		{
		  printf ("Request Time out. \r\n");
		  cnt = 0;
		  break;
		}
	      else
		{
		  cnt++;
		  wait_1ms (50);	// wait 50ms
		}
	      // wait_time for 2 seconds, Break on fail
	    }

	  break;
	default:
	  TRACE ("");
	  break;

	}
      TRACE ("");
#ifdef PING_DEBUG
      if (req >= 3)
	{

	  printf ("Ping Request = %d, PING_Reply = %d\r\n", req, rep);

	  if (rep == req)
	    return SUCCESS;
	  else
	    return REPLY_ERROR;

	}
#endif
    }
  return SUCCESS;
}


uint8_t
ping_count (uint8_t s, uint16_t pCount, uint8_t * addr)
{

  uint16_t rlen, cnt, i;
  cnt = 0;

  for (i = 0; i < pCount + 1; i++)
    {

      if (i != 0)
	{
	  /* Output count number */
	  printf ("\r\nNo.%d\r\n", (i - 1));
	}

      switch (getSn_SR (s))
	{
	case SOCK_CLOSED:
	  sock_close (s);
	  // close the SOCKET
	  /* Create Socket */
	  IINCHIP_WRITE (Sn_PROTO (s), IPPROTO_ICMP);	// set ICMP Protocol
	  if (socket (s, Sn_MR_IPRAW, 3000, 0) != s)
	    {			// open the SOCKET with IPRAW mode, if fail then Error
	      printf ("\r\n socket %d fail r\n", (s));
#ifdef PING_DEBUG
	      return SOCKET_ERROR;
#endif
	    }
	  /* Check socket register */
	  while (getSn_SR (s) != SOCK_IPRAW);

	  wait_1ms (1000);	// wait 1000ms
	  wait_1ms (1000);	// wait 1000ms
	  break;

	case SOCK_IPRAW:
	  ping_request (s, addr);
	  req++;
	  while (1)
	    {
	      if ((rlen = getSn_RX_RSR (s)) > 0)
		{
		  ping_reply (s, addr, rlen);
		  rep++;
		  if (ping_reply_received)
		    break;

		}

	      /* wait_time for 2 seconds, Break on fail */
	      if ((cnt > 100))
		{
		  printf ("\r\nRequest Time out. \r\n");
		  cnt = 0;
		  break;
		}
	      else
		{
		  cnt++;
		  wait_1ms (50);	// wait 50ms
		}
	    }

	  break;

	default:
	  break;

	}
#ifdef PING_DEBUG
      if (req >= pCount)
	{
	  printf ("Ping Request = %d, PING_Reply = %d\r\n", req, rep);

	  if (rep == req)
	    return SUCCESS;
	  else
	    return REPLY_ERROR;
	}
#endif
    }

  return 0;
}

uint8_t
ping_request (uint8_t s, uint8_t * addr)
{
  uint16_t i;
  int n;

  TRACE ("");
  //Initailize flag for ping reply
  ping_reply_received = 0;
  /* make header of the ping-request  */
  PingRequest.Type = PING_REQUEST;	// Ping-Request
  PingRequest.Code = CODE_ZERO;	// Always '0'
  PingRequest.ID = htons (RandomID++);	// set ping-request's ID to random integer value
  PingRequest.SeqNum = htons (RandomSeqNum++);	// set ping-request's sequence number to ramdom integer value
  //size = 32;                                 // set Data size

  /* Fill in Data[]  as size of BIF_LEN (Default = 32) */
  for (i = 0; i < BUF_LEN; i++)
    {
      PingRequest.Data[i] = (i) % 8;	//'0'~'8' number into ping-request's data     
    }
  /* Do checksum of Ping Request */
  PingRequest.CheckSum = 0;	// value of checksum before calucating checksum of ping-request packet
  PingRequest.CheckSum = htons (checksum ((uint8_t *) & PingRequest, sizeof (PingRequest)));	// Calculate checksum

// get time tick base

  SetTime (&t1);
  /* sendto ping_request to destination */
  TRACE ("");
  if ((n =
       sendto (s, (uint8_t *) & PingRequest, sizeof (PingRequest), addr,
	       3000)) == 0)
    {				// Send Ping-Request to the specified peer.
      printf ("\r\n Fail to send ping-reply packet  r\n");
    }
  else
    {
#ifdef DEBUG
      printf ("%d bytes from ");
      printf ("%d.%d.%d.%d: ", (addr[0]), (addr[1]), (addr[2]), (addr[3]));
      printf (" icmp_seq=%x CheckSum:%x\r\n", htons (PingRequest.SeqNum),
	      htons (PingRequest.CheckSum));
#endif
    }
  return 0;
}				// ping request

void
saveaddr (unsigned char *addr)
{
  memcpy (save_addr, addr, 4);
}

void
restaddr (unsigned char *addr)
{
  memcpy (addr, save_addr, 4);
}

uint8_t
ping_reply (uint8_t s, uint8_t * addr, uint16_t rlen)
{

  uint16_t tmp_checksum;
  uint16_t len;
  uint16_t i;
  uint8_t data_buf[128];
  uint16_t port;
  PINGMSGR PingReply;
  port = 3000;


  /* receive data from a destination */
  saveaddr (addr);
  len = recvfrom (s, (uint8_t *) data_buf, rlen, addr, &port);
  restaddr (addr);
  if (data_buf[0] == PING_REPLY)
    {
      PingReply.Type = data_buf[0];
      PingReply.Code = data_buf[1];
      PingReply.CheckSum = (data_buf[3] << 8) + data_buf[2];
      PingReply.ID = (data_buf[5] << 8) + data_buf[4];
      PingReply.SeqNum = (data_buf[7] << 8) + data_buf[6];

      for (i = 0; i < len - 8; i++)
	{
	  PingReply.Data[i] = data_buf[8 + i];
	}
      /* check Checksum of Ping Reply */
      tmp_checksum = ~checksum ((unsigned char *)&data_buf, len);
      if (tmp_checksum != 0xffff)
	printf ("tmp_checksum = %x\r\n", tmp_checksum);
      else
	{
// get total time for ping
#ifdef NOTIMER
	  printf
	    (" %d bytes from %d.%d.%d.%d: icmp_seq=%x\n", (rlen + 6),
	     (addr[0]), (addr[1]), (addr[2]), (addr[3]),
	     htons (PingReply.SeqNum));
#else
	  GetTime (&t2, &t1);
	  /*  Output the Destination IP and the size of the Ping Reply Message */
	  printf
	    (" %d bytes from %d.%d.%d.%d: icmp_seq=%x time=%ld ms \n",
	     (rlen + 6), (addr[0]), (addr[1]), (addr[2]), (addr[3]),
	     htons (PingReply.SeqNum), t2.t.time * 20);
#endif
	  /*  SET ping_reply_receiver to '1' and go out the while_loop (waitting for ping reply) */
	  ping_reply_received = 1;
	}
    }
  else if (data_buf[0] == PING_REQUEST)
    {
      PingReply.Code = data_buf[1];
      PingReply.Type = data_buf[2];
      PingReply.CheckSum = (data_buf[3] << 8) + data_buf[2];
      PingReply.ID = (data_buf[5] << 8) + data_buf[4];
      PingReply.SeqNum = (data_buf[7] << 8) + data_buf[6];

      for (i = 0; i < len - 8; i++)
	{
	  PingReply.Data[i] = data_buf[8 + i];
	}
      /* check Checksum of Ping Reply */
      tmp_checksum = PingReply.CheckSum;
      PingReply.CheckSum = 0;
      PingReply.CheckSum = htons (checksum ((unsigned char *)&PingReply, len));

      if (tmp_checksum != PingReply.CheckSum)
	{
	  printf (" \n CheckSum error %x should be %x \n",
		  (tmp_checksum), htons (PingReply.CheckSum));
	}

      /*  Output the Destination IP and the size of the Ping Reply Message */
      printf
	("Request from %d.%d.%d.%d  ID:%x SeqNum:%x  :data size %d bytes\r\n",
	 (addr[0]), (addr[1]), (addr[2]), (addr[3]), (PingReply.ID),
	 (PingReply.SeqNum), (rlen + 6));
      /*  SET ping_reply_receiver to '1' and go out the while_loop (waitting for ping reply) */
      ping_reply_received = 1;

    }
  else
    {
      printf (" Unkonwn msg. \n");
    }


  return 0;
}				// ping_reply



uint16_t
checksum (uint8_t * data_buf, uint16_t len)
{
  uint16_t sum, tsum, i, j;
  uint32_t lsum;

  j = len >> 1;
  lsum = 0;
  tsum = 0;
  for (i = 0; i < j; i++)
    {
      tsum = data_buf[i * 2];
      tsum = tsum << 8;
      tsum += data_buf[i * 2 + 1];
      lsum += tsum;
    }
  if (len % 2)
    {
      tsum = data_buf[i * 2];
      lsum += (tsum << 8);
    }
  sum = (uint16_t) lsum;
  sum = ~(sum + (lsum >> 16));
  return sum;

}


uint16_t
htons (uint16_t hostshort)
{
#if 1
  //#ifdef LITTLE_ENDIAN
  uint16_t netshort = 0;
  netshort = (hostshort & 0xFF) << 8;

  netshort |= ((hostshort >> 8) & 0xFF);
  return netshort;
#else
  return hostshort;
#endif
}


/*****************************************************************************************
	Function name: wait_1ms
	Input		:	cnt; Delay duration = cnt * 1m seconds
	Output	:	non
	Description
	: A delay function for waiting cnt*20m second. This function use wait_1us but the wait_1us
		has some error (not accurate). So if you want exact time delay, please use the Timer.
*****************************************************************************************/
void
wait_1ms (unsigned int cnt)
{
  unsigned int i;
#ifndef NOTIMER
  i = cnt / 25;			// 1 = .02 sec.  
  if (i < 1)
    i = 1;
  WaitTime (i);
#else
  for (i = 0; i < cnt; i++)
    spi_delay (500);
#endif
}
