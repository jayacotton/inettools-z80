void NvramSetTofU (unsigned long);
unsigned long NvramGetTofU ();
void NvramSetUptime (unsigned long);
unsigned long NvramGetUptime (int);
void NvramSetDeltaUptime (unsigned long);
unsigned long NvramGetDeltaUptime ();
void NvramSetEpoch (long);
long NvramGetEpoch ();
void NvramSetMac (unsigned int *);
void NvramGetMac (unsigned int *);
void NvramSetTZ (long);
long NvramGetTZ ();
void NvramSetTZText (unsigned char *);
void NvramGetTZText (unsigned char *);
void NvramSetIP (unsigned char *);
void NvramGetIP (unsigned char *);
void NvramSetDns (unsigned char *);
void NvramGetDns (unsigned char *);
void NvramSetGate (unsigned char *);
void NvramGetGate (unsigned char *);
void NvramSetMask (unsigned char *);
void NvramGetMask (unsigned char *);
void NvramDumper ();
