#include <stdio.h>

b(char * m)
{
	printf("%s",m);
}
a(char * m)
{
	printf("%s",m);
	b(m);
}
main ()
{
	printf("This is a message\n");
	a("this is also a message\n");
}
