struct TICKSTORE {
	union{
		unsigned long	time;
		unsigned int de_hl[2];
	}t;
};

#define FORMAT_RAW	0
#define FORMAT_MILLI	1
#define FORMAT_TIME	2

#define TICK  .02

#define k_sysget 248
#define k_gettimer 208

extern void gettick(struct TICKSTORE *ticks);
extern void SetTime(struct TICKSTORE *out);	
extern void GetTime(struct TICKSTORE *out, struct TICKSTORE *in);
extern void WaitTime(unsigned int time);
extern void FormatTime(char format, char *out, struct TICKSTORE *in);
