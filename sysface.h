extern void SetTZtxt();
extern char *GetTZtxt();
extern long GetTZ();
extern void SetTZ(long);
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
extern unsigned char InStat();
extern unsigned char InChar();
extern void OutChar(unsigned char);
/* on s100 bus machines with 88-vi/rtc board */
#define NMB	0x10
