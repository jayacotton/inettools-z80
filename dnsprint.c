//******************************************************************************
//
//! \file dnsprint.c
//! \brief DIG implementation file.
//! \details This code takes a host name string and finds the 
//! internet address for it.   This is the simplest version of
//! DIG that is possible.  The ISC version is very large and 
//! complex.  We are not using nslookup, we are simply asking
//! the DNS server for an address translation.
//!
//! \version 1.0
//! \date 4/13/2019
//! \par Revision history
//!	04/13/2019 First version of the code.
//!
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
#include "dns.h"
#include "string.h"

#define SOCK_DNS 6  // same as SOCK_DHCP

wiz_NetInfo gWIZNETINFO;
unsigned char run_user_applications;

char DNS_buffer[2048];

unsigned char mac[6] = {0x98,0x76,0xb6,0x11,0x00,0xc4};
unsigned char ip[4];
unsigned char dns[4];
unsigned char remip[4];
unsigned char dnsname[80];
void main(int argc, char *argv[])
{
	if(argc > 1){
		strcat(dnsname,argv[1]);
	}else{
		strcat(dnsname,"www.nasa.gov");
	}

	if(Ethernet_begin(mac) == 0){
		if(Ethernet_hardwareStatus() == EthernetNoHardware)
			printf("Can't find the ethernet h/w\n");
		if(Ethernet_linkStatus() == LinkOFF)
			printf("Plug in the cable\n");
		exit(0);
	}
	Ethernet_localIP(ip);
	Ethernet_localDNS(dns);
	DNS_init(SOCK_DNS,DNS_buffer); // share the data buffer ??
	printf("%s :",dnsname);
	if(DNS_run(dns,dnsname,remip)==1)
	{
		printf("%d.%d.%d.%d\n",
		remip[0],remip[1],remip[2],remip[3]);
	}else{
		printf("error\n");
	}
	
}
