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
//!	04/14/2019 The first version of the program.
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
#include "ethernet.h"
#include "wizchip_conf.h"
#include "trace.h"

wiz_NetInfo gWIZNETINFO;
unsigned char run_user_applications;

unsigned char mac[6] = {0x98,0x76,0xb6,0x11,0x00,0xc4};
unsigned char ip[4];

void main()
{

	if(Ethernet_begin(mac) == 0){
TRACE("error checking ");
		if(Ethernet_hardwareStatus() == EthernetNoHardware)
			printf("Can't find the ethernet h/w\n");
		if(Ethernet_linkStatus() == LinkOFF)
			printf("Plug in the cable\n");
		exit(0);
	}
TRACE("Seems to be working");
	Ethernet_localIP(ip);
	printf("inet %d.%d.%d.%d ",ip[0],ip[1],ip[2],ip[3]);
	Ethernet_localSN(ip);
	printf("netmask %d.%d.%d.%d\n",ip[0],ip[1],ip[2],ip[3]);
	Ethernet_localDNS(ip);
	printf("dns server %d.%d.%d.%d\n",ip[0],ip[1],ip[2],ip[3]);
	printf("ether %02x.%02x.%02x.%02x.%02x.%02x",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
}
