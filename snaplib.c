//******************************************************************************
//
//! \file snaplib.c
//! \brief Memory snapshot Implemtation file.
//! \details 
//!	
//!        options include:  show ascii text
//!                          show dec values TBD
//!                          show hex values
//!
//!
//!        char * here     Address where snap was requested.
//!        char * addr     Address to dump.
//!        int     size    Amount of bytes to show.
//!        int     flags   TBD
//!
//! \version 1.0
//! \date	Thur May 02 11:22:44 PDT 2019
//! \par Revision history
//! 	UNK First version published at sourceforge.com a very long time ago.
//!	5/2/19 this version is mostly like what I put on sourceforge.com.
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
#include <ctype.h>
#define SnapAscii 1
#define SnapHex 4
#define SnapDec 8
#define SnapTight 0x20

int snapmem(here,addr,size,flags,msg)
unsigned char *here;
unsigned char *addr;
int size;
int flags;
char *msg;
{
int i;
int j;
unsigned char *where;
int ascii;
int dism;
int dec;

	where = addr;
	ascii = 0;
	dism = 0;
	dec = 0;

	printf("Snapshot taken at: %04x %s\n",here,msg);
	if(flags & SnapAscii){
		ascii++;
	}
	/* raw snap */
	if(flags & SnapDec){
	}
	/* snap hex is defalut */
	j = 0;
	for(i=0;i<size;i++){
		if(j==0) printf("0x%04x: ",where);
		if(ascii){
			if(SnapTight){
				if(isalpha(*where)){
					printf(" %c ",*where++);
				}else{
					printf("%02x ",*where++);
				}
			}
			else {
				if(isalpha(*where)){
					printf("   %c ",*where++);
				}else{
					printf("0x%02x ",*where++);
				}
			}
		}else if(dec) {
		}else {
			printf("0x%02x ",*where++);
		}
		j++;
		if(j==12){
			printf("\n");
			j = 0;
		}
	}
	printf("\n");
	return 0;
}

