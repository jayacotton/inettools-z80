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
	secs  = time;
	if(days > 1)
	printf("up %d days %d:%d:%d\n",days,hours,mins,secs);
	else
	printf("up %d day %d:%d:%d\n",days,hours,mins,secs);

}
main()
{
long time ;

	time = GetUptime(1);
	PrintAsTime(time);
	return 0;
}
