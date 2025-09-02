#include "bt04a.h"
#include "usart.h"
#include "oled.h"
#include "uln2003.h"
#include "SG90.h"
#include "tim.h"
#include <stdio.h>
#include <string.h>

uint8_t rx_buff[100] = {0};
uint8_t rx_buffer = 0;
uint8_t rx_index_buf = 0;

extern uint16_t cl_max;
extern int TIME[3];
extern uint8_t mode;
extern uint8_t ch_flag;
extern uint8_t cl_flag;
extern uint8_t ren_flag;
extern uint8_t koqi_flag;
extern uint8_t yu_flag;
uint8_t data[100] = {0};

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

void BT04A_DATA_Pro(void) {
    // 指令解析
    switch (rx_buff[0]) 
			{
        case '1':
            mode = 0;  // 自动模式
						rx_buff[0] = NULL;
				    rx_index_buf = 0;
            break;
        case '2':
            mode = 1;  // 手动模式
						rx_buff[0] = NULL;
				    rx_index_buf = 0;
            break;
        case '3':
            if (mode == 1) 
						{
							ch_flag = 1;
							Servo_SetAngle(&htim3,90);  // 开窗户
						}
						rx_buff[0] = NULL;
				    rx_index_buf = 0;
            break;
        case '4':
            if (mode == 1) 
						{
							ch_flag = 0;
							Servo_SetAngle(&htim3,0);  // 关窗户
						}
						rx_buff[0] = NULL;
				    rx_index_buf = 0;
            break;
        case '5':
            if (mode == 1) 
							cl_flag = 1;  // 开窗帘
						rx_buff[0] = NULL;
				    rx_index_buf = 0;
            break;
        case '6':
            if (mode == 1) 
							cl_flag = 0;  // 关窗帘
						rx_buff[0] = NULL;
				    rx_index_buf = 0;
            break;
				case '7':
				{
						sprintf((char *)data,"%s,%s,%s",yu_flag ? "youyu" : "wuyu",koqi_flag ? "jingao" : "zhengchang",ren_flag ?  "youren" : "wuren" );
						BT04A_SendString((const char *)data);
						rx_buff[0] = NULL;
				    rx_index_buf = 0;
					break;
				}
        default:
						rx_buff[0] = NULL;
				    rx_index_buf = 0;
            break;
    }
}

//发送数据
void BT04A_SendString(const char *str)
{
	while (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE) == RESET);
	HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen((const char *)str), HAL_MAX_DELAY);
}
