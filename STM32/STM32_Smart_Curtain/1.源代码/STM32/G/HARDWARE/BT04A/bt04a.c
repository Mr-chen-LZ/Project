#include "bt04a.h"
#include "usart.h"
#include "oled.h"
#include "uln2003.h"
#include <stdio.h>
#include <string.h>

uint8_t rx_buff[100] = {0};
uint8_t rx_buffer = 0;
uint8_t rx_index_buf = 0;
extern uint8_t mode;
extern float panduan_val[2];
extern uint8_t CL_Flag;

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
		if(strcmp((const char *)rx_buff,"B") == 0 && mode == 1)
		{
			YJ45_Start();
			CL_Flag++;
			if(CL_Flag > 1)
				CL_Flag = 0;
			rx_index_buf = 0;
		}
		if(strcmp((const char *)rx_buff,"A") == 0)
		{
			mode++;
			if(mode > 1)
				mode = 0;
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
