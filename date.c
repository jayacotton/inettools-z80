//******************************************************************************
//
//! \file date.c
//! \brief Get NTP date Implemtation file.
//! \details 
//!  Use NTP code to get the date time group and display it.
//!
//! \version 1.0
//! \date 5/2/2019
//! \par Revision history
//!	10/29/2019  Using RomWBW RTC support code.
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
#include "sysface.h"

char *weekday[7] = {"Sun", "Mon", "Tue", "Wed", "Thur", "Fri", "Sat" };

char *monthname[13] =
  { "NULL","Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct",
  "Nov", "Dec"
};

#define O_Year 0
#define O_Month 1
#define O_Date 2
#define O_Hour 3
#define O_Minute 4
#define O_Second 5

unsigned char *bcd_buffer;
dayofweek(year, month, day)
int     year;                                   /* Year, 1978 = 1978    */
int     month;                                  /* Month, January = 1   */
int     day;                                    /* Day of month, 1 = 1  */
/*
 * Return the day of the week on which this date falls: Sunday = 0.
 * Note, this routine is valid only for the Gregorian calender.
 */
{
        register int yearfactor;

        yearfactor = year + (month - 14)/12;
        return (( (13 * (month + 10 - (month + 10)/13*12) - 1)/5
                + day + 77 + 5 * (yearfactor % 100)/4
                + yearfactor / 400
                - yearfactor / 100 * 2) % 7);
}

void get_via_romwbw(unsigned char *sec,unsigned char * min, unsigned char * hr, 
unsigned char * wday, unsigned char *month,unsigned char *day,unsigned int *year)
{
	bcd_buffer = GetTOD();
	*sec = (bcd_buffer[O_Second] & 0x0f) + ((bcd_buffer[O_Second]>>4) & 0x0f) * 10;
	*min = (bcd_buffer[O_Minute] & 0x0f) + ((bcd_buffer[O_Minute]>>4) & 0x0f) * 10;
	*hr = (bcd_buffer[O_Hour] & 0x0f)+((bcd_buffer[O_Hour]>>4) & 0x0f) * 10;
	*month = (bcd_buffer[O_Month] & 0x0f)+ 
		((bcd_buffer[O_Month]>>4) & 0x0f) * 10;
	*day = (bcd_buffer[O_Date] & 0x0f)+ ((bcd_buffer[O_Date]>>4) & 0x0f) * 10;
	*year = (bcd_buffer[O_Year] & 0x0f) + ((bcd_buffer[O_Year]>>4) & 0x0f) * 10;
}

void main(){
uint8_t hr;
uint8_t min;
uint8_t sec;
uint8_t month;
uint8_t day;
uint16_t year;
uint8_t wday;

	get_via_romwbw(&sec,&min,&hr,&wday,&month,&day,&year);
#ifdef PDT
  printf ("%s %s %d %02d:%02d:%02d %d\n",
	weekday[dayofweek(year+2000,month,day)],
	monthname[month],
	day,hr,min,sec,year+2000);
#else
  printf ("%s %s %d %02d:%02d:%02d %d\n",
	weekday[dayofweek(year+2000,month,day)],
	monthname[month],
	day,hr,min,sec,year+2000);
#endif
  return 0;
}
