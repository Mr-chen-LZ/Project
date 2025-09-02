/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "oled.h"
#include "key.h"
#include "stdio.h"
#include "string.h"
#include "esp8266.h"
#include "onenet.h"
#include "mqttkit.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
void OLED_Poce(void);
void KEY_Poce(void);
void DATA_Poce(void);
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char PUB_BUF[256];
unsigned char *dataPtr = NULL;
uint8_t key = 0;

uint8_t OLED[128] = {0};
uint8_t seat_Sate[4] = {0};
uint8_t seat_Flag[4] = {0};
uint8_t seat_Weiyue[4] = {0};

uint16_t time_Seat1[2] = {0};
uint16_t time_Seat2[2] = {0};
uint16_t time_Seat3[2] = {0};
uint16_t time_Seat4[2] = {0};
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
	OLED_Init();
  ESP8266_Init();
	HAL_Delay(500);
	OLED_Clear();	
  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
		DATA_Poce();
		KEY_Poce();
		OLED_Poce();
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void OLED_Poce(void)
{
	sprintf((char *)OLED,"    Tu_Shu_Guan");
	OLED_ShowString(0,0,OLED,12,1);
	switch(seat_Sate[0])
	{
		case 0:sprintf((char *)OLED,"Seat:Wu_Ren    ");break;
		case 1:sprintf((char *)OLED,"Seat:Yi_Yu_Yue    ");break;
		case 2:sprintf((char *)OLED,"Seat:You_Ren    ");break;
	}
	OLED_ShowString(0,12,OLED,12,1);
	switch(seat_Sate[1])
	{
		case 0:sprintf((char *)OLED,"Seat:Wu_Ren    ");break;
		case 1:sprintf((char *)OLED,"Seat:Yi_Yu_Yue    ");break;
		case 2:sprintf((char *)OLED,"Seat:You_Ren    ");break;
	}
	OLED_ShowString(0,24,OLED,12,1);
	switch(seat_Sate[2])
	{
		case 0:sprintf((char *)OLED,"Seat:Wu_Ren    ");break;
		case 1:sprintf((char *)OLED,"Seat:Yi_Yu_Yue    ");break;
		case 2:sprintf((char *)OLED,"Seat:You_Ren    ");break;
	}
	OLED_ShowString(0,36,OLED,12,1);
	switch(seat_Sate[3])
	{
		case 0:sprintf((char *)OLED,"Seat:Wu_Ren    ");break;
		case 1:sprintf((char *)OLED,"Seat:Yi_Yu_Yue    ");break;
		case 2:sprintf((char *)OLED,"Seat:You_Ren    ");break;
	}
	OLED_ShowString(0,48,OLED,12,1);
}

void KEY_Poce(void)
{
	uint8_t key1;
	key1 = key_read();
	if(key1 != key)
		key = key1;
	else
		key1 = 0;
	switch(key1)
	{
		case 1:
		{
			if(seat_Flag[0] == 1)//当预约标志位置1是有效，将座位状态改为签到，并将预约标志位改为已签到
			{
				seat_Flag[0] = 2;
				seat_Sate[0] = 2;
				time_Seat1[0] = time_Seat1[1] = 0;
				led_dis(1,1);
			}
			break;
		}
		case 2:
		{
			if(seat_Flag[1] == 1)//当预约标志位置1是有效，将座位状态改为签到，并将预约标志位改为已签到
			{
				time_Seat2[0] = time_Seat2[1] = 0;
				seat_Flag[1] = 2;
				seat_Sate[1] = 2;
				led_dis(2,1);
			}
			break;
		}
		case 3:
		{
			if(seat_Flag[2] == 1)//当预约标志位置1是有效，将座位状态改为签到，并将预约标志位改为已签到
			{
				time_Seat3[0] = time_Seat3[1] = 0;
				seat_Flag[2] = 2;
				seat_Sate[2] = 2;
				led_dis(3,1);
			}
			break;
		}
		case 4:
		{
			if(seat_Flag[3] == 1)//当预约标志位置1是有效，将座位状态改为签到，并将预约标志位改为已签到
			{
				time_Seat4[0] = time_Seat4[1] = 0;
				seat_Flag[3] = 2;
				seat_Sate[3] = 2;
				led_dis(4,1);
			}
			break;
		}
	}
}

