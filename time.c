/*

Time functions for RC2014 running RomWBW with a KIO chip.

The CTC is programed to produce interupts at 20ms per tick.

The time functions will not provide time if the KIO is missing
or there is no other tick source.  It will provide 0 always.

Initial code R0.1  10/24/2019  Jay Cotton

*/

/*  MIT  */

/* The functions:

	Get the time register from the kernel, and store it in
	TICKSTORE out.  This is used later for computing how long
	we where doing something.

	void settime(TICKSTORE *out);	

	Get the time delta from the kernel.  This will read the
	current time tick from the kernel and subtract TICKSTORE in
	from it.  The result time tick is returned in TICKSTORE out.

	void gettime(TICKSTORE *out, TICKSTORE *in);	

	Wait in this call for time TICKS.  The limit is a 16 bit
	number of ticks.  On the test system that is about 1300 
	seconds max.
	
	void waittime(unsigned int time);

	This function will format the time tick into an ascii string
	for printing or storing in a string buffer.  

	Formats are:  
		0 - Raw decimal convertion.
		1 - Time in milli seconds.
		2 - Time in hr:min:sec:milli

	Users should make certain the output buffer is large enough
	for the output string.

	void formattime(char format, char *out, TICKSTORE *in);

*/

#include <stdio.h>
#include <stdlib.h>
#include "time.h"
#include <math.h>

unsigned int hl,de;

struct TICKSTORE test;

void gettick(struct TICKSTORE *ticks)
{
#asm
	ld	b,k_sysget
	ld	c,k_gettimer
	rst	08
; de:hl = timer value
	ld	(_hl),hl
	ld	(_de),de	
#endasm
	ticks->t.de_hl[0] = hl;
	ticks->t.de_hl[1] = de;	
}
void SetTime(struct TICKSTORE *out)	
{
	gettick(out);
}
void GetTime(struct TICKSTORE *out, struct TICKSTORE *in)
{
struct TICKSTORE tmp;

	gettick(&tmp);
	out->t.time = tmp.t.time - in->t.time;
}
void WaitTime(unsigned int time)
{
struct TICKSTORE tmp,tmp1;
	SetTime(&tmp);
	while(1){
		GetTime(&tmp1,&tmp);
		if(tmp1.t.time >= time)
			return;
	}
}
void FormatTime(char format, char *out, struct TICKSTORE *in)
{
;float res;
unsigned long result;

	switch(format){
		case FORMAT_RAW:
                        result = in->t.time;
                        sprintf(out,"%ld",result);
			break;
		case FORMAT_MILLI:
		        ;res = (float)in->t.time * (float)TICK;
			result = in->t.time;
			sprintf(out,"%ld",result);
			break;
		case FORMAT_TIME:
			break;
	}
}
