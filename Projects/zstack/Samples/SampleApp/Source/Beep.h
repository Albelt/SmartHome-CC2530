/*********************************************************
 * BEEP.H
 * 无源蜂鸣器，3.3V，低电平触发
 * 作者：物联网1701文诗波
 * 邮箱：albeltbo@foxmail.com
 * 时间：2020.07.18
******************************************************/

/* 占用1个GPIO
 * P2_0	   DATA_PIN
*/

#ifndef __BEEP_H__
#define __BEEP_H__

#include "General.h"
#include <ioCC2530.h>

//蜂鸣器的IO口初始化
void BeepInit();
//蜂鸣器响一段时间, 1 <= time <= 255，time越大时间越长
void Beep(u8 time);


#endif