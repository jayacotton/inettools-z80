//******************************************************************************
//
//! \file ntp.c
//! \brief NTP Implemtation file.
//! \details This code is mostly David Lettier's ntp.c file with a lot
//! modifications for z80/rc2014/z88dk/wiznet.  Included is code from 
//! GNU offtime for the time converion, and for setting the RTC I used
//! code frm rtc.asm by Andrew Lynch et.al.
//!
//! \version 2.0
//! \date 4/23/2019
//! \par Revision history
//!     4/23/2019       First version 
//1     4/30/2019       The time convertion has a at 12 GMT with the date.
//!     5/1/2019        Added RTC loading support.  BUGGY
//!     10/29/2019      RTC support in RomWBW, removed rtci2c and ctc code.
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
/* includeing the GNU license since I based much of the time convertion on
 the gnu offtime.c function */
/* Copyright (C) 1991-2019 Free Software Foundation, Inc.
	   This file is part of the GNU C Library.
	
	   The GNU C Library is free software; you can redistribute it and/or
	   modify it under the terms of the GNU Lesser General Public
	   License as published by the Free Software Foundation; either
	   version 2.1 of the License, or (at your option) any later version.
	
	   The GNU C Library is distributed in the hope that it will be useful,
	   but WITHOUT ANY WARRANTY; without even the implied warranty of
	   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	   Lesser General Public License for more details.
	
	   You should have received a copy of the GNU Lesser General Public
	   License along with the GNU C Library; if not, see
	   <http://www.gnu.org/licenses/>.  */
/*
 *
 * (C) 2014 David Lettier.
 *
 * http://www.lettier.com/
 *
 * NTP client.
 *
 * Compiled with gcc version 4.7.2 20121109 (Red Hat 4.7.2-8) (GCC).
 *
 * Tested on Linux 3.8.11-200.fc18.x86_64 #1 SMP Wed May 1 19:44:27 UTC 2013 x86_64 x86_64 x86_64 GNU/Linux.
 *
 * To compile: $ gcc main.c -o ntpClient.out
 *
 * Usage: $ ./ntpClient.out
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include "socket.h"
#include "netdb.h"
#include <ctype.h>
#include "ethernet.h"
#include "dns.h"
#include "trace.h"
#include "spi.h"
#include "sysface.h"

/* time since 1970 */
#ifdef UNIXEPOCH
#define NTP_TIMESTAMP_DELTA 2208988800
#define NTP_TIMESTAMP_DELTA_HEX 0x83aa7e80
#else
/* Y2KEPOCH */
#define NTP_TIMESTAMP_DELTA 3124137601
#define NTP_TIMESTAMP_DELTA_HEX 0xBA368E81
#endif
#define SECS_PER_DAY 86400
#define SECS_PER_HOUR 3600

extern unsigned int RTCCount ();
extern unsigned char *RTCType ();

struct wiz_NetInfo_t gWIZNETINFO;
unsigned char mac[6];

char DNS_buffer[256];
extern unsigned char HostAddr[4];
unsigned char run_user_applications;

#define O_Year 0
#define O_Month 1
#define O_Date 2
#define O_Hour 3
#define O_Minute 4
#define O_Second 5
unsigned char bcd_buffer[8];
  // Structure that defines the 48 byte NTP packet protocol.
struct ntp_packet
{
  uint8_t li_vn_mode;		// Eight bits. li, vn, and mode.
  // li.   Two bits.   Leap indicator.
  // vn.   Three bits. Version number of the protocol.
  // mode. Three bits. Client will pick mode 3 for client.
  uint8_t stratum;		// Eight bits. Stratum level of the local clock.
  uint8_t poll;			// Eight bits. Maximum interval between successive messages.
  uint8_t precision;		// Eight bits. Precision of the local clock.
  uint32_t rootDelay;		// 32 bits. Total round trip delay time.
  uint32_t rootDispersion;	// 32 bits. Max error aloud from primary clock source.
  uint32_t refId;		// 32 bits. Reference clock identifier.
  uint32_t refTm_s;		// 64 bits. Reference time-stamp seconds.
  uint32_t refTm_f;
  uint32_t origTm_s;		// 64 bits. Originate time-stamp seconds.
  uint32_t origTm_f;
  uint32_t rcTm_s;		// 64 bits. Received time-stamp seconds.
  uint32_t rcTm_f;
  uint32_t txTm_s;		// 64 bits and the most important field the client cares 
  uint32_t txTm_f;
};				// Total: 384 bits or 48 bytes.

struct ntp_packet packet;
void
error (char *msg, int val)
{
  printf ("Error %d: %s\n", val, msg);	// Print the error message to stderr.

  exit (0);			// Quit the process.
}

uint32_t local_v;
union
{
  uint32_t l;
  uint8_t c[4];
} un;

uint16_t year;
uint8_t month;
uint32_t rem;
uint16_t guess;
unsigned char buffer1[100];


// there seems to be a bug in ntohl on z88dk so
// I have this version until the bug is resolved.
uint32_t
my_ntohl (uint32_t v)
{
  uint8_t p[4];
  int i;

  un.l = v;
  for (i = 0; i < 4; i++)
    {
      p[i] = un.c[3 - i];
    }
  for (i = 0; i < 4; i++)
    {
      un.c[i] = p[i];
    }
  return un.l;

}

