#ifndef __TIMER_H_
#define __TIMER_H_

#include "delay.h"  // 引入uint32定义

void Timer0_Init(void); 
extern uint32 GetSysTick(void);  
extern volatile uint32 sys_tick;  // 声明全局计时变量


#endif
