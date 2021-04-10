#include <string.h>
#include <stdio.h>
#include "spi.h"
#include "inet.h"
#include "fram.h"
#include "disk.h"
#include "nvram.h"

void
InetSetTofU (unsigned long time)
{
#ifdef FRAM
  FramSetTofU (time);
#elif DISK
  DiskSetTofU (time);
#elif NVRAM
  NvramSetTofU (time);
#endif
}

unsigned long
InetGetTofU ()
{
#ifdef FRAM
  return FramGetTofU ();
#elif DISK
  return DiskGetTofU ();
#elif NVRAM
  return NvramGetTofU ();
#endif
}

void
InetSetUptime (unsigned long time)
{
#ifdef FRAM
  FramSetUptime (time);
#elif DISK
  DiskSetUptime (time);
#elif NVRAM
  NvramSetUptime (time);
#endif
}

unsigned long
InetGetUptime (int flag)
{
#ifdef FRAM
  return FramGetUptime (flag);
#elif DISK
  return DiskGetUptime (flag);
#elif NVRAM
  return NvramGetUptime (flag);
#endif
}

void
InetSetDeltaUptime (unsigned long uptime)
{
#ifdef FRAM
  FramSetDeltaUptime (uptime);
#elif DISK
  DiskSetDeltaUptime (uptime);
#elif NVRAM
  NvramSetDeltaUptime (uptime);
#endif
}

unsigned long
InetGetDeltaUptime ()
{
#ifdef FRAM
  return FramGetDeltaUptime ();
#elif DISK
  return DiskGetDeltaUptime ();
#elif NVRAM
  return NvramGetDeltaUptime ();
#endif
}

void
InetSetEpoch (long epoch)
{
#ifdef FRAM
  FramSetEpoch (epoch);
#elif DISK
  DiskSetEpoch (epoch);
#elif NVRAM
  NvramSetEpoch (epoch);
#endif
}

long
InetGetEpoch ()
{
#ifdef FRAM
  return FramGetEpoch ();
#elif DISK
  return DiskGetEpoch ();
#elif NVRAM
  return NvramGetEpoch ();
#endif
}

void
InetSetMac (unsigned int *mac)
{
#ifdef FRAM
  FramSetMac (mac);
#elif DISK
  DiskSetMac (mac);
#elif NVRAM
  NvramSetMac (mac);
#endif
}

void
InetGetMac (unsigned int *mac)
{
#ifdef FRAM
  FramGetMac (mac);
#elif DISK
  DiskGetMac (mac);
#elif NVRAM
  NvramGetMac (mac);
#endif
}

void
InetSetTZ (long zone)
{
#ifdef FRAM
  FramSetTZ (zone);
#elif DISK
  DiskSetTZ (zone);
#elif NVRAM
  NvramSetTZ (zone);
#endif
}

long
InetGetTZ ()
{
#ifdef FRAM
  return FramGetTZ ();
#elif DISK
  return DiskGetTZ ();
#elif NVRAM
  return NvramGetTZ ();
#endif
}

void
InetSetTZText (unsigned char *t)
{
#ifdef FRAM
  FramSetTZText (t);
#elif DISK
  DiskSetTZText (t);
#elif NVRAM
  NvramSetTZText (t);
#endif
}

void
InetGetTZText (unsigned char *t)
{
#ifdef FRAM
  FramGetTZText (t);
#elif DISK
  DiskGetTZText (t);
#elif NVRAM
  NvramGetTZText (t);
#endif
}

void
InetSetIP (unsigned char *ip)
{
#ifdef FRAM
  FramSetIP (ip);
#elif DISK
  DiskSetIP (ip);
#elif NVRAM
  NvramSetIP (ip);
#endif
}

void
InetGetIP (unsigned char *ip)
{
#ifdef FRAM
  FramGetIP (ip);
#elif DISK
  DiskGetIP (ip);
#elif NVRAM
  NvramGetIP (ip);
#endif
}

void
InetSetDns (unsigned char *ip)
{
#ifdef FRAM
  FramSetDns (ip);
#elif DISK
  DiskSetDns (ip);
#elif NVRAM
  NvramSetDns (ip);
#endif
}

