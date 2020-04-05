/*
traceing functions for debugging c programs.

the snap library is an integral part of the debugging tool box.

	int snap(&location, &what, amount, flag, &string)

	location is an address, this just prints an address of the place where you are calling
	from or maybe the address of the routine calling snap.
	what is the address of the buffer or memmory location to dump.
	amount is the number of bytes to dump.
	flag, 0 is dump in hex, 1 is dump in ascii text, 2 dump in decimal.
	string is a pointer to a string message that heads the snap.

	TRACE("msg");  prints the file name, line number and message.

	TVAL("%<fmt> msg",variable);  this will format and print a variable.


These macros can be left in place in the code and when the -DDEBUG flag is not present
they will be compiled out of the code stream.


*/

int snapmem(int *, int *, int, int, char *);

#ifdef DEBUG
#define SNAP(addr,size,flag) { char snapbuff[50]; \
	sprintf(snapbuff,"file %s: line %d",__FILE__,__LINE__); \
	snapmem(addr,addr,size,flag,snapbuff); \
	}; 
#define TRACE(msg) printf("%s %d %s\n",__FILE__,__LINE__,msg);
#define TRACEFL(msg) {if(tfl)printf("%s %d %s\n",__FILE__,__LINE__,msg);}
#define TVAL(fmt,var) printf(fmt,var);
#define LED(v) {outp(0,v);}
#define LEDN(v) {outp(0,1<<v);}
#else
#define TRACE(msg)
#define TRACEFL(msg)
#define TVAL(fmt,var)
#define SNAP(addr,size,flag)
#define LEDN(V)
#endif
