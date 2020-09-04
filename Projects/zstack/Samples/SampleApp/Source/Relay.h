/*********************************************************
 * RELAY.H
 * �̵���ģ��
 * 3.3V������ѹ�������߽�P0_1(��KEY1����)���͵�ƽ����
 * ���ߣ�������1701��ʫ��
 * ���䣺albeltbo@foxmail.com
 * ʱ�䣺2020.07.18
******************************************************/

/* ռ��1��GPIO(��KEY1����)
 * P0_1	   DATA_PIN
*/

#ifndef __RELAY_H__
#define __RELAY_H__

#include "General.h"
#include <ioCC2530.h>

//�̵�����IO�ڳ�ʼ��
void RelayInit();
//�̵������ش�
void RelayOn();
//�̵������عر�
void RelayOff();

#endif