void
InetGetDns (unsigned char *ip)
{
#ifdef FRAM
  FramGetDns (ip);
#elif DISK
  DiskGetDns (ip);
#elif NVRAM
  NvramGetDns (ip);
#endif
}

void
InetSetGate (unsigned char *ip)
{
#ifdef FRAM
  FramSetGate (ip);
#elif DISK
  DiskSetGate (ip);
#elif NVRAM
  NvramSetGate (ip);
#endif
}

void
InetGetGate (unsigned char *ip)
{
#ifdef FRAM
  FramGetGate (ip);
#elif DISK
  DiskGetGate (ip);
#elif NVRAM
  NvramGetGate (ip);
#endif
}

void
InetSetMask (unsigned char *ip)
{
#ifdef FRAM
  FramSetMask (ip);
#elif DISK
  DiskSetMask (ip);
#elif NVRAM
  NvramSetMask (ip);
#endif
}

void
InetGetMask (unsigned char *ip)
{
#ifdef FRAM
  FramGetMask (ip);
#elif DISK
  DiskGetMask (ip);
#elif NVRAM
  NvramGetMask (ip);
#endif
}

void
InetDumper ()
{
#ifdef FRAM
  FramDumper ();
#elif DISK
  DiskDumper ();
#elif NVRAM
  NvramDumper ();
#endif
}

#ifdef FRAM

struct storage s;

void
FramSetTofU (unsigned long time)
{
  spi_init ();
  FramRead (&s);
  s.Fram4.TofU = time;
  FramWrite (&s);
}

unsigned long
FramGetTofU ()
{
  spi_init ();
  FramRead (&s);
  return (s.Fram4.TofU);
}

void
FramSetUptime (unsigned long time)
{
  spi_init ();
  FramRead (&s);
  s.Fram2.Uptime = time;
  FramWrite (&s);
}

unsigned long
FramGetUptime (int flag)
{
  spi_init ();
  FramRead (&s);
  if (flag)
    return (s.Fram2.Uptime);
  return (s.Fram2.Uptime - FramGetDeltaUptime ());
}

void
FramSetGate (unsigned char *ip)
{
  int i;
  spi_init ();
  FramRead (&s);
  for (i = 0; i < 4; i++)
    {
      s.Fram1.EtherGate[i] = ip[i];
    }
  FramWrite (&s);
}

void
FramGetGate (unsigned char *ip)
{
  int i;
  spi_init ();
  FramRead (&s);
  for (i = 0; i < 4; i++)
    {
      ip[i] = s.Fram1.EtherGate[i];
    }
}

void
FramSetIP (unsigned char *ip)
{
  int i;
  spi_init ();
  FramRead (&s);
  for (i = 0; i < 4; i++)
    {
      s.Fram1.EtherIP[i] = ip[i];
    }
  FramWrite (&s);
}

void
FramGetIP (unsigned char *ip)
{
  int i;
  spi_init ();
  FramRead (&s);
  for (i = 0; i < 4; i++)
    {
      ip[i] = s.Fram1.EtherIP[i];
    }
}

void
FramSetMask (unsigned char *ip)
{
  int i;
  spi_init ();
  FramRead (&s);
  for (i = 0; i < 4; i++)
    {
      s.Fram1.EtherMask[i] = ip[i];
    }
  FramWrite (&s);
}

void
FramGetMask (unsigned char *ip)
{
  int i;
  spi_init ();
  FramRead (&s);
  for (i = 0; i < 4; i++)
    {
      ip[i] = s.Fram1.EtherMask[i];
    }
}

void
FramSetDns (unsigned char *ip)
{
  int i;
  spi_init ();
  FramRead (&s);
  for (i = 0; i < 4; i++)
    {
      s.Fram1.EtherDns[i] = ip[i];
    }
  FramWrite (&s);
}

void
FramGetDns (unsigned char *ip)
{
  int i;
  spi_init ();
  FramRead (&s);
  for (i = 0; i < 4; i++)
    {
      ip[i] = s.Fram1.EtherDns[i];
    }
}

void
FramSetDeltaUptime (unsigned long uptime)
{
  spi_init ();
  FramRead (&s);
  s.Fram2.DeltaUptime = uptime;
  FramWrite (&s);
}

