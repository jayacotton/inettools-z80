#include "fram.h"
#include <string.h>
#include <stdio.h>

#ifdef FRAM

struct storage s;

void FramSetMac(unsigned char *mac)
{
	FramRead(&s);
	memcpy(s.Fram1.EtherAddr,mac,6);
	FramDumper();
}

// read the FRAM into local storage
void FramRead(struct storage *ram)
{
int i;
unsigned char *p;

	SpiSelect(FRam);	// select the FRAM on the SPI bus
	SpiCommand(FRMREAD);	// Send a read command
	SpiWrite16(0);		// Send address 
	i = sizeof(struct storage);
	p = (unsigned char *)ram;
	while(i){
		*p = SpiRead();
		p++;
		i--;
	}
	SpiSelect(PARK);
}
void FramDumper()
{
	snapmem(0,&s,0x28,4,"Fram Storage");
	printf("EtherAddr %x.%x.%x.%x.%x.%x\n",
		s.Fram1.EtherAddr[0],s.Fram1.EtherAddr[1],s.Fram1.EtherAddr[2],
		s.Fram1.EtherAddr[3],s.Fram1.EtherAddr[4],s.Fram1.EtherAddr[5]);
}
#endif
