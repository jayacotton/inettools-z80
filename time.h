/* my version of time.h */

#ifndef _TIME_
#define _TIME_
#define CPM 	1
#define ZSDOS 	2

enum {
	EMPTY = 0,	// time slot is empty
	RECURING,	// at zero count, reset the time
	ONESHOT,	// at zero count, free the time slot
	CALLME,		// at zero cound, do a callback
	UPCOUNT,	// count up
	DOWNCOUNT	// count down
};

struct callback{
unsigned char 	type;	// counter type
unsigned int	tcount; // terminal count
void 		(*fn)();// callback function
};

#ifdef K_CODE
// This gets allocated in the time.c file
struct callback *callback0;
struct callback *callback1;
struct callback *callback2;
#else
extern struct callback *callback0;
extern struct callback *callback1;
extern struct callback *callback2;
extern void time_init();
extern void time_wait(int);

#endif
#define MAX_TIMERS 3
#endif
