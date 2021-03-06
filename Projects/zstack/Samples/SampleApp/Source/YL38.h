/*********************************************************
 * YL38.H
 * YL-38火焰传感器
 * 5V供电，检测到火焰时数据线输出低电平
 * 作者：物联网1701文诗波
 * 邮箱：albeltbo@foxmail.com
 * 时间：2020.07.18
******************************************************/

/* 占用1个GPIO
 * P0_6	   DATA_PIN
*/

#ifndef __YL38_H__
#define __YL38_H__

#include "General.h"
#include <ioCC2530.h>

//传感器是否检测到火焰，是返回1，否则返回0
u8 YL38On();

#endif