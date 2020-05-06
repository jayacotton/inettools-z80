/* read and set the time zone field in the nvram 

NOTE:  at this time no checking is done on the time zone
information.  You type it we store it.
*/

#include <stdio.h>
#include <stdlib.h>
#include "sysface.h"

long zone_off;

void
main (int argc, char *argv[])
{
  if (argc <= 1)
    {
      zone_off = GetTZ ();
	if(zone_off >0) 
		printf("Your offset is UTC+%ld seconds\n",zone_off);
	else
		printf("Your offset is UTC%ld seconds\n",zone_off);
    }
  else
    {
      zone_off = atoi (argv[1]);
      printf ("You set UTC %ld seconds\n", zone_off);
      SetTZ (zone_off);
    }
}
