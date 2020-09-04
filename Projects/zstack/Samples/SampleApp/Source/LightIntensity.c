#include "LightIntensity.h"

#define DATA_PIN P0_5

u8 LightOn()
{
  if(DATA_PIN == 0)
	return 1;
  else
	return 0;
}