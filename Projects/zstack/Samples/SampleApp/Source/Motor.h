/*********************************************************
 * MOTOR.H
 * �������ಽ�����28BYJ-48(����ʽ������������Ĳ������)������оƬΪULN2003��5V����
 * ���ߣ�������1701��ʫ��
 * ���䣺albeltbo@foxmail.com
 * ʱ�䣺2020.07.18
******************************************************/

/* ռ��4��GPIO
 *   P0_4	A   IN1
 *   P0_5	B 	IN2
 *   P0_6	C	IN3
 *   P0_7	D	IN4
*/

#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "General.h"
#include <ioCC2530.h>

//��ʼ��IO��
void StepMotorInit();	

/* �������ת�����ƣ������ģ�
 * dir=0Ϊ˳ʱ�룬dir=1Ϊ��ʱ��
 * speedΪת�ٵ��ڣ�1 <= speed <= 10
 * countΪ����������һ������ת��45��
 */
void StepMotor(u8 dir, u8 speed, u16 count);

#endif