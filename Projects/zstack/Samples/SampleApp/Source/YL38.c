#include "YL38.h"

#define DATA_PIN P0_6

u8 YL38On()
{
  if(DATA_PIN == 0) //当检测到火焰时，数据口为低电平
	return 1;
  else
	return 0;
}