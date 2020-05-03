/* get and print the uptime value */

#include <stdio.h>
#include <stdlib.h>
#include "sysface.h"

unsigned char days;
unsigned char hours;
unsigned char mins;
unsigned char secs;

void PrintAsTime(long time)
{
	days  = time/86400;
	time -= (long)days*86400;
	hours = time/3600;
	time -= (long)hours * 3600;
	mins  = time/60;
	time -= (long)mins*60;
	printf("up %02d days %02d:%02d\n",days,hours,mins);

}
main()
{
long time ;

	time = GetUptime(0);
	PrintAsTime(time);
}