unsigned long
FramGetDeltaUptime ()
{
  spi_init ();
  FramRead (&s);
  return (s.Fram2.DeltaUptime);
}

void
FramSetEpoch (long epoch)
{
  spi_init ();
  FramRead (&s);
  s.Fram2.Epoch = epoch;
  FramWrite (&s);
}

long
FramGetEpoch ()
{
  spi_init ();
  FramRead (&s);
  return (s.Fram2.Epoch);
}

void
FramSetTZ (long zone)
{
  spi_init ();
  FramRead (&s);
  s.Fram2.TimeZone = zone;
  FramWrite (&s);
}

long
FramGetTZ ()
{
  spi_init ();
  FramRead (&s);
  return (s.Fram2.TimeZone);
}

void
FramSetTZText (unsigned char *t)
{
  spi_init ();
  FramRead (&s);
  memcpy (s.Fram2.TimeZoneText, t, 4);
  FramWrite (&s);
}

void
FramGetTZText (unsigned char *t)
{
  int i;
  spi_init ();
  FramRead (&s);
  memcpy (t, s.Fram2.TimeZoneText, 4);
}

void
FramGetMac (unsigned int *mac)
{
  int i;
  spi_init ();
  FramRead (&s);
  for (i = 0; i < 6; i++)
    {
      mac[i] = s.Fram1.EtherAddr[i];
    }
}

void
FramSetMac (unsigned int *mac)
{
  int i;
  spi_init ();
  FramRead (&s);
  for (i = 0; i < 6; i++)
    {
      s.Fram1.EtherAddr[i] = mac[i];
    }
  FramWrite (&s);
}

// write back to the FRAM
void
FramWrite (struct storage *ram)
{
  int size;
  int i;
  unsigned char *p;

  spi_init ();
  // convert struct pointer to char pointer
  p = (unsigned char *) ram;
  // and get size of transfer
  size = sizeof (struct storage);
  // write a write enable
  SpiSelect (FRam);
  SpiWrite (FRMWREN);
  SpiSelect (PARK1);
  // Write a write memory command 
  SpiSelect (FRam);
  SpiWrite (FRMWRT);
  SpiWrite16 (0);
  for (i = 0; i < size; i++)
    {
      SpiWrite (*p++);
    }
  // deselect 
  SpiSelect (PARK1);
  // now, write protect the ram 
  SpiSelect (FRam);
  SpiWrite (FRMWDRI);
  SpiSelect (PARK1);
}

// read the FRAM into local storage
void
FramRead (struct storage *ram)
{
  int size;
  int i;
  unsigned char *p;

  spi_init ();
  // get transfer size and convert struct the char pointer
  size = sizeof (struct storage);
  p = (unsigned char *) ram;
  // select FRAM
  SpiSelect (FRam);		// select the FRAM on the SPI bus
  // write the READ command
  SpiCommand (FRMREAD);		// Send a read command
  // and the address
  SpiWrite16 (0);		// Send address 
  // and the bytes
  for (i = 0; i < size; i++)
    {
      *p++ = SpiRead ();
    }
  // and deselect the FRAM
  SpiSelect (PARK1);
}

void
FramDumper ()
{
  printf ("EtherAddr %x.%x.%x.%x.%x.%x\n",
	  s.Fram1.EtherAddr[0], s.Fram1.EtherAddr[1], s.Fram1.EtherAddr[2],
	  s.Fram1.EtherAddr[3], s.Fram1.EtherAddr[4], s.Fram1.EtherAddr[5]);
}
#endif
#ifdef DISK

struct storage s;

void
DiskSetTofU (unsigned long time)
{
  DiskRead (&s);
  s.Fram4.TofU = time;
  DiskWrite (&s);
}

unsigned long
DiskGetTofU ()
{
  DiskRead (&s);
  return (s.Fram4.TofU);
}

void
DiskSetUptime (unsigned long time)
{
  DiskRead (&s);
  s.Fram2.Uptime = time;
  DiskWrite (&s);
}

unsigned long
DiskGetUptime (int flag)
{
  DiskRead (&s);
  if (flag)
    return (s.Fram2.Uptime);
  return (s.Fram2.Uptime - DiskGetDeltaUptime ());
}