unsigned char
int2bcd (unsigned char input)
{
  unsigned char high = 0;

  while (input >= 10)
    {
      high++;
      input -= 10;
    }
  return (high << 4) | input;
}

/* convert this to a 6 byte bcd encoded buffer */
void
set_via_romwbw (uint8_t seconds, uint8_t minutes, uint8_t hour, uint8_t day,
		uint8_t month, uint8_t wday, uint16_t year)
{
  if (year > 99)
    bcd_buffer[O_Year] = year - 2000;	/*assume this century */
  else
    bcd_buffer[O_Year] = year;	/* assume this century */
  bcd_buffer[O_Year] = int2bcd (bcd_buffer[O_Year]);
  bcd_buffer[O_Month] = int2bcd (month);
  bcd_buffer[O_Date] = int2bcd (day);
  bcd_buffer[O_Hour] = int2bcd (hour);
  bcd_buffer[O_Minute] = int2bcd (minutes);
  bcd_buffer[O_Second] = int2bcd (seconds);
  SetTOD (bcd_buffer);
}

dayofweek (year, month, day)
     int year;			/* Year, 1978 = 1978    */
     int month;			/* Month, January = 1   */
     int day;			/* Day of month, 1 = 1  */
/*
 * Return the day of the week on which this date falls: Sunday = 0.
 * Note, this routine is valid only for the Gregorian calender.
 */
{
  register int yearfactor;

  yearfactor = year + (month - 14) / 12;
  return (((13 * (month + 10 - (month + 10) / 13 * 12) - 1) / 5
	   + day + 77 + 5 * (yearfactor % 100) / 4
	   + yearfactor / 400 - yearfactor / 100 * 2) % 7);
}

int portno = 123;		// NTP UDP port number.
char host_name[256];
#pragma output REGISTER_SP = 0xc000
int
main (int argc, char *argv[])
{
  int sockfd;
  int n;			// Socket file descriptor and the n 
  // return result from writing/reading from the socket.
  int i;
  int rn;
  unsigned char *p;
  uint16_t *lp;
  unsigned char destip[4];
  unsigned int destport;
  int wait;
  time_t tvec;
  struct tm *tp;

/* this could be inproved to deal with int timer clock on rc2014 */

  if (argc == 2)
    strcpy(host_name , argv[1]);
	else
    strcpy(host_name, "time.google.com");

  if (strcmp (RTCType (), "DS1302") != 0)
    {
      printf ("Only works with DS1322\n");
      exit (0);
    }

  memset (&packet, 0, sizeof (struct ntp_packet));

  // Set the first byte's bits to 00,011,011 for li = 0, vn = 3, and mode = 3. 
  // vn = 3 is very important, since we don't want to handle longlong
  // divides 

  packet.li_vn_mode = 27;
  packet.stratum = 0;
  packet.poll = 6;
  packet.precision = 0xec;
  packet.rootDelay = 49;
  packet.rootDispersion = 0x4e;
  packet.refId = 49;
  packet.refTm_s = 52;

  // Create a UDP socket, convert the host-name to an IP address, set the port number,
  // connect to the server, send the packet, and then read in the return packet.

  memcpy (gWIZNETINFO.mac, mac, 6);
  if (Ethernet_begin (mac) == 0)
    {
      if (Ethernet_hardwareStatus () == EthernetNoHardware)
	printf ("Can't find the ethernet h/w\n");
      else if (Ethernet_linkStatus () == LinkOFF)
	printf ("Plug in the cable\n");
      else
	printf ("unknown error\n");
      exit (0);
    }
  Ethernet_localIP (gWIZNETINFO.ip);
  Ethernet_localDNS (gWIZNETINFO.dns);
  DNS_init (SOCK_DNS, DNS_buffer);	// share the data buffer ??
  if (DNS_run (gWIZNETINFO.dns, host_name, HostAddr) == 0)
    {
      printf ("dns error\n");
      exit (0);
    }

  sockfd = socket (1, Sn_MR_UDP, portno, 0);

  if (sockfd < 0)
    error ("Socket: ", sockfd);

  // Send it the NTP packet it wants. If n == -1, it failed.

  n = sendto (sockfd, (char *) &packet,
	      sizeof (struct ntp_packet), HostAddr, portno);

  if (n < 0)
    error ("ERROR writing to socket", n);

  // Wait and receive the packet back from the server. If n == -1, it failed.

  while (getSn_RX_RSR (1) < sizeof (struct ntp_packet));
  n = recvfrom (1, (char *) &packet,
		sizeof (struct ntp_packet), destip, &destport);
  if (n == 0)
    error ("ERROR: no data from server", n);
  if (n < 0)
    error ("ERROR reading from socket", n);

// swap the bytes around to match our machine
  un.l = my_ntohl (packet.txTm_s);
  un.l = un.l - (uint32_t) NTP_TIMESTAMP_DELTA_HEX;
  EpochSet (un.l);
  tvec = EpochGet ();
  tp = localtime (&tvec);
  year = tp->tm_year + 1900;
  month = tp->tm_mon + 1;
  set_via_romwbw (tp->tm_sec,
		  tp->tm_min,
		  tp->tm_hour,
		  tp->tm_mday,
		  month, dayofweek (year, month, tp->tm_mday), year);
  return 0;
}
