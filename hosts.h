/* hosts.h 

All the defines for libhosts.c

*/

typedef struct hostip {
unsigned char ip[4];
}HOSTIP;

typedef struct hostname {
HOSTIP	ip;		/* host ip address */
unsigned char *name;	/* formal name string */
unsigned char *alias;	/* alias name string */
struct hostname *next;  /* pointer to next record in list */
}HOSTNAME;


extern HOSTNAME *HostRoot;	/* anchor the list here */

extern void Hosts_Init();
extern void Hosts_Dump();
extern HOSTIP *Hosts_Lookup(char *name);