void
DiskSetGate (unsigned char *ip)
{
  int i;
  DiskRead (&s);
  for (i = 0; i < 4; i++)
    {
      s.Fram1.EtherGate[i] = ip[i];
    }
  DiskWrite (&s);
}

void
DiskGetGate (unsigned char *ip)
{
  int i;
  DiskRead (&s);
  for (i = 0; i < 4; i++)
    {
      ip[i] = s.Fram1.EtherGate[i];
    }
}

void
DiskSetIP (unsigned char *ip)
{
  int i;
  DiskRead (&s);
  for (i = 0; i < 4; i++)
    {
      s.Fram1.EtherIP[i] = ip[i];
    }
  DiskWrite (&s);
}

void
DiskGetIP (unsigned char *ip)
{
  int i;
  DiskRead (&s);
  for (i = 0; i < 4; i++)
    {
      ip[i] = s.Fram1.EtherIP[i];
    }
}

void
DiskSetMask (unsigned char *ip)
{
  int i;
  DiskRead (&s);
  for (i = 0; i < 4; i++)
    {
      s.Fram1.EtherMask[i] = ip[i];
    }
  DiskWrite (&s);
}

void
DiskGetMask (unsigned char *ip)
{
  int i;
  DiskRead (&s);
  for (i = 0; i < 4; i++)
    {
      ip[i] = s.Fram1.EtherMask[i];
    }
}

void
DiskSetDns (unsigned char *ip)
{
  int i;
  DiskRead (&s);
  for (i = 0; i < 4; i++)
    {
      s.Fram1.EtherDns[i] = ip[i];
    }
  DiskWrite (&s);
}

void
DiskGetDns (unsigned char *ip)
{
  int i;
  DiskRead (&s);
  for (i = 0; i < 4; i++)
    {
      ip[i] = s.Fram1.EtherDns[i];
    }
}

void
DiskSetDeltaUptime (unsigned long uptime)
{
  DiskRead (&s);
  s.Fram2.DeltaUptime = uptime;
  DiskWrite (&s);
}

unsigned long
DiskGetDeltaUptime ()
{
  DiskRead (&s);
  return (s.Fram2.DeltaUptime);
}

void
DiskSetEpoch (long epoch)
{
  DiskRead (&s);
  s.Fram2.Epoch = epoch;
  DiskWrite (&s);
}

long
DiskGetEpoch ()
{
  DiskRead (&s);
  return (s.Fram2.Epoch);
}

void
DiskSetTZ (long zone)
{
  DiskRead (&s);
  s.Fram2.TimeZone = zone;
  DiskWrite (&s);
}

long
DiskGetTZ ()
{
  DiskRead (&s);
  return (s.Fram2.TimeZone);
}

void
DiskSetTZText (unsigned char *t)
{
  DiskRead (&s);
  memcpy (s.Fram2.TimeZoneText, t, 4);
  DiskWrite (&s);
}

void
DiskGetTZText (unsigned char *t)
{
  int i;
  DiskRead (&s);
  memcpy (t, s.Fram2.TimeZoneText, 4);
}

void
DiskGetMac (unsigned int *mac)
{
  int i;
  DiskRead (&s);
  for (i = 0; i < 6; i++)
    {
      mac[i] = s.Fram1.EtherAddr[i];
    }
}

void
DiskSetMac (unsigned int *mac)
{
  int i;
  DiskRead (&s);
  for (i = 0; i < 6; i++)
    {
      s.Fram1.EtherAddr[i] = mac[i];
    }
  DiskWrite (&s);
}

FILE *File;
unsigned char *name = "c:inet.cfg";

// Note:  we will have to keep track of the default 
// boot drive or default system drive (it moves)
// write back to the DISK
void
DiskWrite (struct storage *ram)
{
  File = fopen (name, "w");
  fwrite (ram, sizeof (struct storage), 1, File);
  fclose (File);
}

// read the DISK into local storage
void
DiskRead (struct storage *ram)
{
  File = fopen (name, "r");
  if (File == 0)
    return;
  fread (ram, sizeof (struct storage), 1, File);
  fclose (File);
}

