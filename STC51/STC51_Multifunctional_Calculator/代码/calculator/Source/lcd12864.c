#include "lcd12864.h"	

 /***************************************************************************
 * 描  述 : LCD12864液晶判忙子程序(读状态)
 * 入  参 : 无
 * 返回值 : 无
 * 备  注 : 若LCD为“忙”状态，一直等待直到LCD空闲
 **************************************************************************/
void Busy(void)   
{   
	uint8 busy;
	do  
	{  
    //读数据			  
		lcd12864_RW = 1;          //并行的读/写选择信号：1为读,0为写    
    lcd12864_RS = 0;          //并行的指令/数据选择信号：1为数据,0为指令   		
		lcd12864_E = 1;           //并行的使能信号：0=关闭使能端,1=打开使能端
		
		P0M1=0x00;  P0M0=0x00;	  //设置P0.0~P0.7为准双向口 	
		busy=P0;                  //读取P0端口数据 
		delay_us(100);        
		lcd12864_E = 0;           //并行的使能信号：0=关闭使能端,1=打开使能端     
	}while(busy&0x80);          //判断BUSY位是否工作:1为内部在工作,0为正常状态 
}  

/***************************************************************************
 * 描  述 : LCD12864液晶模块从当前地址读RAM数据
 * 参  数 : 无
 * 返回值 : Data：读取的数据
 **************************************************************************/
uint8 Read_Data(void)
{
  uint8 Data;
  
	P0 = 0xFF;                //P0端口赋值0xFF
	Busy();                   //判忙LCD12864液晶  （设置P0.0~P0.7为准双向口）
	
	//读指令	   
	lcd12864_RW = 1;          //并行的读/写选择信号：1为读,0为写 
  lcd12864_RS = 1;          //并行的指令/数据选择信号：1为数据,0为指令   	
	lcd12864_E = 1;           //并行的使能信号：0=关闭使能端,1=打开使能端 
  
  Data=P0;                  //保存读取的数据
	delay_us(10);     
  lcd12864_E = 0;           //并行的使能信号：0=关闭使能端,1=打开使能端    
  delay_us(10); 
  return Data;
}

/***************************************************************************
 * 描  述 : LCD12864液晶命令写入程序 
 * 参  数 :  uint8 cmd：写入的命令
 * 返回值 : 无
 **************************************************************************/
void Write_Cmd(uint8 cmd) 
{      
	Busy();                   //判忙LCD12864液晶  （设置P0.0~P0.7为准双向口）
	
	//写指令	   
	lcd12864_RW = 0;          //并行的读/写选择信号：1为读,0为写 
  lcd12864_RS = 0;          //并行的指令/数据选择信号：1为数据,0为指令  

	P0M1=0x00;  P0M0=0xFF;	  //设置P0.0~P0.7为推挽输出
	P0=cmd;                   //写入命令 
	
	lcd12864_E = 1;           //并行的使能信号：0=关闭使能端,1=打开使能端 
	delay_us(10); 
  lcd12864_E = 0;           //并行的使能信号：0=关闭使能端,1=打开使能端    
  delay_us(10);  	
}

/***************************************************************************
 * 描  述 : LCD12864液晶数据写入程序 
 * 参  数 : uint8 dat：写入的数据
 * 返回值 : 无
 **************************************************************************/
void Write_Data(uint8 dat) 
{      
	Busy();                   //判忙LCD12864液晶  （设置P0.0~P0.7为准双向口）
	
	//写数据	   
	lcd12864_RW = 0;          //并行的读/写选择信号：1为读,0为写 
  lcd12864_RS = 1;          //并行的指令/数据选择信号：1为数据,0为指令  

	P0M1=0x00;  P0M0=0xFF;	  //设置P0.0~P0.7为推挽输出
	P0=dat;                   //写入数据
  
	lcd12864_E = 1;           //并行的使能信号：0=关闭使能端,1=打开使能端 
	delay_us(30); 
  lcd12864_E = 0;           //并行的使能信号：0=关闭使能端,1=打开使能端 
}  

/**************************************************************************
 * 描  述 : 指定位置显示单个字符
 * 参  数 : row[in]:行，范围1~4
 *        ：column[in]:列，范围0~7  
 *        ：chr[in]:字符编码
 * 返回值 : 无
 **************************************************************************/
void LCD_ShowChar(uint8 row,uint8 column,uint8 chr)
{      	
	uint8 Address; 
	
	//row判断第几行,column判断第几列,0x80为液晶行初始地址   
	if(row == 1)Address=0x80+column;   
	if(row == 2){Address=0x90+column;}        
	if(row == 3){Address=0x88+column;}    
	if(row == 4){Address=0x98+column;}  
	Write_Cmd(Address);  //写入地址命令到LCD12864     
	Write_Data(chr);     //写入显示数据到LCD12864    
}

/***************************************************************************
 * 描  述 : 字符串显示 
 * 参  数 : row[in]:行，范围1~4
 *        ：column[in]:列，范围0~7  
 *        ：num[in]:字符个数，注意一个汉字=2个字符
 *        ：s[in]:指向待显示的字符串
 * 返回值 : 无
 **************************************************************************/
