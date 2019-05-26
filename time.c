//******************************************************************************
//
//! \file time.c
//! \brief Time functsions Implemtation file.
//! \details API is pulled out of thin air.
//!
//! \version 1.0
//! \date 4/20/2019
//! \par Revision history
//!	4/20/2019 First version
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

#define K_CODE

#include <stdio.h>
#include "time.h"
#include "ctc.h"
#include "loader.h"

//! init the time code structs
extern struct driver_file Head;
extern struct driver_head *head;

void time_init()
{
#asm
	di
#endasm
	callback0->type = EMPTY;
	callback1->type = EMPTY;
	callback2->type = EMPTY;
#asm
	ei
#endasm
}

//! get a vacant timer slot.
struct callback * time_get_slot()
{
	if(callback0->type == EMPTY)
		return (callback0);
	if(callback1->type == EMPTY)
		return (callback1);
	if(callback2->type == EMPTY)
		return (callback2);
	return 0;
}

//! wait here for s time.  Anywhere from .01 sec to 655 seconds.
void time_wait_01(unsigned int s)
{
struct callback *p;

	p = time_get_slot();
	if(p == 0){
		printf("no timers left\n");
		return;
	}
#asm
	di
#endasm
	p->type = ONESHOT;
	p->tcount = s;
	p->fn = 0;
#asm
	ei
#endasm
	while(p->tcount);
}
//! wait for apx 1 second increments
void time_wait(unsigned int s)
{
	time_wait_01(s*50);	// 1 second apx
}
