extern void InetSetTofU (unsigned long);
extern unsigned long InetGetTofU ();
extern void InetSetUptime (unsigned long);
extern unsigned long InetGetUptime (int);
extern void InetSetDeltaUptime (unsigned long);
extern unsigned long InetGetDeltaUptime ();
extern void InetSetEpoch (long);
// get epoch time in seconds since midnight 1/1/1970
extern long InetGetEpoch ();
extern void InetSetMac (unsigned char *);
// get hardware mac address
extern void InetGetMac (unsigned char *);
extern void InetSetTZ (long);
// get the system time zone
extern long InetGetTZ ();
extern void InetSetTZText (unsigned char *);
// get the time zone text string
extern void InetGetTZText (unsigned char *);
extern void InetSetIP (unsigned char *);
// get the local ip address
extern void InetGetIP (unsigned char *);
extern void InetSetDns (unsigned char *);
// get the dns server address
extern void InetGetDns (unsigned char *);
extern void InetSetGate (unsigned char *);
// get the gateway address
extern void InetGetGate (unsigned char *);
extern void InetSetMask (unsigned char *);
// get the network mask
extern void InetGetMask (unsigned char *);
extern void InetDumper ();