void LCD_ShowString(uint8 row,uint8 column,uint8 num,uint8 *s)   
{     
	uint8 Address;    
	
	//row判断第几行,column判断第几列,0x80为液晶行初始地址   
	if(row == 1)Address=0x80+column;   
	if(row == 2){Address=0x90+column;}        
	if(row == 3){Address=0x88+column;}    
	if(row == 4){Address=0x98+column;}   
	Write_Cmd(Address);    //写入显示起始地址
	while(num)             //写入显示数据的大小  
	{    
		Write_Data(*(s++));  //写入显示数据到LCD12864    
		num--;         
	}   
} 

/***************************************************************************
 * 描  述 : 填充，当如数数据为0的时候即可实现清屏
 * 参  数 : uint8 dat：填充的数据
 * 返回值 : 无
**************************************************************************/
void Fill_GDRAM(uint8 dat)
{
  uint8 i,j,k;

  uint8 GDRAM_X=0x80;
  uint8 GDRAM_Y=0x80;

  for(i=0;i<2;i++)
  {
    for(j=0;j<32;j++)
    {
      for(k=0;k<8;k++)
      {
        Write_Cmd(0x34);
        Write_Cmd(GDRAM_Y+j);
        Write_Cmd(GDRAM_X+k);
        Write_Data(dat);
        Write_Data(dat);
      }
    }
    GDRAM_X=0x88;
  }

  Write_Cmd(DRAM_ON);   //开启绘图模式
  Write_Cmd(0x30);      //恢复基本指令集，关闭绘图模式
}

/**************************************************************************
 * 描  述 : LCD12864液晶任意位置画点
 * 参  数 : x[in]:x坐标，范围0~127
 *        ：y[in]:y坐标，范围0~63  
 *        ：color[in]:0=熄灭，1=点亮，2=反显
 * 返回值 : 无
 **************************************************************************/
void LCD_DrawDot(uint8 x,uint8 y,uint8 color)
{
  uint8 row,columu,x_byte,x_bit; 
  uint8 Read_H=0,Read_L=0;  
	
  Write_Cmd(0x34);  //使用扩展指令集 
  Write_Cmd(0x36);  //绘图显示开     
  
	//X轴一次操作对应16个位(2个字节)，所以要计算画的点属于哪个字节、在字节中属于哪个位
	x_byte = x>>4;    //计算将要画的点属于哪一个字节     
  x_bit  = x & 0x0F;//计算将要画的点属于哪一个位 
  
	//GDRAM将屏幕划分为上半屏和下半屏，在上半屏和下半屏中Y轴的坐标都是从0~31，所以要根据输入的
	//直角坐标计算出改点属于上半屏还是下半屏
	if(y < 32) {columu = x_byte,row=y;} //该点位于上半屏
  else              //该点位于下半屏 
  {         
	  row = y-32;         
	  columu = x_byte + 8;     
  } 
  Write_Cmd(row+0x80);       //写入行地址
  Write_Cmd(columu+0x80);    //写入列地址
  Read_Data();               //假读
  Read_H=Read_Data();        //读出将要写入的地址的数据，这样做是为了不影响屏幕上其他点的显示
  Read_L=Read_Data();   
	
	//读的时候地址会自动增加，所以要重新写一下地址
  Write_Cmd(row+0x80);
  Write_Cmd(columu+0x80);   
  if(x_bit < 8)
	{
		switch(color)
    {
       case 0:Read_H &=(~(0x01<<(7-x_bit))); break; 
       case 1:Read_H |=(0x01<<(7-x_bit)); break;
       case 2:Read_H ^=(0x01<<(7-x_bit)); break; 
       default:break;
    } 
	}
  else
	{
		switch(color)
    {
       case 0:Read_L &=(~(0x01<<(15-x_bit))); break;
       case 1:Read_L |=(0x01<<(15-x_bit)); break; 
       case 2:Read_L ^=(0x01<<(15-x_bit)); break; 
       default:break;
    } 
	}

  //写入数据	
	Write_Data(Read_H);
  Write_Data(Read_L);
  Write_Cmd(0x30);//恢复基本指令集，关闭绘图模式
}

/**************************************************************************
 * 描  述 : LCD12864液晶画水平直线
 * 参  数 : x0[in]:起点x轴坐标，范围0~127
 *        : x1[in]:终点x轴坐标，范围0~127
 *        ：y[in]:y轴坐标，范围0~63  
 *        ：color[in]:0=熄灭，1=点亮，2=反显
 * 返回值 : SUCESS：操作程序。 PARM_ERR：输入参数错误
 **************************************************************************/
