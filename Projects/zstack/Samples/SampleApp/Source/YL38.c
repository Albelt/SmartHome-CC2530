#include "YL38.h"

#define DATA_PIN P0_6

u8 YL38On()
{
  if(DATA_PIN == 0) //����⵽����ʱ�����ݿ�Ϊ�͵�ƽ
	return 1;
  else
	return 0;
}