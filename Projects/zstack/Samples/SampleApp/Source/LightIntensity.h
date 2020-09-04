/*********************************************************
 * LIGHTINTENSITY.H
 * 人体红外传感器，5V供电，一根数据线，数字输入(阈值不能调节)
 * 光照强度大于一定阈值时输出低电平，小于该阈值时输出高电平
 * LightOn函数，有光返回1，否则返回0
 * 作者：物联网1701文诗波
 * 邮箱：albeltbo@foxmail.com
 * 时间：2020.07.18
******************************************************/

/* 占用1个GPIO
 * P0_5	   DATA_PIN
*/

#ifndef __LIGHTINTENSITY_H__
#define __LIGHTINTENSITY_H__

#include "General.h"
#include <ioCC2530.h>

//光照强度是否大于某个阈值，是返回1，否返回0
u8 LightOn();

#endif