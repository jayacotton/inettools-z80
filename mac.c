/* this program allows the user to enter the mac address
for there wiznet card.  It stores the mac address in a
config file, to be used by all of the inet programs */

#include <stdio.h>
#ifdef FRAM
#include "fram.h"
#include <stdlib.h>
#include "spi.h"
#endif
extern void snapmem();

FILE *f;

unsigned int mac[6];
void main(int argc,char *argv[])
{
#ifdef FRAM
	spi_init();
	if(argc >1){
		FramGetMac(mac);
		printf("%x.%x.%x.%x.%x.%x",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
		exit(0);
	}
#endif
	

	printf("Enter the MAC address as a set of 6 hex digits comma seperated\n");

	scanf("%x,%x,%x,%x,%x,%x",&mac[0],&mac[1],&mac[2],&mac[3],&mac[4],&mac[5]);
#ifdef FRAM
	FramSetMac(mac);
#else
	remove("mac.cfg");	
	f = fopen("mac.cfg","w");
	fprintf(f,"%d,%d,%d,%d,%d,%d",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	fclose(f);
#endif
}
