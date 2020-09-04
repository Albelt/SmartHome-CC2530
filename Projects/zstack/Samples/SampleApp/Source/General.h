/*********************************************************
 * GENERAL.H
 * CC2530ͨ��ͷ�ļ��������˳��õ��������͡��ꡢ������LED���ơ�KEY���
 * ���ߣ�������1701��ʫ��
 * ���䣺albeltbo@foxmail.com
 * ʱ�䣺2020.07.18
******************************************************/

#ifndef __GENERAL_H__
#define __GENERAL_H__

#include <ioCC2530.h>

/*�������Ͷ���*/
typedef unsigned char u8;
typedef unsigned int  u16;
typedef unsigned long u32;


/*�������ʱ����������32MHz��ϵͳʱ����*/
void DelayMs(u16 msec); //��ʱ������msec������
void Delay10us();		//��ʱ10us


/*LED1,2,3���ƣ�ռ��3��GPIO�� */
#define LED1    P1_0
#define LED2    P1_1
#define LED3    P1_4        //�ͷ��������ã���Ҫ���·�������������ʹ��
void LEDInit();             
//�꺯����ʹ�þ����� LED1ON;
#define LED1ON      (LED1 = 0)  
#define LED1OFF     (LED1 = 1)
#define LED1FLIP    (LED1 = ~ LED1)
#define LED2ON      (LED2 = 0)
#define LED2OFF     (LED2 = 1)
#define LED2FLIP    (LED2 = ~ LED2)
#define LED3ON      (LED3 = 0)
#define LED3OFF     (LED3 = 1)
#define LED3FLIP    (LED3 = ~ LED3)


#endif