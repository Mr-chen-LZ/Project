#include "timer.h"

// ����volatileȫ�ּ�ʱ����
volatile uint32 sys_tick = 0;

// ��ȷ���壺12Tģʽ�£�11.0592MHz����1ms��ʱ����ĳ�ֵ
// ���㹫ʽ����ֵ = 65536 - (Fosc * ��ʱʱ��) / 12
// ������̣���ֵ = 65536 - (11059200 * 0.001) / 12 = 65536 - (11059.2 / 12) = 65536 - 921.6
// ����ȡ922���������ڣ���˳�ֵ = 65536 - 922 = 64614
#define TIMER0_RELOAD_VALUE 64614UL

void Timer0_Init(void)
{
    AUXR &= 0x7F;   // ��ʱ��0����Ϊ12Tģʽ (���λT0x12��0)��ȷ���봫ͳ8051����

    TMOD &= 0xF0;   // ���T0�Ŀ���λ
    TMOD |= 0x01;   // ����T0Ϊģʽ1��16λ��ʱ����

    // װ���ֵ
    TH0 = (unsigned char)(TIMER0_RELOAD_VALUE / 256); // ��8λ
    TL0 = (unsigned char)(TIMER0_RELOAD_VALUE % 256); // ��8λ

    ET0 = 1;        // ʹ�ܶ�ʱ��0�ж�
    EA = 1;         // ʹ�����ж�
    TR0 = 1;        // ������ʱ��0
}

void Timer0_ISR(void) interrupt 1
{
    // �ж�������װ�س�ֵ
    TH0 = (unsigned char)(TIMER0_RELOAD_VALUE / 256);
    TL0 = (unsigned char)(TIMER0_RELOAD_VALUE % 256);

    sys_tick++;     // ϵͳʱ�ӵδ�+1

}

uint32 GetSysTick(void)
{
    return sys_tick;
}
