/*********************************************************
 * GENERAL.H
 * CC2530通用头文件，定义了常用的数据类型、宏、函数、LED控制、KEY检测
 * 作者：物联网1701文诗波
 * 邮箱：albeltbo@foxmail.com
 * 时间：2020.07.18
******************************************************/

#ifndef __GENERAL_H__
#define __GENERAL_H__

#include <ioCC2530.h>

/*数据类型定义*/
typedef unsigned char u8;
typedef unsigned int  u16;
typedef unsigned long u32;


/*下面的延时函数都是在32MHz的系统时钟下*/
void DelayMs(u16 msec); //延时函数，msec：毫秒
void Delay10us();		//延时10us


/*LED1,2,3控制，占用3个GPIO口 */
#define LED1    P1_0
#define LED2    P1_1
#define LED3    P1_4        //和仿真器共用，需要拔下仿真器才能正常使用
void LEDInit();             
//宏函数，使用举例： LED1ON;
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