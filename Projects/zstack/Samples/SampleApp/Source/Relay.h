/*********************************************************
 * RELAY.H
 * 继电器模块
 * 3.3V工作电压，数据线接P0_1(和KEY1共用)，低电平触发
 * 作者：物联网1701文诗波
 * 邮箱：albeltbo@foxmail.com
 * 时间：2020.07.18
******************************************************/

/* 占用1个GPIO(和KEY1共用)
 * P0_1	   DATA_PIN
*/

#ifndef __RELAY_H__
#define __RELAY_H__

#include "General.h"
#include <ioCC2530.h>

//继电器的IO口初始化
void RelayInit();
//继电器开关打开
void RelayOn();
//继电器开关关闭
void RelayOff();

#endif