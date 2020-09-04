#include "Beep.h"

#define DATA_PIN P2_0

void BeepInit()
{
  P2SEL &= ~ 0x01;
  P2DIR |=   0x01;
}

void Beep(u8 time)
{
  u8 i;
  for(i=0; i<time; i++)
  {
	DATA_PIN = 1;
	Delay10us();
	DATA_PIN = 0;
	Delay10us(); Delay10us(); Delay10us(); Delay10us(); Delay10us(); Delay10us(); Delay10us(); Delay10us();
  }
}