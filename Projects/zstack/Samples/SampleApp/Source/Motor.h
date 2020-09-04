/*********************************************************
 * MOTOR.H
 * 五线四相步进电机28BYJ-48(永磁式减速型四相八拍步进电机)，驱动芯片为ULN2003，5V供电
 * 作者：物联网1701文诗波
 * 邮箱：albeltbo@foxmail.com
 * 时间：2020.07.18
******************************************************/

/* 占用4个GPIO
 *   P0_4	A   IN1
 *   P0_5	B 	IN2
 *   P0_6	C	IN3
 *   P0_7	D	IN4
*/

#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "General.h"
#include <ioCC2530.h>

//初始化IO口
void StepMotorInit();	

/* 步进电机转动控制（单八拍）
 * dir=0为顺时针，dir=1为逆时针
 * speed为转速调节，1 <= speed <= 10
 * count为脉冲数量，一个脉冲转动45°
 */
void StepMotor(u8 dir, u8 speed, u16 count);

#endif