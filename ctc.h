//******************************************************************************
//
//! \file ctc.h 
//! \brief CTC driver header file.
//! \details This is a DIY homebrew driver.
//!
//! \version 1.0
//! \date 5/9/2019
//! \par Revision history
//!	5/9/2019 update to work with newer version of RomWBW
//!	4/20/2019  first version of code.
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

#ifndef _CTC_
#define _CTC_
#ifndef NOHBIOS
#include "hbios.h"
#endif
extern int ctc_vet();
extern unsigned int *ctc_gaddr(int *);
extern void ctc_paddr(int *, int *);
extern void ctc_vector();
extern void ctc_exit();
extern unsigned int GetVer();
extern unsigned int GetDos();
extern void ctc_interupt();
extern void ctc_init();
extern uint32_t *counter;

#define _CTC_BASE 0xa0
#define _CTC_0	_CTC_BASE + 0
#define _CTC_1	_CTC_BASE + 1
#define _CTC_2	_CTC_BASE + 2
#define _CTC_3	_CTC_BASE + 3

#define _SIO_STATUS 0x80

#endif
