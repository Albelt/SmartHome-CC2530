#include "HCSR501.h"

#define DATA_PIN P0_4

u8 HCSR501On()
{
  if(DATA_PIN == 1)
  	return 1;
  else
	return 0;
}