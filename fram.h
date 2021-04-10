/*  FRAM is a nonvolital ram device on the ethernet spi board.

This ram is used to store all the network information, time zone
data and any thing else that may need long term storage.

For that reason it is loosly associated with inet stack but 
not a required device.

*/

/*
 * Layout of NVRAM block used to store CP/NET configuration.
 */

/*
 * Unused bytes should be initialized to 0xff
 * (buffer is initially set to all 0xff).
 *
 * NOTE: Erased FLASH memory is 0xff, FRAM ships as 0x00.
 */

typedef struct wizsok {
	uchar	resv1[4];
	uchar	cpnet;		// Sn_PORT0: 0x31 if configured for CP/NETvi 
	uchar	sid;		// Sn_PORT1: CP/NET server node ID
	uchar	resv2[6];
	uchar	dipr[4];	// destination (server) IP address
	uchar	dport[2];	// destination (server) port
	uchar	resv3[11];
	uchar	kpalvtr;	// moved to Sn_KPALVTR in W5500
	uchar	resv4[2];
}WizSok;

typedef struct wizcfg {
	uchar	resv1;
	uchar	gar[4];		// gateway IP address
	uchar	subr[4];	// subnet mask
	uchar	shar[6];	// MAC address
	uchar	sipr[4];	// client IP address
	uchar	resv2[10];
	uchar	pmagic;		// client CP/NET node ID
	uchar	resv3[2];

	WizSok	sockets[8];

	uchar	cfgtbl[38];	// CP/NET config table template

	uchar	resv4[182];	// for expansion
	uchar	chksum[4];	// 32-bit sum of first 508 bytes, little-endian
}WizCfg;

/* sizeof(struct wizcfg) == 512 */


typedef struct inet_data
{
  unsigned char EtherAddr[6];	/* the hardware device address */
  unsigned char EtherIP[4];	/* the Ethernet address provided by dhcp */
  unsigned char EtherMask[4];	/* the domain mask bits */
  unsigned char EtherGate[4];	/* the gateway address */
  unsigned char EtherDns[4];	/* the dns server address */
} INET_DATA;

typedef struct time_zone
{
  unsigned long TimeZone;	/* the time zone */
  unsigned char TimeZoneText[4];
  unsigned long Epoch;		/* time epoch since 1970 */
  unsigned long DeltaUptime;
  unsigned long Uptime;
} TIME_ZONE;

// deprecate cpnet_data due to wizcfg above
typedef struct cpnet_data
{
  unsigned char Client;
  unsigned char Server;
} CPNET_DATA;

typedef struct event_data
{
  unsigned long TofU;		/* time of last config */
  unsigned long Event1;		/* timeing events to remember */
  unsigned long Event2;
  unsigned long Event3;
} EVENT_DATA;

/* this is the long term storage.  Please add any additions to the
end of the list */

struct storage
{
  WizCfg    Fram0;		/* all of the CPNET nv ram data */
  INET_DATA Fram1;		/* all the ip net data */
  TIME_ZONE Fram2;		/* all the timezone data */
// depricate CPNET_DATA
  CPNET_DATA Fram3;		/* all the cpnet data */
  EVENT_DATA Fram4;		/* all the time event data */
};

extern void FramSetTofU(unsigned long);
extern unsigned long FramGetTofU();
extern void FramSetUptime(unsigned long);
extern unsigned long FramGetUptime(int);
extern void FramSetDeltaUptime (unsigned long);
extern unsigned long FramGetDeltaUptime ();
extern void FramSetEpoch (long);
extern long FramGetEpoch ();
extern void FramWrite (struct storage *);
extern void FramSetMac (unsigned int *);
extern void FramGetMac (unsigned int *);
extern void FramSetTZ (long);
extern long FramGetTZ ();
extern void FramSetTZText (unsigned char *);
extern void FramGetTZText (unsigned char *);
extern void FramSetIP (unsigned char *);
extern void FramGetIP (unsigned char *);
extern void FramSetDns (unsigned char *);
extern void FramGetDns (unsigned char *);
extern void FramSetGate (unsigned char *);
extern void FramGetGate (unsigned char *);
extern void FramSetMask (unsigned char *);
extern void FramGetMask (unsigned char *);
extern void FramRead (struct storage *);
extern void FramDumper ();

/*  FRAM defines for use with the SPI bus */

#define FRam 	4
#define ENET	2
#define SD	3
#define PARK1	1

/* fram chip command bytes */

#define FRMWREN	6
#define	FRMWDRI 4
#define FRMWDSR 5
#define FRMWRSR 1
#define FRMREAD 3
#define FRMWRT	2

extern void SpiSelect (unsigned char);
extern void SpiCommand (unsigned char);
extern void SpiWrite (unsigned char);
extern void SpiWrite16 (unsigned char);
extern unsigned char SpiRead ();
