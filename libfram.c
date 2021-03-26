#include "fram.h"
#include <string.h>
#include <stdio.h>

#ifdef FRAM
#include "spi.h"

struct storage s;

void FramSetGate(unsigned char *ip)
{
int i;
	spi_init();
	FramRead(&s);
	for(i=0;i<4;i++){
		s.Fram1.EtherGate[i] = ip[i];
	}
	FramWrite(&s);
}
void FramGetGate(unsigned char *ip)
{
int i;
	spi_init();
	FramRead(&s);
	for(i=0;i<4;i++){
		ip[i]= s.Fram1.EtherGate[i]; 
	}
}
void FramSetIP(unsigned char *ip)
{
int i;
	spi_init();
	FramRead(&s);
	for(i=0;i<4;i++){
		s.Fram1.EtherIP[i] = ip[i];
	}
	FramWrite(&s);
}
void FramGetIP(unsigned char *ip)
{
int i;
	spi_init();
	FramRead(&s);
	for(i=0;i<4;i++){
		ip[i]= s.Fram1.EtherIP[i]; 
	}
}
void FramSetMask(unsigned char *ip)
{
int i;
	spi_init();
	FramRead(&s);
	for(i=0;i<4;i++){
		s.Fram1.EtherMask[i] = ip[i];
	}
	FramWrite(&s);
}
void FramGetMask(unsigned char *ip)
{
int i;
	spi_init();
	FramRead(&s);
	for(i=0;i<4;i++){
		ip[i]= s.Fram1.EtherMask[i]; 
	}
}
void FramSetDns(unsigned char *ip)
{
int i;
	spi_init();
	FramRead(&s);
	for(i=0;i<4;i++){
		s.Fram1.EtherDns[i] = ip[i];
	}
	FramWrite(&s);
}
void FramGetDns(unsigned char *ip)
{
int i;
	spi_init();
	FramRead(&s);
	for(i=0;i<4;i++){
		ip[i]= s.Fram1.EtherDns[i]; 
	}
}
void FramSetDeltaUptime(unsigned long uptime)
{
	spi_init();
	FramRead(&s);
	s.Fram2.DeltaUptime = uptime;
	FramWrite(&s);
}
unsigned long FramGetDeltaUptime()
{
	spi_init();
	FramRead(&s);
	return (s.Fram2.DeltaUptime);
}
void FramSetEpoch(long epoch)
{
	spi_init();
	FramRead(&s);
	s.Fram2.Epoch = epoch;
	FramWrite(&s);
}
long FramGetEpoch()
{
	spi_init();
	FramRead(&s);
	return (s.Fram2.Epoch);
}
void FramSetTZ(long zone)
{
	spi_init();
	FramRead(&s);
	s.Fram2.TimeZone = zone;
	FramWrite(&s);
}
long FramGetTZ()
{
	spi_init();
	FramRead(&s);
	return(s.Fram2.TimeZone);
}
void FramSetTZText(unsigned char *t)
{
	spi_init();
	FramRead(&s);
	memcpy(s.Fram2.TimeZoneText,t,4);
	FramWrite(&s);
}
void FramGetTZText(unsigned char *t)
{
int i;
	spi_init();
	FramRead(&s);
	memcpy(t,s.Fram2.TimeZoneText,4);
}
void FramGetMac(unsigned int *mac)
{
int i;
	spi_init();
	FramRead(&s);
	for(i=0;i<6;i++){
		mac[i] = s.Fram1.EtherAddr[i];
	}
}
void FramSetMac(unsigned int *mac)
{
int i;
	spi_init();
	FramRead(&s);
	for(i=0;i<6;i++){
		s.Fram1.EtherAddr[i]=mac[i];
	}
	FramWrite(&s);
}
// write back to the FRAM
void FramWrite(struct storage *ram)
{
int size;
int i;
unsigned char *p;

	spi_init();
	// convert struct pointer to char pointer
	p = (unsigned char *)ram;
	// and get size of transfer
	size = sizeof(struct storage);
	// write a write enable
	SpiSelect(FRam);
	SpiWrite(FRMWREN);
	SpiSelect(PARK1);
	// Write a write memory command 
	SpiSelect(FRam);
	SpiWrite(FRMWRT);
	SpiWrite16(0);
	for(i=0;i<size;i++){
		SpiWrite(*p++);
	}
	// deselect 
	SpiSelect(PARK1);
	// now, write protect the ram 
	SpiSelect(FRam);
	SpiWrite(FRMWDRI);
	SpiSelect(PARK1);
}
// read the FRAM into local storage
void FramRead(struct storage *ram)
{
int size;
int i;
unsigned char *p;

	spi_init();
	// get transfer size and convert struct the char pointer
	size = sizeof(struct storage);
	p = (unsigned char *)ram;
	// select FRAM
	SpiSelect(FRam);	// select the FRAM on the SPI bus
	// write the READ command
	SpiCommand(FRMREAD);	// Send a read command
	// and the address
	SpiWrite16(0);		// Send address 
	// and the bytes
	for(i=0;i<size;i++){
		*p++ = SpiRead();
	}
	// and deselect the FRAM
	SpiSelect(PARK1);
}
void FramDumper()
{
	printf("EtherAddr %x.%x.%x.%x.%x.%x\n",
		s.Fram1.EtherAddr[0],s.Fram1.EtherAddr[1],s.Fram1.EtherAddr[2],
		s.Fram1.EtherAddr[3],s.Fram1.EtherAddr[4],s.Fram1.EtherAddr[5]);
}
#endif