void
DiskDumper ()
{
  printf ("EtherAddr %x.%x.%x.%x.%x.%x\n",
	  s.Fram1.EtherAddr[0], s.Fram1.EtherAddr[1], s.Fram1.EtherAddr[2],
	  s.Fram1.EtherAddr[3], s.Fram1.EtherAddr[4], s.Fram1.EtherAddr[5]);
}
#endif

#ifdef NVRAM
void
NvramSetTofU (unsigned long time)
{
}

void
NvramSetMask (unsigned char *ip)
{
}

void
NvramSetUptime (unsigned long time)
{
}

void
NvramSetIP (unsigned char *ip)
{
}

void
NvramSetGate (unsigned char *ip)
{
}

void
NvramSetDeltaUptime (unsigned long time)
{
  int i;
  int addr;
  for (i = 0; i < 4; i++)
    {
      addr = UptimeBuf + (i << 1);
      SetNvram (addr, (unsigned char) uptime & 0xff);
      uptime = uptime >> 8;
    }
}

void
NvramSetEpoch (long time)
{
  int i;
  int addr;
  for (i = 0; i < 4; i++)
    {
      addr = EpochBuf + ((i) << 1);
      SetNvram (addr, (unsigned char) lepoch & 0xff);
      lepoch = lepoch >> 8;
    }

}

void
NvramSetMac (unsigned int *ip)
{
}

void
NvramSetTZ (long zone)
{
  int i;
  long lzone;
  int addr;
  lzone = zone;
  for (i = 0; i < 4; i++)
    {
      addr = TZbuf + (i << 1);
      SetNvram (addr, (unsigned char) lzone & 0xff);
      lzone = lzone >> 8;
    }

}

void
NvramSetTZText (unsigned char *txt)
{
  int i;
  int addr;
  for (i = 0; i < 4; i++)
    {
      addr = TXtxt + (i << 1);
      SetNvram (addr, txt[i]);
    }
}

void
NvramSetDns (unsigned char *ip)
{
}

void
NvramGetDns (unsigned char *ip)
{
}

void
NvramGetGate (unsigned char *ip)
{
}

unsigned long
NvramGetDeltaUptime ()
{
  for (i = 0; i < 4; i++)
    {
      res = res << 8;
      addr = UptimeBuf + ((3 - i) << 1);
      res |= (unsigned char) GetNvram (addr);
    }
  return res;
}

long
NvramGetTZ ()
{
  int i;
  long res;
  int addr;
  res = 0;
  for (i = 0; i < 4; i++)
    {
      res = res << 8;
      addr = TZbuf + ((3 - i) << 1);
      res |= 0xff & GetNvram (addr);
    }
  return res;
}

void
NvramGetTZText (unsigned char *txt)
{
  int i;
  int addr;
  memset (TZName, 0, 6);
  for (i = 0; i < 4; i++)
    {
      addr = TZtxt + ((i) << 1);
      TZName[i] = GetNvram (addr);
    }
  return &TZName;

}

long
NvramGetEpoch ()
{
  int i;
  long res;
  int addr;

  for (i = 0; i < 4; i++)
    {
      res = res << 8;
      addr = EpochBuf + ((3 - i) << 1);
      res |= 0xff & GetNvram (addr);
    }
  return res;
}

void
NvramGetIP (unsigned char *ip)
{
}

void
NvramGetMask (unsigned char *ip)
{
}

void
NvramGetMac (unsigned int *ip)
{
}

unsigned long
NvramGetTofU ()
{
  return time;
}

unsigned long
NvramGetUptime (int flag)
{
  return GetUptime (flag);
}

void
NvramRead (struct storage *ram)
{
  int i;
  unsigned char *p;
  p = (unsigned char *) ram;
  for (i = 0; i < sizeof (struct storage); i++)
    {
      SetNvram (i, *p++);
    }
}

void
NvramWrite (struct storage *ram)
{
  int i;
  unsigned char *p p = (unsigned char *) ram;
for (i = 0:i < sizeof (struct storage); i++)
    {
      *p++ = GetNvram (i);
    }
}

void
NvramDumper ()
{
}
#endif
