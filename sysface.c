/* this code interfaces to RomWBW and exposes the system calls 
*/

unsigned int count;
unsigned int rtccount()
{
#asm
	ld	bc,$f820	; get rtc count 0 = none
	rst	08
	cp	0
	jp	z,_Cnt		;
	jp	_rtcDone		;
._Cnt	ld	a,e
._rtcDone ld	(_count),a

#endasm
	return (count);
}
char *types[4] =
{"DS1322","BQ4845P","SIMH","INT TIMER"};
unsigned char *rtctype()
{
#asm
	ld	bc,$2800	; get rtc device type
	rst	08
	ld	a,d
	ld	(_count),a
#endasm
	return (types[count>>4]	);
}
