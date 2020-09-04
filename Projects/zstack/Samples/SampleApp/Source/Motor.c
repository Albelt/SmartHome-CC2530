#include "Motor.h"

//A,B,C,D四个输出相位
#define A P0_4
#define B P0_5
#define C P0_6
#define D P0_7

/* 单八拍逆时针旋转相序表(顺时针从后往前读即可)
 * 低四位表示ABCD四根数据线的输出，当某一位的bit=1时经过达林顿管后输出低电平，
 * 这个相位的绕线就会导通，从而让转子旋转一个角度(6.625°)
*/
u8 PHASE[8] = { 0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x09 };


void StepMotorInit()
{
  P0SEL &= 0x0f;
  P0DIR |= 0xf0;
  A = 0; B = 0; C = 0; D = 0;
}
 
void DelayUs(u8 n) //i=1为100us
{
  u8 i = 0;
  for(i=0; i<n*50; i++) //延时太短，步进电机无法正常运行
	Delay10us();
}

void StepMotor(u8 dir, u8 speed, u16 count)
{
  u8 i,j;
  u8 delay_time; //控制速度，延时越大转速越慢，但是力矩增大
  delay_time = 5 - speed; //速度越大，延时越小(1 <= speed <= 3)

  for(i=0; i<count; i++) //输出count个脉冲
  {
  	if(dir == 0) //正转
  	{
	  for(j=8; j>0; j--)
	  {
	    A = (PHASE[j-1]     ) & 0x01;
	    B = (PHASE[j-1] >> 1) & 0x01;
	    C = (PHASE[j-1] >> 2) & 0x01;
	    D = (PHASE[j-1] >> 3) & 0x01;
		DelayUs(delay_time);
	  }
  	}
  	else //反转
  	{
	  for(j=0; j<8; j++)
	  {
	    A = (PHASE[j]     ) & 0x01;
	    B = (PHASE[j] >> 1) & 0x01;
	    C = (PHASE[j] >> 2) & 0x01;
	    D = (PHASE[j] >> 3) & 0x01;
		DelayUs(delay_time);
	  }
  	}
  }
}