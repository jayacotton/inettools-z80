/* process the hosts file when looking for a host name 

This should get called from dns befor it starts a ip/name search
on the dns server.


the hosts file is somewhat free form, but takes the following 

IP <TAB>	NAME  <TAB>	ALIAS
		this is the	this is the
		formal name	informal name
		of the host	of the host

192.168.0.120   server  		linux-host
192.168.0.154   windows			cygwin-host
209.237.226.90  www.opensource.org      opensource
13.227.75.41    www.nasa.gov            nasa

The TAB is really white space.  The ALIAS is optional, and limited to 1 
non white space string.

*/

#include "hosts.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <trace.h>

FILE *in;
HOSTNAME *HostRoot;

/* read the hosts file and parse out the data */
/* this will likely get called every time an inet app
runs.  So, lets make it real clean and short */
extern long heap;
unsigned char *b1;
unsigned char *b2;
void
Hosts_Init ()
{
  HOSTNAME *p;

  HostRoot = calloc (sizeof (HOSTNAME), 1);
  b1 = calloc (80, 1);
  b2 = calloc (80, 1);
printf("heap %x\n",heap);
printf("HostRoot %x, b1 %x, b2 %x\n",HostRoot,b1,b2);
TRACE(" ");
  if (HostRoot)
    {
TRACE(" ");
      p = HostRoot;
      in = fopen ("hosts", "r");
      if (in)
	{
TRACE(" ");
	  while (fscanf
		 (in, "%d.%d.%d.%d %s %s", &p->ip.ip[0], &p->ip.ip[1],
		  &p->ip.ip[2], &p->ip.ip[3], b1, b2) != EOF)
	    {
TRACE(" ");
	      p->name = calloc (strlen (b1), 1);
	      if (p->name == 0)
		return;
TRACE(" ");
	      p->alias = calloc (strlen (b2), 1);
	      if (p->alias == 0)
		return;
TRACE(" ");
	      strupr (b1);
	      strupr (b2);
	      strcpy (p->name, b1);
	      strcpy (p->alias, b2);
      printf ("%d.%d.%d.%d\t%s\t%s", p->ip.ip[0], p->ip.ip[1],
	      p->ip.ip[2], p->ip.ip[3], p->name, p->alias);
	      p->next = calloc (sizeof (HOSTNAME), 1);
	      if (p->next == 0)
		return;
	      p = p->next;
	    }
	}
      fclose (in);
      free (b1);
      free (b2);
    }
}

void
Hosts_Dump ()
{
  HOSTNAME *p;

  p = HostRoot;
	if(p == 0) return;
  do
    {
      printf ("%d.%d.%d.%d\t%s\t%s", p->ip.ip[0], p->ip.ip[1],
	      p->ip.ip[2], p->ip.ip[3], p->name, p->alias);
      p = p->next;
    }
  while (p);
}

/* name can be the formal host name or an alias */
/* NOTE:  CP/M has the anoying habit of converting strings
to upper case at unexpected times.  So, in lookup we convert
all the strings to upper case befor the search. */

HOSTIP *
Hosts_Lookup (char *name)
{
  HOSTNAME *p;
  char *c;
  c = strupr (name);
  p = HostRoot;
	if(p == 0) return NULL;
  do
    {
      if (strstr (p->name, c))
	{
	  return (&p->ip);
	}
      else if (strstr (p->alias, c))
	{
	  return (&p->ip);
	}
      p = p->next;
    }
  while (p);
  return NULL;
}
