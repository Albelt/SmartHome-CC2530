/*********************************************************
 * DHT11.H
 * 温湿度传感器，3.3V供电，采用软件单总线协议
 * 读取数据时要严格按照时序来，否则会出错
 * 作者：物联网1701文诗波
 * 邮箱：albeltbo@foxmail.com
 * 时间：2020.07.18
******************************************************/

/* 占用1个GPIO
 * P0_7	   DATA_PIN
*/

#ifndef __DHT11_H__
#define __DHT11_H__

#define uchar unsigned char
extern void Delay_ms(unsigned int xms);	//延时函数
extern void COM(void);                  // 温湿写入
extern void DHT11(uchar *temp, uchar *humid);   
extern void DHT11Init();

#endif