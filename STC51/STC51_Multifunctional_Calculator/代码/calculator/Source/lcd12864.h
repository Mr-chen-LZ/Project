#ifndef __LCD12864_H_
#define __LCD12864_H_

#include "delay.h"
#include <math.h>

/**********************
引脚别名定义
***********************/			
sbit lcd12864_RS=P2^4;     //LCD12864 RS端 
sbit lcd12864_RW=P2^3;     //LCD12864 RW端 
sbit lcd12864_E =P2^1;     //LCD12864 E 端  

#define  BASIC_SET   0x30     //基本指令集
#define  EXTEND_SET  0x34     //扩展指令集
#define  DRAM_ON     0x36     //绘图显示开
#define  DRAM_OFF    0x34     //绘图显示关

#define SUCESS    0           //操作成功
#define PARM_ERR  1           //参数错误  


// 补充LCD底层函数声明
extern void Busy(void);
extern void Write_Cmd(uint8 cmd);
extern void Write_Data(uint8 dat);
extern uint8 Read_Data(void);  // 若需要可添加


extern void LCD_Init(void);
extern void LCD_ShowChar(uint8 row,uint8 column,uint8 chr);
extern void LCD_ShowString(uint8 row,uint8 column,uint8 num,uint8 *s);
extern void Fill_GDRAM(uint8 dat);
extern void LCD_DrawDot(uint8 x,uint8 y,uint8 color);
extern uint8 DrawHline(uint8 x0,uint8 x1,uint8 y,uint8 color);
extern uint8 DrawVline(uint8 x,uint8 y0,uint8 y1,uint8 color);
extern uint8 DrawLine(uint8 x0,uint8 y0,uint8 x1,uint8 y1,uint8 color);
extern void DrawBMP(uint8 *bmp);

#endif
