#include <ioCC2530.h>
#include "General.h"
#include "OnBoard.h"

void DelayMs(u16 msec)
{ 
    unsigned char i;
    while(msec--)
    {
        for(i=0;i<100;i++)
            Delay10us();
    }
}

void Delay10us()
{
  MicroWait(10); 
}


//LED1,2,3的端口初始化
void LEDInit()
{
  P1DIR |= 0x13;  //P1_0,P1_1,P1_4配置为输出口
}



