#include "w5500.h"
#include "spi.h"
#include <stdio.h>

main ()
{
  int i;
  spi_init ();
	while(1)
    {
      getSn_SR (0);
    }
}
