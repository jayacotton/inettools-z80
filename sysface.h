extern unsigned long GetTZ();
extern void SetTZ(unsigned long);
extern unsigned long GetUptime(int);
extern int TestBIOS();
extern void EpochSet(unsigned long);
extern long EpochGet();
extern void SetNvram(int,unsigned char);
extern unsigned char GetNvram(int );
extern void SetDeltaUptime(unsigned long );
extern unsigned long GetDeltaUptime();
extern unsigned long GetUptime();
extern void SetUptime(unsigned long time);
extern void SetTOD(unsigned char *);
extern unsigned char *GetTOD();
extern unsigned int RTCCount();
extern unsigned char *RTCType();
