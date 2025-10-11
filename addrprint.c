#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ethernet.h"
#include "wizchip_conf.h"
#include "trace.h"
#include "inet.h"
#include "hexprint.h"

#define VERSION "v1.9"

wiz_NetInfo gWIZNETINFO;
unsigned char run_user_applications;
unsigned char mac[6];
unsigned char ip[4];



unsigned int DeltaTimeINET()
{
  return(InetGetEpoch()-InetGetTofU());
}

void main(int argc, char *argv[])
{
  // Get MAC address from FRAM first, before any network operations
  InetGetMac(mac);
  
  if (argc > 1) {
    if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
      printf("IFCONFIG %s\n", VERSION);
      exit(0);
    }
    InetSetTofU(InetGetEpoch());
    if (Ethernet_begin(mac) == 0) {
      if (Ethernet_hardwareStatus() == EthernetNoHardware)
        printf("Can't find the ethernet h/w\n");
      if (Ethernet_linkStatus() == LinkOFF)
        printf("Plug in the cable\n");
    }
    Ethernet_localIP(ip);
    InetSetIP(ip);
    Ethernet_localSN(ip);
    InetSetMask(ip);
    Ethernet_localDNS(ip);
    InetSetDns(ip);
    Ethernet_localGW(ip);
    InetSetGate(ip);
  }
  
  if (DeltaTimeINET() > 1440) {
    InetSetTofU(InetGetEpoch());
    if (Ethernet_begin(mac) == 0) {
      if (Ethernet_hardwareStatus() == EthernetNoHardware)
        printf("Can't find the ethernet h/w\n");
      if (Ethernet_linkStatus() == LinkOFF)
        printf("Plug in the cable\n");
    }
  }
  
  InetGetIP(ip);
  printf("IFCONFIG %s\n", VERSION);
  printf("inet %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
  InetGetMask(ip);
  printf("netmask %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
  InetGetDns(ip);
  printf("dns server %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
  InetGetGate(ip);
  printf("Default Gateway %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
  InetGetMac(mac);
  printf("ether "); print_mac(mac); printf("\n");
}