uint8 DrawHline(uint8 x0,uint8 x1,uint8 y,uint8 color)
{ 
  uint8 temp;
	
  //判断输入参数是否合法
	if((x0>127)||(x1>127)||(y>63))return PARM_ERR;
	if(color>2)return PARM_ERR;

	if(x0>x1)
	{
    temp=x0;x0=x1;
    x1=temp;
  } 
  do
  { 
	  LCD_DrawDot(x0,y,color);
		x0++; 
  }
  while(x1>=x0);
	
	return SUCESS;
}

/**************************************************************************
 * 描  述 : LCD12864液晶画垂直直线
 * 参  数 : x[in]:x轴坐标，范围0~127
 *        : y0[in]:y轴起点坐标，范围0~63
 *        ：y1[in]:y轴终点坐标，范围0~63  
 *        ：color[in]:0=熄灭，1=点亮，2=反显
 * 返回值 : SUCESS：操作程序。 PARM_ERR：输入参数错误
 **************************************************************************/
uint8 DrawVline(uint8 x,uint8 y0,uint8 y1,uint8 color)
{ 
  uint8 temp;
	
	//判断输入参数是否合法
	if((x>127)||(y0>63)||(y1>63))return PARM_ERR;
	if(color>2)return PARM_ERR;
  if(y0>y1)
  { 
	  temp=y0;y1=y0;y0=temp;
  }
  do
  { 
    LCD_DrawDot(x,y0,color);
		y0++; 
  }
  while(y1>=y0);
	
	return SUCESS;
}

/**************************************************************************
 * 描  述 : LCD12864液晶画任意直线
 * 参  数 : x0[in]:x轴起点坐标，范围0~127
 *        : x1[in]:x轴终点坐标，范围0~127
 *        : y0[in]:y轴起点坐标，范围0~63
 *        ：y1[in]:y轴终点坐标，范围0~63  
 *        ：color[in]:0=熄灭，1=点亮，2=反显
 * 返回值 : 无
 **************************************************************************/
uint8 DrawLine(uint8 x0,uint8 y0,uint8 x1,uint8 y1,uint8 color)
{
  uint16 i,distance;	
	uint16 x=0,y=0,dx,dy;
  uint8 incx,incy;

	dx=x1-x0;  //计算x轴的增量
  dy=y1-y0;  //计算y轴的增量
 
  if(dx > 0)incx=1;
  else if(dx == 0)
  { 
    //x轴起点和终点相等，表示画垂直直线
    DrawVline(x0,y0,y1,color);
		return SUCESS;
	}
  else incx=-1;
	
	if(dy > 0)incy=1;
  else if(dy == 0)
  { 
    //y轴起点和终点相等，表示画水平直线
    DrawHline(x0,y0,y1,color);
    return SUCESS;
	}
  else incy=-1;
	
	//取dx,dy绝对值
	dx=abs(dx);
	dy=abs(dy);
	//角度变换，确保角度<=45°
	if(dx > dy)distance=dx;
	else distance=dy;
  
	//开始绘制直线
	for(i=0;i<=distance+1;i++)
	{
		LCD_DrawDot(x0,y0,color);
		x+=dx;
		y+=dy;
		if(x>distance)
		{
      x-=distance;
      x0+=incx;
    }
    if(y>distance)
		{
      y-=distance;
			y0+=incy;
		}
	}
	return SUCESS;
}

/***************************************************************************
 * 描  述 : 显示图片128×64
 * 参  数 : uint8 *bmp：图片
 * 返回值 : 无
**************************************************************************/
void DrawBMP(uint8 *bmp)	
{ 
  uint8 i,j;

  Write_Cmd(DRAM_OFF);        //写数据时,关闭图形显示

  for(i=0;i<32;i++)
  {
    Write_Cmd(0x80+i);        //先写入水平坐标值
    Write_Cmd(0x80);          //写入垂直坐标值
    for(j=0;j<16;j++)         //再写入两个8位元的数据    
    Write_Data(*bmp++);     
    delay_us(30); 
  }

  for(i=0;i<32;i++)
  { 
    Write_Cmd(0x80+i);
    Write_Cmd(0x88);
    for(j=0;j<16;j++)         
	  Write_Data(*bmp++);    
	  delay_us(30); 
  }
  Write_Cmd(DRAM_ON);       //写完数据,开图形显示 
}

 /***************************************************************************
 * 描  述 : LCD12864液晶初始化子程序 
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************/
 void LCD_Init(void)     
 {   
   lcd12864_E = 0;	  //关闭LCD12864模块使能端
	 delay_ms(1);                  
	 Write_Cmd(0x30);   //基本指令集     
	 delay_ms(1);      
	 Write_Cmd(0x02);   // 地址归位     
	 delay_ms(1);      
	 Write_Cmd(0x0c);   //整体显示打开,游标关闭     
	 delay_ms(1);      
	 Write_Cmd(0x06);   //游标右移     
	 delay_ms(1);      
	 Write_Cmd(0x80);   //设定显示的起始地址     
	 delay_ms(1);      
	 Write_Cmd(0x01);   //清除显示
   delay_ms(10);      // 这个延时必须要有，否则可能会出现花屏	 
 }

/*********************************END FILE********************************************/	