void DATA_Poce(void)
{
	dataPtr = ESP8266_GetIPD(3);//检测手机端数据返回
	if(dataPtr != NULL)
	{
		if(strstr((const char *)dataPtr, "ChaXun") != NULL)//查询座位信息
		{
			sprintf(PUB_BUF,"Seat1:%s,Seat2:%s,Seat3:%s,Seat4:%s",seat_Sate[0]?"Bu_Ke_Yu_Yue":"Wu_Ren",seat_Sate[1]?"Bu_Ke_Yu_Yue":"Wu_Ren",
			seat_Sate[2]?"Bu_Ke_Yu_Yue":"Wu_Ren",seat_Sate[3]?"Bu_Ke_Yu_Yue":"Wu_Ren");
			ESP8266_SendData((unsigned char *)PUB_BUF,strlen(PUB_BUF));
		}
		else if(strstr((const char *)dataPtr, "YuYue1") != NULL)//预约座位1
		{
			if(seat_Sate[0] == 0)//座位1可预约返回成功
			{
				sprintf(PUB_BUF,"Zuo_Wei_1_Yu_Yue_Chen_Gon");
				ESP8266_SendData((unsigned char *)PUB_BUF,strlen(PUB_BUF));
				time_Seat1[0] = time_Seat1[1] = 0;
				seat_Flag[0] = 1;
				seat_Sate[0] = 1;
			}
			else//座位1已被预约或已签到返回失败
			{
				sprintf(PUB_BUF,"Zuo_Wei_1_Yu_Yue_Shi_Bai");
				ESP8266_SendData((unsigned char *)PUB_BUF,strlen(PUB_BUF));
			}
		}
		else if(strstr((const char *)dataPtr, "YuYue2") != NULL)//预约座位2
		{
			if(seat_Sate[1] == 0)//座位2可预约返回成功
			{
				sprintf(PUB_BUF,"Zuo_Wei_2_Yu_Yue_Chen_Gon");
				ESP8266_SendData((unsigned char *)PUB_BUF,strlen(PUB_BUF));
				time_Seat2[0] = time_Seat2[1] = 0;
				seat_Flag[1] = 1;
				seat_Sate[1] = 1;
			}
			else//座位2已被预约或已签到返回失败
			{
				sprintf(PUB_BUF,"Zuo_Wei_2_Yu_Yue_Shi_Bai");
				ESP8266_SendData((unsigned char *)PUB_BUF,strlen(PUB_BUF));
			}
		}
		else if(strstr((const char *)dataPtr, "YuYue3") != NULL)//预约座位3
		{
			if(seat_Sate[2] == 0)//座位3可预约返回成功
			{
				sprintf(PUB_BUF,"Zuo_Wei_3_Yu_Yue_Chen_Gon");
				ESP8266_SendData((unsigned char *)PUB_BUF,strlen(PUB_BUF));
				time_Seat3[0] = time_Seat3[1] = 0;
				seat_Flag[2] = 1;
				seat_Sate[2] = 1;
			}
			else//座位3已被预约或已签到返回失败
			{
				sprintf(PUB_BUF,"Zuo_Wei_3_Yu_Yue_Shi_Bai");
				ESP8266_SendData((unsigned char *)PUB_BUF,strlen(PUB_BUF));
			}
		}
		else if(strstr((const char *)dataPtr, "YuYue4") != NULL)//预约座位4
		{
			if(seat_Sate[3] == 0)//座位4可预约返回成功
			{
				sprintf(PUB_BUF,"Zuo_Wei_4_Yu_Yue_Chen_Gon");
				ESP8266_SendData((unsigned char *)PUB_BUF,strlen(PUB_BUF));
				time_Seat4[0] = time_Seat4[1] = 0;
				seat_Flag[3] = 1;
				seat_Sate[3] = 1;
			}
			else//座位4已被预约或已签到返回失败
			{
				sprintf(PUB_BUF,"Zuo_Wei_4_Yu_Yue_Shi_Bai");
				ESP8266_SendData((unsigned char *)PUB_BUF,strlen(PUB_BUF));
			}
		}
	}
	if(seat_Weiyue[0] == 1)//迟到标志位置1时给客户端发送迟到信息，并将相关标志位回退
	{
		sprintf(PUB_BUF,"Zuo_Wei_1_Chi_Dap");
		ESP8266_SendData((unsigned char *)PUB_BUF,strlen(PUB_BUF));
		seat_Weiyue[0] = 0;
		seat_Sate[0] = 0;
	}
	if(seat_Weiyue[1] == 1)//迟到标志位置1时给客户端发送迟到信息，并将相关标志位回退
	{
		sprintf(PUB_BUF,"Zuo_Wei_2_Chi_Dap");
		ESP8266_SendData((unsigned char *)PUB_BUF,strlen(PUB_BUF));
		seat_Weiyue[0] = 0;
		seat_Sate[1] = 0;
	}
	if(seat_Weiyue[2] == 1)//迟到标志位置1时给客户端发送迟到信息，并将相关标志位回退
	{
		sprintf(PUB_BUF,"Zuo_Wei_3_Chi_Dap");
		ESP8266_SendData((unsigned char *)PUB_BUF,strlen(PUB_BUF));
		seat_Weiyue[2] = 0;
		seat_Sate[2] = 0;
	}
	if(seat_Weiyue[3] == 1)//迟到标志位置1时给客户端发送迟到信息，并将相关标志位回退
	{
		sprintf(PUB_BUF,"Zuo_Wei_4_Chi_Dap");
		ESP8266_SendData((unsigned char *)PUB_BUF,strlen(PUB_BUF));
		seat_Weiyue[3] = 0;
		seat_Sate[3] = 0;
	}
	if(seat_Sate[0] == 0) led_dis(1,0);
	if(seat_Sate[1] == 0) led_dis(2,0);
	if(seat_Sate[2] == 0) led_dis(3,0);
	if(seat_Sate[3] == 0) led_dis(4,0);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
