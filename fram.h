/*  FRAM is a nonvolital ram device on the ethernet spi board.

This ram is used to store all the network information, time zone
data and any thing else that may need long term storage.

For that reason it is loosly associated with inet stack but 
not a required device

*/


typedef struct inet_data {
unsigned char EtherAddr [6];	/* the hardware device address */
unsigned char EtherIP	[4];	/* the Ethernet address provided by dhcp */
unsigned char EtherMask [4];	/* the domain mask bits */
unsigned char EtherGate	[4];	/* the gateway address */
unsigned char EtherDns	[4];	/* the dns server address */
} INET_DATA;

typedef struct time_zone {
unsigned int TimeZone [2];	/* the time zone */
} TIME_ZONE;

typedef struct cpnet_data {
unsigned char Client;
unsigned char Server;
} CPNET_DATA;

struct storage {
INET_DATA Fram1;
TIME_ZONE Fram2;
CPNET_DATA Fram3;
};

extern void FramSetMac(unsigned char *);
extern void FramRead(struct storage *);
extern void FramDumper();

/*  FRAM defines for use with the SPI bus */

#define FRam 	2
#define SD	1
#define ENET	0
#define PARK	3

/* fram chip command bytes */

#define FRMWREN	6
#define	FRMWDRI 4
#define FRMWDSR 5
#define FRMWRSR 1
#define FRMREAD 3
#define FRMWRT	2

extern void SpiSelect(unsigned char);
extern void SpiCommand(unsigned char);
extern void SpiWrite(unsigned char);
extern void SpiWrite16(unsigned char);
extern unsigned char SpiRead();

