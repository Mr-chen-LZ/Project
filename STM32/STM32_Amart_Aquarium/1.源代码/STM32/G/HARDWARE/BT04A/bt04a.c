#include "bt04a.h"
#include "usart.h"
#include "oled.h"
#include "sg90.h"
#include "tim.h"
#include <stdio.h>
#include <string.h>

uint8_t rx_buff[100] = {0};
uint8_t rx_buffer = 0;
uint8_t rx_index_buf = 0;
uint8_t data[100] = {0};
extern uint8_t mode;

 
extern short temp;
//投食舵机运行标志位
extern uint8_t ts_flag;
//加热棒标志位
extern uint8_t jr_flag;
//定时抽水逻辑
extern uint8_t gl_flag;
//过滤水泵标志位
extern uint8_t cs_flag;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	rx_buff[rx_index_buf] = rx_buffer;
	rx_index_buf++;
	HAL_UART_Receive_IT(&huart2, &rx_buffer, 1);
}


/* 初始化函数 */
void BT04A_Init(void)
{
	HAL_UART_Receive_IT(&huart2, &rx_buffer, 1);
}

void BT04A_DATA_Pro(void)
{
	if(rx_index_buf != 0)
	{
		if(rx_buff[0] == '0')
		{
			mode = 0;
			rx_index_buf = 0;
		}
		else if(rx_buff[0] == '1')
		{
			mode = 1;
			rx_index_buf = 0;
		}
		else if(rx_buff[0] == '2')
		{
			//抽水水泵，过滤水泵，投食舵机，加热片
			sprintf((char *)data,"cs_sb:%s,gl_sb:%s,ts_duoji:%s,jr:%s,temp:%.1f",cs_flag?"guan":"kai",gl_flag?"guan":"kai",ts_flag?"guan":"kai",jr_flag?"guan":"kai",temp*0.1f);
			BT04A_SendString((const char *)data);
			rx_index_buf = 0;
		}
		else if(mode == 1 && rx_buff[0] == '3')//手动投食
		{
			Servo_SetAngle(&htim3,90);
			HAL_Delay(300);
			Servo_SetAngle(&htim3,0);
			rx_index_buf = 0;
		}
		else
		{
			rx_buff[0] = NULL;
			rx_index_buf = 0;
		}
	}
}

//发送数据
void BT04A_SendString(const char *str)
{
	while (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE) == RESET);
	HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen((const char *)str), HAL_MAX_DELAY);
}
