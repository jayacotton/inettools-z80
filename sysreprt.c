/* this program will list the sysface storage values
 * from the system storage.  This can be FRAM,DISK or NVRAM.
 *
 * the program is generally used to verify sysface stroage integrety.
 * there are no parameters for this program.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sysface.h"
#include "fram.h"
#include "inet.h"

extern struct storage s;
unsigned char *ptr;
unsigned char *ptr1;
int memsize;
unsigned char tz[6];
unsigned char ipbuf[8];
void
pIP (char *str, unsigned char *buf)
{
  printf ("%s %d.%d.%d.%d\n", str, buf[0], buf[1], buf[2], buf[3]);
}

void
report ()
{
  InetGetIP (ipbuf);
  pIP ("Client IP: ", ipbuf);
  InetGetMask (ipbuf);
  pIP ("NetMask:   ", ipbuf);
  InetGetDns (ipbuf);
  pIP ("DNS Server:", ipbuf);
  InetGetGate (ipbuf);
  pIP ("GateWay:   ", ipbuf);
  InetGetMac (ipbuf);
  printf ("MAC addr:   %02x:%02x:%02x:%02x:%02x:%02x\n", ipbuf[0], ipbuf[1],
	  ipbuf[2], ipbuf[3], ipbuf[4], ipbuf[5]);

  // TimeZone: TimeZone
  printf ("Seconds offset: %ld\n", InetGetTZ ());
  // TimeZoneText:
  memset (tz, 0, 6);
  InetGetTZText ((unsigned char *) tz);
  printf ("TimeZone: %s\n", tz);
  // Epoch:
  printf ("Epoch: %ld\n", InetGetEpoch ());
  // Uptime:
  printf ("Uptime: %ld\n", InetGetUptime (0));
  // DeltaUptime:
  printf ("Delta: %ld\n", InetGetDeltaUptime ());
  // CPNET info:
  // for now, just dredge the info from FRAM (if we have it)
#ifdef FRAM
  // gar: gateway IP address
  printf ("GAR          : %d.%d.%d.%d\n", s.Fram0.gar[0],
	  s.Fram0.gar[1], s.Fram0.gar[2], s.Fram0.gar[3]);
  // subr: subnet mask
  printf ("SUBR         : %d.%d.%d.%d\n", s.Fram0.subr[0],
	  s.Fram0.subr[1], s.Fram0.subr[2], s.Fram0.subr[3]);
  // shar: MAC address
  printf ("SHAR         :  %02x:%02x:%02x:%02x:%02x:%02x\n", s.Fram0.shar[0],
	  s.Fram0.shar[1], s.Fram0.shar[2], s.Fram0.shar[3], s.Fram0.shar[4],
	  s.Fram0.shar[5]);
  // sipr: client IP address
  printf ("SIPR         : %d.%d.%d.%d\n", s.Fram0.sipr[0],
	  s.Fram0.sipr[1], s.Fram0.sipr[2], s.Fram0.sipr[3]);
  // pmagic: client CP/NET node id
  printf("Magic %d\n",s.Fram0.pmagic);
#endif
  ptr = (unsigned char *) 2;
  printf ("Start of BIOS   : %04x\n", *ptr * 0x100);
  ptr1 = (unsigned char *) 6;
  printf ("Start of BDOS   : %04x\n", ((*ptr - 0xe) * 0x100) + *ptr1);
  printf ("Start of CCP    : %04x\n", (*ptr - 0x16) * 0x100);
  memsize = 256 * *ptr;
  memsize = memsize - 18944;
  memsize = (memsize / 1024) + 20;
  printf ("CP/M size       : %dk\n", memsize);
  ptr1 = (unsigned char *) 7;
  printf ("Available TPA   : %dk\n", (256 * (*ptr1) - 1) / 1024);
  if ((*ptr - 0x3) == *ptr1)
    printf ("RSX installed   :No\n");
  else
    printf ("RSX installed   :Yes\n");
}


void
main ()
{
  report ();
}
