#ifndef __KEY4X4_H_
#define __KEY4X4_H_

#include "delay.h"

/************************************************************************
引脚别名定义
*************************************************************************/			
sbit COL4=P5^2; 	   //4*4矩阵检测列检测端口
sbit COL3=P5^3; 	   //4*4矩阵检测列检测端口
sbit COL2=P2^6; 	   //4*4矩阵检测列检测端口
sbit COL1=P2^7; 	   //4*4矩阵检测列检测端口
sbit ROW4=P4^4; 	   //4*4矩阵检测行检测端口
sbit ROW3=P4^5; 	   //4*4矩阵检测行检测端口
sbit ROW2=P4^6; 	   //4*4矩阵检测行检测端口
sbit ROW1=P4^7; 	   //4*4矩阵检测行检测端口


// 按键值映射宏（与key_4x4.c中的key_val对应）
#define KEY_1       1
#define KEY_2       2
#define KEY_3       3
#define KEY_4       4
#define KEY_5       5
#define KEY_6       6
#define KEY_7       7
#define KEY_8       8
#define KEY_9       9
#define KEY_0       10
#define KEY_PLUS    11    // +/^
#define KEY_MINUS   12    // -/√
#define KEY_MULTI   13    // */1/x
#define KEY_EQUAL   14    // =（短按退格，长按计算）
#define KEY_AC      15    // 清除
#define KEY_DIV     16    // 除法

// 长按阈值（单位：ms，与sys_tick的1ms计数对应）
#define LONG_PRESS_THRESHOLD 500

// 声明跨文件变量（key_4x4.c中使用的is_long_press）
// extern uint8 is_long_press;  

// 统一函数返回类型为uint8_t
extern uint8 KeyScan(void);

#endif
