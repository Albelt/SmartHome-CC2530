/*********************************************************
 * BEEP.H
 * ��Դ��������3.3V���͵�ƽ����
 * ���ߣ�������1701��ʫ��
 * ���䣺albeltbo@foxmail.com
 * ʱ�䣺2020.07.18
******************************************************/

/* ռ��1��GPIO
 * P2_0	   DATA_PIN
*/

#ifndef __BEEP_H__
#define __BEEP_H__

#include "General.h"
#include <ioCC2530.h>

//��������IO�ڳ�ʼ��
void BeepInit();
//��������һ��ʱ��, 1 <= time <= 255��timeԽ��ʱ��Խ��
void Beep(u8 time);


#endif