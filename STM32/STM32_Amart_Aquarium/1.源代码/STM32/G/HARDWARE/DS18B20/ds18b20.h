#ifndef __DS18B20_H__
#define __DS18B20_H__


/**********************************
����ͷ�ļ�
**********************************/
#include "main.h"

#define DS18B20_OUT_SET 	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_SET)
#define DS18B20_OUT_RESET 		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_RESET)

#define DS18B20_IN  		HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10)


/**********************************
��������
**********************************/
short	DS18B20_Get_Temp(void);						//DS18B20��ȡ�¶Ⱥ���
uint8_t DS18B20_Init(void);  								//DS18B20��ʼ������
uint8_t DWT_Init(void);

#endif
