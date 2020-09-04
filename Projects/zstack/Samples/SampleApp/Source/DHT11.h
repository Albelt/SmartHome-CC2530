/*********************************************************
 * DHT11.H
 * ��ʪ�ȴ�������3.3V���磬�������������Э��
 * ��ȡ����ʱҪ�ϸ���ʱ��������������
 * ���ߣ�������1701��ʫ��
 * ���䣺albeltbo@foxmail.com
 * ʱ�䣺2020.07.18
******************************************************/

/* ռ��1��GPIO
 * P0_7	   DATA_PIN
*/

#ifndef __DHT11_H__
#define __DHT11_H__

#define uchar unsigned char
extern void Delay_ms(unsigned int xms);	//��ʱ����
extern void COM(void);                  // ��ʪд��
extern void DHT11(uchar *temp, uchar *humid);   
extern void DHT11Init();

#endif