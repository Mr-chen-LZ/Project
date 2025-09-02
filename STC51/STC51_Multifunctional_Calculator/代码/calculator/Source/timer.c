#include "timer.h"

// 定义volatile全局计时变量
volatile uint32 sys_tick = 0;

// 正确定义：12T模式下，11.0592MHz晶振，1ms定时所需的初值
// 计算公式：初值 = 65536 - (Fosc * 定时时间) / 12
// 计算过程：初值 = 65536 - (11059200 * 0.001) / 12 = 65536 - (11059.2 / 12) = 65536 - 921.6
// 我们取922个机器周期，因此初值 = 65536 - 922 = 64614
#define TIMER0_RELOAD_VALUE 64614UL

void Timer0_Init(void)
{
    AUXR &= 0x7F;   // 定时器0设置为12T模式 (最高位T0x12置0)，确保与传统8051兼容

    TMOD &= 0xF0;   // 清除T0的控制位
    TMOD |= 0x01;   // 设置T0为模式1（16位定时器）

    // 装入初值
    TH0 = (unsigned char)(TIMER0_RELOAD_VALUE / 256); // 高8位
    TL0 = (unsigned char)(TIMER0_RELOAD_VALUE % 256); // 低8位

    ET0 = 1;        // 使能定时器0中断
    EA = 1;         // 使能总中断
    TR0 = 1;        // 启动定时器0
}

void Timer0_ISR(void) interrupt 1
{
    // 中断内重新装载初值
    TH0 = (unsigned char)(TIMER0_RELOAD_VALUE / 256);
    TL0 = (unsigned char)(TIMER0_RELOAD_VALUE % 256);

    sys_tick++;     // 系统时钟滴答+1

}

uint32 GetSysTick(void)
{
    return sys_tick;
}
