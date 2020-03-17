
//******************************************************************************
//
//! \file ethernet.h
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
#ifndef _ERTHERNET_
#define _ERTHERNET_

extern unsigned int Ethernet_begin(unsigned char *);
extern unsigned int Ethernet_hardwareStatus();
extern unsigned int Ethernet_linkStatus();
extern void Ethernet_localIP(unsigned char *);
extern void Ethernet_localSN(unsigned char *);
//extern void Ethernet_localGW(unsigned char *);
extern void Ethernet_localDNS(unsigned char *);
extern void Ethernet_localIP(unsigned char *);
extern void Display_Net_Conf();




#define EthernetNoHardware 1
#define LinkOFF 10
#define LinkON 11

#define MY_MAX_DHCP_RETRY 2
#endif
