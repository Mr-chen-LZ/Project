#ifndef __TIMER_H_
#define __TIMER_H_

#include "delay.h"  // ����uint32����

void Timer0_Init(void); 
extern uint32 GetSysTick(void);  
extern volatile uint32 sys_tick;  // ����ȫ�ּ�ʱ����


#endif
