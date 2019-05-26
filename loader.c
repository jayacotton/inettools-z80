//******************************************************************************
//
//! \file loader.c
//! \brief driver loader Implemtation file.
//! \details This code will load a (multiple) driver into high
//! memory for use by the client (TPA) program.  It is designed
//! to work with RomWbW v2.9.1+ in Mode 2 interrupt.  TBD is Mode 1
//!
//! The driver binary has certain requirments to allow the loader
//! to work.  See driver struct below.
//!
//! The driver is built using macro and asm8080 to generate a .bin
//! file.
//!
//! \version 1.0
//! \date 5/10/2019
//! \par Revision history
//!   5/10/2019 Need to load in hex format
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
#include <string.h>
#include "loader.h"
#include "trace.h"

struct driver_file Head;
FILE *driv;

uint8_t l0;
uint8_t l1;

uint8_t
hex2 (FILE * fp)
{
  uint8_t ret;
  l0 = fgetc (fp) - 0x30;
  if (l0 > 9)
    l0 -= 7;
  l1 = fgetc (fp) - 0x30;
  if (l1 > 9)
    l1 -= 7;
  ret = (l0 << 4) | l1;
  return ret;
}

uint16_t l2;
uint8_t *
hex4 (FILE * fp)
{
  l2 = hex2 (fp);
  l2 = l2 << 8;
  l2 |= hex2 (fp);
  return ((void *) l2);
}

uint8_t *l_ptr;
uint8_t l_size;
uint8_t rz;
uint8_t
hextobin (uint8_t * ptr, FILE * fp)
{
  int ret_count;

  ret_count = 0;
  if (fgetc (fp) != ':')
    {
      printf ("Hex record alignment issue\n");
      exit (8);
    }
  // next 2 bytes are the record size, not counting the address
  l_size = hex2 (fp);
  if (l_size == 0)
    return 0;
  // next 4 bytes are the dest address
  l_ptr = hex4 (fp);
  // next 2 bytes are zero
  fgetc (fp);
  fgetc (fp);
  // now starts the data, count the binary bytes. 
  while (l_size--)
    {
// decide how to store the data
      if (ptr == 0)
	{
	  rz = hex2 (fp);
	  *l_ptr = rz;
// verify that the store happens
	  if (*l_ptr != rz)
	    {
	      printf ("Store ERROR\n");
	      exit (0);
	    }
	  l_ptr++;
	}
      if (ptr != 0)
	{
	  *ptr++ = hex2 (fp);
	}
      ret_count++;
    }
  // last 2 bytes are the checksum.  Ignore.              
  fgetc (fp);
  fgetc (fp);
  fgetc (fp);			// newline....
  return ret_count;
}

struct driver_head *head;

uint16_t *lnk;
uint16_t *Pbdos;

uint8_t
loader (uint8_t * file_name, uint8_t * desc)
{
  int rz;
	Pbdos = 6;
  driv = fopen (file_name, "r");
  if (driv == 0)
    {
      printf ("Can't open file %s\n", file_name);
      return 0;
    }
  printf ("Loading: %s\n", desc);
  hextobin ((void *) &Head, driv);
  head = (struct driver_head *) Head.dest_addr;
  lnk = (uint16_t)head->link;
  printf ("Target Addr 0x%04x, size %d\n", Head.dest_addr, Head.size);

// load the data into memory
  do
    {
      rz = hextobin (NULL, driv);
    }
  while (rz);

  fclose (driv);
// final step, rewire the bdos pointer to protect the driver.

printf("0x%04x 0x%04x 0x%04x\n",lnk,Pbdos,head);
	*lnk = *Pbdos;
	*Pbdos = *head;
  return 1;
}

uint16_t *
GetServ ()
{
  return head->ctcint;
}

uint32_t *
GetCount ()
{
  return head->counter;
}
