#include <stdio.h>
#include <stdlib.h>
#include "sysface.h"

#define O_Year 0
#define O_Month 1
#define O_Date 2
#define O_Hour 3
#define O_Minute 4
#define O_Second 5

#define ASCII_ESC 27

unsigned char *colon[5] = {
  "       ",
  "  ::   ",
  "       ",
  "  ::   ",
  "       "
};

unsigned char *zero[5] = {
  " 0000  ",
  "00  00 ",
  "00  00 ",
  "00  00 ",
  " 0000  "
};

unsigned char *one[5] = {
  "1111   ",
  "  11   ",
  "  11   ",
  "  11   ",
  "111111 "
};

unsigned char *two[5] = {
  " 2222  ",
  "22  22 ",
  "   22  ",
  "  22   ",
  "222222 "
};

unsigned char *three[5] = {
  " 3333  ",
  "33  33 ",
  "   333 ",
  "33  33 ",
  " 3333  "
};

unsigned char *four[5] = {
  "44  44 ",
  "44  44 ",
  "444444 ",
  "    44 ",
  "    44 "
};

unsigned char *five[5] = {
  "555555 ",
  "55     ",
  "55555  ",
  "    55 ",
  "55555  "
};

unsigned char *six[5] = {
  " 6666  ",
  "66     ",
  "66666  ",
  "66  66 ",
  " 6666  "
};

unsigned char *seven[5] = {
  "777777 ",
  "   77  ",
  "  77   ",
  " 77    ",
  "77     "
};

unsigned char *eight[5] = {
  " 8888  ",
  "88  88 ",
  " 8888  ",
  "88  88 ",
  " 8888  "
};

unsigned char *nine[5] = {
  " 9999  ",
  "99  99 ",
  " 99999 ",
  "    99 ",
  " 9999  "
};

unsigned char **numbers[] = {
  zero,
  one,
  two,
  three,
  four,
  five,
  six,
  seven,
  eight,
  nine,
  colon
};

unsigned char *bcd_buffer;
void
draw (unsigned char *digit[], int line)
{
  printf ("%s", digit[line]);
}

void
main ()
{
  int i;
  while (1)
    {
      if (bdos (CPM_ICON, 0))
	exit (99);
      bcd_buffer = GetTOD ();
      for (i = 0; i < 5; i++)
	{
	  // hours
	  draw (numbers[((bcd_buffer[O_Hour] >> 4) & 0xf)], i);
	  draw (numbers[(bcd_buffer[O_Hour] & 0xf)], i);
	  draw (numbers[10], i);
	  // minutes
	  draw (numbers[((bcd_buffer[O_Minute] >> 4) & 0xf)], i);
	  draw (numbers[(bcd_buffer[O_Minute] & 0xf)], i);
	  draw (numbers[10], i);
	  // seconds
	  draw (numbers[((bcd_buffer[O_Second] >> 4) & 0xf)], i);
	  draw (numbers[(bcd_buffer[O_Second] & 0xf)], i);
	  printf ("\n");
	}
	printf("%c[A",ASCII_ESC);
	printf("%c[A",ASCII_ESC);
	printf("%c[A",ASCII_ESC);
	printf("%c[A",ASCII_ESC);
	printf("%c[A",ASCII_ESC);
    }
}
