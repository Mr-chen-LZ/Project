#include "bt04a.h"
#include "usart.h"
#include "oled.h"
#include "uln2003.h"
#include <stdio.h>
#include <string.h>

uint8_t rx_buff[100] = {0};
uint8_t rx_buffer = 0;
uint8_t rx_index_buf = 0;

extern uint8_t num1;
extern uint8_t num2;
extern uint8_t num3;
uint8_t data[100] = {0};
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	rx_buff[rx_index_buf] = rx_buffer;
	rx_index_buf++;
	HAL_UART_Receive_IT(&huart2, &rx_buffer, 1);
}



void BT04A_Init(void)
{
	HAL_UART_Receive_IT(&huart2, &rx_buffer, 1);
}

void BT04A_DATA_Pro(void)
{
	if(rx_index_buf != 0)
	{
		if(rx_buff[0] == '1')
		{
			sprintf((char *)data,"Num1:%d,Num2:%d,Num3:%d\r\n",num1,num2,num3);
			BT04A_SendString((const char *)data);
			rx_index_buf = 0;
		}
		else
		{
			rx_buff[0] = NULL;
			rx_index_buf = 0;
		}
	}
}

void BT04A_SendString(const char *str)
{
	while (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE) == RESET);
	HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen((const char *)str), HAL_MAX_DELAY);
}
