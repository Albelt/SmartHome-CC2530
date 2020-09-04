#include "Motor.h"

//A,B,C,D�ĸ������λ
#define A P0_4
#define B P0_5
#define C P0_6
#define D P0_7

/* ��������ʱ����ת�����(˳ʱ��Ӻ���ǰ������)
 * ����λ��ʾABCD�ĸ������ߵ��������ĳһλ��bit=1ʱ�������ֶٹܺ�����͵�ƽ��
 * �����λ�����߾ͻᵼͨ���Ӷ���ת����תһ���Ƕ�(6.625��)
*/
u8 PHASE[8] = { 0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x09 };


void StepMotorInit()
{
  P0SEL &= 0x0f;
  P0DIR |= 0xf0;
  A = 0; B = 0; C = 0; D = 0;
}
 
void DelayUs(u8 n) //i=1Ϊ100us
{
  u8 i = 0;
  for(i=0; i<n*50; i++) //��ʱ̫�̣���������޷���������
	Delay10us();
}

void StepMotor(u8 dir, u8 speed, u16 count)
{
  u8 i,j;
  u8 delay_time; //�����ٶȣ���ʱԽ��ת��Խ����������������
  delay_time = 5 - speed; //�ٶ�Խ����ʱԽС(1 <= speed <= 3)

  for(i=0; i<count; i++) //���count������
  {
  	if(dir == 0) //��ת
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
  	else //��ת
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