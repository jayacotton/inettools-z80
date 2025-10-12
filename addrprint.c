//******************************************************************************
//
//! \file addrprint.c
//! \brief ifconfig Implemtation file.
//! \details IFCONFIG is used to print out your client ip address
//! and a fiew other interesting ethernet numbers.  This version 
//! is as simple as it can get.  There are no options.
//!
//! \version 1.0
//! \date 4/13/2019
//! \par Revision history
//!     04/14/2019 The first version of the program.
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
#include <unistd.h>
#include "ethernet.h"
#include "wizchip_conf.h"
#include "trace.h"
#include "malloc.h"

#include "inet.h"

extern long heap ();

wiz_NetInfo gWIZNETINFO;
unsigned char run_user_applications;

unsigned char mac[6];

unsigned char ip[4];
unsigned char nm[4];
unsigned char dns[4];
unsigned char gw[4];

unsigned int
DeltaTimeINET ()
{
  return (InetGetEpoch () - InetGetTofU ());
}

void
help ()
{
    printf ("-S get setup from dhcp server\n");
  printf ("-I <nnn.nnn.nnn.nnn> set ip address\n");
  printf ("-N <nnn.nnn.nnn.nnn> set net mask \n");
  printf ("-D <nnn.nnn.nnn.nnn> set dms server address\n");
  printf ("-G <nnn.nnn.nnn.nnn> set gateway address\n");
  printf ("-L prints the current settings\n");
}

unsigned char
nib2bin (char ch)
{				// ch is upper case
  ch -= 0x30;
  if (ch >= 10)
    {				// 17 or more
      ch -= 7;
    }
  return (ch);
}

// convert a string to and ip address string in binary
// not much checking for the quality of the data
void
parse_ip (unsigned char proto[], char *str)
{				// str => xx:xx:xx:xx\0
  int i;
char ch;

	sscanf(str,"%d.%d.%d.%d",&proto[0],&proto[1],&proto[2],&proto[3]);
}

void
main (int argc, char *argv[])
{
  unsigned char opt;
  unsigned char sflag, lflag;

  lflag = sflag = 0;

  while ((opt = getopt (argc, argv, "SI:N:D:L:G:HL")) != -1)
    {
      switch (opt)
	{
	case 'S':		// set from system dhcp
	  sflag++;
	  lflag++;
	  break;
	case 'I':		// set ip from console
	  lflag++;
	  parse_ip (ip, optarg);
	  InetSetIP (ip);
	printf("%d.%d.%d.%d\n",ip[0],ip[1],ip[2],ip[3]);
	  break;
	case 'N':		// set netmask
	  lflag++;
	  parse_ip (nm, optarg);
	  InetSetMask (nm);
	printf("%d.%d.%d.%d\n",nm[0],nm[1],nm[2],nm[3]);
	  break;
	case 'D':		// set dns server
	  lflag++;
	  parse_ip (dns, optarg);
	  InetSetDns (dns);
	printf("%d.%d.%d.%d\n",dns[0],dns[1],dns[2],dns[3]);
	  break;
	case 'G':		// set gateway
	  lflag++;
	  parse_ip (gw, optarg);
	  InetSetGate (gw);
	printf("%d.%d.%d.%d\n",gw[0],gw[1],gw[2],gw[3]);
	  break;
	case 'H':		// print help message
	  help ();
	  exit (0);
	  break;
	case 'L':
	default:		// just list the data
	  lflag++;
	  break;
	}
    }
  if (sflag)
    {
      InetGetMac (mac);
      InetSetTofU (InetGetEpoch ());
      if (Ethernet_begin (mac) == 0)
	{
	  if (Ethernet_hardwareStatus () == EthernetNoHardware)
	    printf ("Can't find the ethernet h/w\n");
	  if (Ethernet_linkStatus () == LinkOFF)
	    printf ("Plug in the cable\n");
	}
      Ethernet_localIP (ip);
      InetSetIP (ip);
      Ethernet_localSN (ip);
      InetSetMask (ip);
      Ethernet_localDNS (ip);
      InetSetDns (ip);
      Ethernet_localGW (ip);
      InetSetGate (ip);
    }
  if (DeltaTimeINET () > 1440)
    {
      InetSetTofU (InetGetEpoch ());
      if (Ethernet_begin (mac) == 0)
	{
	  if (Ethernet_hardwareStatus () == EthernetNoHardware)
	    printf ("Can't find the ethernet h/w\n");
	  if (Ethernet_linkStatus () == LinkOFF)
	    printf ("Plug in the cable\n");
	}
    }
  if (lflag)
    {
      InetGetIP (ip);
      printf ("inet %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
      InetGetMask (ip);
      printf ("netmask %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
      InetGetDns (ip);
      printf ("dns server %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
      InetGetGate (ip);
      printf ("Default Gateway %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
      InetGetMac (mac);
      printf ("ether %x:%x:%x:%x:%x:%x", (unsigned short) mac[0],
	      (unsigned short) mac[1], (unsigned short) mac[2],
	      (unsigned short) mac[3], (unsigned short) mac[4],
	      (unsigned short) mac[5]);
    }
}
