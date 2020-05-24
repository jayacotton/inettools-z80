/**
	Modified for cpm and z88dk by Jay Cotton 3/25/19
 */

#include <stdio.h>
#include <string.h>
#include "spi.h"
#include "w5500.h"
#include "socket.h"
#include "dhcp.h"
#include <stdlib.h>
#include "types.h"

#define SOCK_DHCP 6
#define MY_MAX_DHCP_RETRY 2

void
wait_10ms (int v)
{
  csleep (v);
}

/*
********************************************************************************
Function Implementation Part
********************************************************************************
*/

/**
	@mainpage  Inet tools and w5500 firmware
	@section intro	Introduction
	- Introduction : telnet, ping, ifconfig, wget, dig. 
	@section CREATEINFO	Author
	- author : www.wiznet.co.kr
	- last update : 2008.1.8
        - author : Jay Cotton
        - last update : 4/15/2019
*/
wiz_NetInfo gWIZNETINFO;
unsigned char run_user_applications;
unsigned char buff[256];
int
main (void)
{
  u_char tmpip[4];
  unsigned char my_dhcp_retry;

  printf ("\n*** DHCP Test using W5500 [SPI MODE] ***\n");

  run_user_applications = 0;
  my_dhcp_retry = 0;
  wait_10ms (100);
  spi_init ();
  /* get ip */
  DHCP_init (SOCK_DHCP, buff);

  if (run_user_applications)
    {
      printf ("*** THANK YOU ***\n");
      tmpip[0] = 211;
      tmpip[1] = 46;
      tmpip[2] = 117;
      tmpip[3] = 79;

      sendto (3, (const u_char *) "for PING TEST", 13, tmpip, 5000);
      while (1)
	{
	  check_DHCP_state (0);
	}
    }

  return 1;
}
