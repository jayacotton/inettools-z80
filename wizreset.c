#include "w5500.h"

void main()
{
  spi_init();
  setMR (MR_RST);
  getMR();
}

