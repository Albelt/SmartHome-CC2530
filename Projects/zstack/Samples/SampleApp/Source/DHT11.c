#include <ioCC2530.h>
#include "General.h"
#include "OnBoard.h"

typedef unsigned char uchar;
typedef unsigned int  uint;

#define DATA_PIN P0_7

//��ʪ�ȶ���
uchar ucharFLAG,uchartemp;
uchar ucharT_data_H,ucharT_data_L,ucharRH_data_H,ucharRH_data_L,ucharcheckdata;
uchar ucharT_data_H_temp,ucharT_data_L_temp,ucharRH_data_H_temp,ucharRH_data_L_temp,ucharcheckdata_temp;
uchar ucharcomdata;

//��ʱ����
void Delay_us() //1 us��ʱ
{
  MicroWait(1); 
}

void Delay_10us() //10 us��ʱ
{
  MicroWait(10); 
}

void Delay_ms(uint Time)//n ms��ʱ
{
    unsigned char i;
    while(Time--)
    {
        for(i=0;i<100;i++)
            Delay_10us();
    }
}

//��ʪ�ȴ���
void COM(void)    // ��ʪд��
{     
    uchar i;         
    for(i=0;i<8;i++)    
    {
        ucharFLAG=2; 
        while((!DATA_PIN)&&ucharFLAG++);
        Delay_10us();
        Delay_10us();
        Delay_10us();
        uchartemp=0;
        if(DATA_PIN)uchartemp=1;
        ucharFLAG=2;
        while((DATA_PIN)&&ucharFLAG++);   
        if(ucharFLAG==1)break;    
        ucharcomdata<<=1;
        ucharcomdata|=uchartemp; 
    }    
}

void DHT11Init()
{
    P0SEL &= ~ 0x80;
    P0DIR &= ~ 0x80;
}

void DHT11(uchar *temp, uchar *humid)   
{
    DATA_PIN=0;
    Delay_ms(19);  //>18MS
    DATA_PIN=1; 
    P0DIR &= ~0x80; //��������IO�ڷ���
    Delay_10us();
    Delay_10us();                        
    Delay_10us();
    Delay_10us();  
    if(!DATA_PIN) 
    {
        ucharFLAG=2; 
        while((!DATA_PIN)&&ucharFLAG++);
        ucharFLAG=2;
        while((DATA_PIN)&&ucharFLAG++); 
        COM();
        ucharRH_data_H_temp=ucharcomdata;
        COM();
        ucharRH_data_L_temp=ucharcomdata;
        COM();
        ucharT_data_H_temp=ucharcomdata;
        COM();
        ucharT_data_L_temp=ucharcomdata;
        COM();
        ucharcheckdata_temp=ucharcomdata;
        DATA_PIN=1; 
        uchartemp=(ucharT_data_H_temp+ucharT_data_L_temp+ucharRH_data_H_temp+ucharRH_data_L_temp);
        if(uchartemp==ucharcheckdata_temp)
        {
            *humid=ucharRH_data_H_temp;
            *temp=ucharT_data_H_temp;
        }   
    } 
    else //û�óɹ���ȡ������0
    {
            *humid = 0;
			*temp = 0;
    } 
    
    P0DIR |= 0x80; //IO����Ҫ�������� 
	Delay_ms(1000);
}
