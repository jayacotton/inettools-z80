/* read and set the time zone field in the nvram 

NOTE:  at this time no checking is done on the time zone
information.  You type it we store it.
*/

#include <stdio.h>
#include <stdlib.h>
#include "sysface.h"

unsigned int zone_off;

void main(int argc, char * argv[])
{
	zone_off = atoi(argv[1]);
	printf("You set GMT-%d\n",zone_off);	
	SetTZ(zone_off);
}
