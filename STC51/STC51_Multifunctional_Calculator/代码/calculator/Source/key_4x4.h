#ifndef __KEY4X4_H_
#define __KEY4X4_H_

#include "delay.h"

/************************************************************************
���ű�������
*************************************************************************/			
sbit COL4=P5^2; 	   //4*4�������м��˿�
sbit COL3=P5^3; 	   //4*4�������м��˿�
sbit COL2=P2^6; 	   //4*4�������м��˿�
sbit COL1=P2^7; 	   //4*4�������м��˿�
sbit ROW4=P4^4; 	   //4*4�������м��˿�
sbit ROW3=P4^5; 	   //4*4�������м��˿�
sbit ROW2=P4^6; 	   //4*4�������м��˿�
sbit ROW1=P4^7; 	   //4*4�������м��˿�


// ����ֵӳ��꣨��key_4x4.c�е�key_val��Ӧ��
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
#define KEY_MINUS   12    // -/��
#define KEY_MULTI   13    // */1/x
#define KEY_EQUAL   14    // =���̰��˸񣬳������㣩
#define KEY_AC      15    // ���
#define KEY_DIV     16    // ����

// ������ֵ����λ��ms����sys_tick��1ms������Ӧ��
#define LONG_PRESS_THRESHOLD 500

// �������ļ�������key_4x4.c��ʹ�õ�is_long_press��
// extern uint8 is_long_press;  

// ͳһ������������Ϊuint8_t
extern uint8 KeyScan(void);

#endif
