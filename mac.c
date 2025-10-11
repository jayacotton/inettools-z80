/* this program allows the user to enter the mac address
for there wiznet card.  It stores the mac address in a
config file, to be used by all of the inet programs */

#include <stdio.h>
#include "inet.h"
#include <stdlib.h>
#include "spi.h"
#include "hexprint.h"
#include <string.h>

#define VERSION "v1.1"

#define VERSION "v1.1"
extern void snapmem();

FILE *f;

unsigned char mac[6];
void main(int argc,char *argv[])
{
#if defined(FRAM) || defined (DISK) || defined (NVRAM)
	if(argc >1){
		if(strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
			printf("MAC Address Tool %s\n", VERSION);
			exit(0);
		}
		InetGetMac(mac);
		print_mac(mac);
		printf("\n");
		exit(0);
	}
#endif
	

	printf("MAC Address Tool %s\n", VERSION);
	printf("Enter the MAC address as a set of 6 hex digits comma seperated\n");

	// Use int variables for scanf, then copy to unsigned char array
	int m0, m1, m2, m3, m4, m5;
	scanf("%x,%x,%x,%x,%x,%x",&m0,&m1,&m2,&m3,&m4,&m5);
	
	// Copy to byte array
	mac[0] = (unsigned char)m0;
	mac[1] = (unsigned char)m1; 
	mac[2] = (unsigned char)m2;
	mac[3] = (unsigned char)m3;
	mac[4] = (unsigned char)m4;
	mac[5] = (unsigned char)m5;
	
#if defined(FRAM) || defined (DISK) || defined (NVRAM)
	InetSetMac(mac);
#else
	remove("mac.cfg");	
	f = fopen("mac.cfg","w");
	fprintf(f,"%d,%d,%d,%d,%d,%d",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	fclose(f);
#endif
}
