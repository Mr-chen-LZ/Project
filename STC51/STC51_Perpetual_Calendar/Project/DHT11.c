#include <REGX52.H>
#include "DHT11.h"

sbit Temp_data=P1^7;

unsigned int rec_dat[9];
unsigned int R_H,R_L,T_H,T_L;
unsigned char RH,RL,TH,TL,revise;

void DHT11_delay_us(unsigned char n)
{
    while(--n);
}
 
//延时ms
void DHT11_delay_ms(unsigned int z)
{
   unsigned int i,j;
   for(i=z;i>0;i--)
      for(j=114;j>0;j--);
}
//DHT11起始信号
void DHT11_start()	
{
	Temp_data=1;
	
	DHT11_delay_us(2);
	
	Temp_data=0;
	
	DHT11_delay_ms(20);
	
	Temp_data=1;
	
	DHT11_delay_us(13);
}
//接收一个字节
unsigned char DHT11_rec_byte()
{
	unsigned char i,dat;
	for(i=0;i<8;i++)
	{
			while(!Temp_data);
		  DHT11_delay_us(9);	//延迟23us
			dat<<=1;			//数据移位，为新一位数据赋值做准备
			if(Temp_data==1)	//如果为高电平，dat为1
			{
				dat+=1;
			}
			while(Temp_data);	//为低电平退出循环，接收下一位数据
	}
	return dat;
}
//接收温湿度数据
void DHT11_receive()
{
	EA = 0;
	DHT11_start();
	Temp_data=1;
	if(Temp_data==0)
	{
		while(Temp_data==0);   //等待拉高     
        DHT11_delay_us(40);  //拉高后延时85us
		
        R_H=DHT11_rec_byte();    //接收湿度高八位  
        R_L=DHT11_rec_byte();    //接收湿度低八位  
        T_H=DHT11_rec_byte();    //接收温度高八位  
        T_L=DHT11_rec_byte();    //接收温度低八位
        revise=DHT11_rec_byte(); //接收校正位
 
        DHT11_delay_us(25);    //结束
        if((R_H+R_L+T_H+T_L)==revise)      //校正
        {
            RH=R_H;
            RL=R_L;
            TH=T_H;
            TL=T_L;		
        } 
        /*数据处理，方便显示*/
        rec_dat[0]=RH;
        rec_dat[1]=RL;
        rec_dat[2]=TH;
        rec_dat[3]=TL;
	}	
	EA = 1;
}


