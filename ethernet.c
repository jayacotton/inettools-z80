
//******************************************************************************
//
//! \file ethernet.c
//! \brief This is an attempt to collect up the odd init functions.
//! \details API is home brew.  In a real OS, these things are found in the kernel 
//!
//! \version 1.0
//! \date 4/13/2019
//! \par Revision history
//!  	4/13/2019	Ver 1.0 created to support INET tools on CP/M
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

#include "types.h"
#include "socket.h"
#include "dhcp.h"
#include "w5500.h"
#include "ethernet.h"
#include <stdio.h>
#include <stdlib.h>
#include "spi.h"
#include "trace.h"

/*
Set up a dhcp session and get an IP address from the
server.   Optionally this code could skip the dhcp
session and assign hardwired addresses.
*/

#define SOCK_DHCP 6

#include <stdio.h>
extern unsigned char tfl;

unsigned char DHCP_buffer[2048];
extern struct wiz_NetInfo_t gWIZNETINFO;
extern unsigned char run_user_applications;
extern char w5500_present;
extern unsigned char w5500_getLinkStatus ();

void
DHCP_reset_static (unsigned char *mac)
{
  gWIZNETINFO.mac[0] = *mac++;
  gWIZNETINFO.mac[1] = *mac++;
  gWIZNETINFO.mac[2] = *mac++;
  gWIZNETINFO.mac[3] = *mac++;
  gWIZNETINFO.mac[4] = *mac++;
  gWIZNETINFO.mac[5] = *mac++;
  gWIZNETINFO.ip[0] = 0;
  gWIZNETINFO.ip[1] = 0;
  gWIZNETINFO.ip[2] = 0;
  gWIZNETINFO.ip[3] = 0;
  gWIZNETINFO.sn[0] = 255;
  gWIZNETINFO.sn[1] = 255;
  gWIZNETINFO.sn[2] = 255;
  gWIZNETINFO.sn[3] = 0;
  gWIZNETINFO.gw[0] = 0;
  gWIZNETINFO.gw[1] = 0;
  gWIZNETINFO.gw[2] = 0;
  gWIZNETINFO.gw[3] = 0;
  gWIZNETINFO.dns[0] = 0;
  gWIZNETINFO.dns[1] = 0;
  gWIZNETINFO.dns[2] = 0;
  gWIZNETINFO.dns[3] = 0;
  gWIZNETINFO.dhcp = NETINFO_DHCP;
}

//! Ethernet_begin, is called by the user level code to get things started.  It will
//! init the spi bus, dhcp, get an IP address, setup the mac address.
unsigned int
Ethernet_begin (unsigned char *mac)
{
  unsigned char my_dhcp_retry;
int i;
TRACE("");
  spi_init();	// since we alway run on spi bus
TRACE("");
  run_user_applications = 0;
  DHCP_reset_static (mac);
  TRACE ("DHCP_init");
  DHCP_init (SOCK_DHCP, DHCP_buffer);
TRACE("");
  while (run_user_applications == 0)
    {
      TRACE ("DHCP_run");
      switch ((i = DHCP_run ()))
	{
	case DHCP_IP_ASSIGN:
      TRACE ("DHCP_IP_ASSIGN");
	  break;
	case DHCP_IP_CHANGED:
      TRACE ("DHCP_IP_CHANGED");
	  break;
	case DHCP_IP_LEASED:
      TRACE ("DHCP_IP_LEASED");
	  run_user_applications = 1;
	  getIPfromDHCP (gWIZNETINFO.ip);
	  getGWfromDHCP (gWIZNETINFO.gw);
	  getSNfromDHCP (gWIZNETINFO.dns);
	  getDNSfromDHCP (gWIZNETINFO.dns);
	  break;
	case DHCP_FAILED:
      TRACE ("DHCP_FAILED");
	  my_dhcp_retry++;
	  if (my_dhcp_retry > MY_MAX_DHCP_RETRY)
	    {
	      DHCP_reset_static (mac);
	      DHCP_stop ();	// if restart, recall DHCP_init()
	      printf (">> DHCP %d Failed\r\n", my_dhcp_retry);
	      //Net_Conf ();
	      Display_Net_Conf ();	// print out static netinfo 
	      my_dhcp_retry = 0;
	    }
	  break;
	case DHCP_RUNNING:
	TRACE("DHCP_RUNNING");
	break;
	default:
	//printf("DHCP_UNKNOWN: %d\n",i);
	  break;
	}
    }

}

//! Ethernet_hardwareStatus, 
//! Return the hardware status of the network interface
//! board
unsigned int
Ethernet_hardwareStatus ()
{
  if (w5500_present == 0)
    {
      return EthernetNoHardware;
    }
  return 0;
}

//! Ethernet_linkStatus,
//! Return the link status of the network interface.
unsigned int
Ethernet_linkStatus ()
{
  unsigned char stat;
  stat = w5500_getLinkStatus ();
  if (stat == LINK_OFF)
    return LinkOFF;
  if (stat == LINK_ON)
    return LinkON;
  return -1;
}

//! Eternet_localIP, get my IP address
/*
Return the local IP address from the chip
*/
void
Ethernet_localIP (unsigned char *ip)
{
  getIPfromDHCP (ip);
}

//! Ethernet_localGW, get my gateway address
void
Ethernet_localGW (unsigned char *ip)
{
  getGWfromDHCP (ip);
}
//! Ethernet_localSN, Get my network mask
void
Ethernet_localSN (unsigned char *ip)
{
  getSNfromDHCP (ip);
}
//! Ethernet_localDNS, Get my DNS server address
void
Ethernet_localDNS (unsigned char *ip)
{
  getDNSfromDHCP (ip);
}
//! Display_Net_Conf, print a bunch of addresses
void
Display_Net_Conf ()
{
  printf ("MAC: %x.%x.%x.%x.%x.%x\n",
	  gWIZNETINFO.mac[0], gWIZNETINFO.mac[1],
	  gWIZNETINFO.mac[2], gWIZNETINFO.mac[3],
	  gWIZNETINFO.mac[4], gWIZNETINFO.mac[5]);
  printf ("IP:  %d.%d.%d.%d\n",
	  gWIZNETINFO.ip[0], gWIZNETINFO.ip[1],
	  gWIZNETINFO.ip[2], gWIZNETINFO.ip[3]);
  printf ("MSK: %d.%d.%d.%d\n",
	  gWIZNETINFO.sn[0], gWIZNETINFO.sn[1],
	  gWIZNETINFO.sn[2], gWIZNETINFO.sn[3]);
  printf ("GW:  %d.%d.%d.%d\n",
	  gWIZNETINFO.gw[0], gWIZNETINFO.gw[1],
	  gWIZNETINFO.gw[2], gWIZNETINFO.gw[3]);
  printf ("DNS: %d.%d.%d.%d\n",
	  gWIZNETINFO.dns[0], gWIZNETINFO.dns[1],
	  gWIZNETINFO.dns[2], gWIZNETINFO.dns[3]);
}
