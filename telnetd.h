/* telnet daemon header */

/* define all the handshake stuff */

#define IAC 	255 
#define SE 	240
#define NOP 	241
#define DM 	242
#define BRK 	243
#define IP 	244
#define AO 	245
#define AYT 	246
#define EC 	247
#define EL 	248
#define GA 	249
#define SB 	250
/* these codes expect options to follow */
#define WILL 	251
#define WONT 	252
#define DO 	253
#define DONT 	254
