#include "Relay.h"

#define DATA_PIN P0_1

//继电器的IO口初始化
void RelayInit()
{
  P0SEL &= ~ 0x02;
  P0DIR |=   0x02;
}

//继电器开关打开
void RelayOn()
{
  DATA_PIN = 0;
}

//继电器开关关闭
void RelayOff()
{
 DATA_PIN = 1;
}