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
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "oled.h"
#include "sg90.h"
#include "uln2003.h"
#include "bt04a.h"
#include "rc522.h"
#include "stdio.h"
#include "string.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
void OLED_Poce(void);
void DATA_Poce(void);
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
extern volatile float ADC_VAL;

uint8_t oled[100] = {0};
char status;
uint8_t cardType[2];
uint8_t uid_0[4] = {0x00,0x00,0x00,0x00};
uint8_t uid_1[4] = {0x44,0x4C,0x3F,0x2D};
uint8_t uid_2[4] = {0x14,0x16,0x3A,0x2D};
uint8_t uid_3[4] = {0xF4,0x7E,0x65,0x2D};
//目的地完成标志位
uint8_t start_flag = 0;
//显示目的地标志位
uint8_t oled_flag = 0;
//电机启动标志位
uint8_t yj45_enable = 0;
uint8_t yj45_num = 0;
//目的地计数
uint8_t num1 = 0;
uint8_t num2 = 0;
uint8_t num3 = 0;
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
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);
  /* USER CODE BEGIN 2 */
	OLED_Init();
	BT04A_Init();
	OLED_Clear();
	DWT_Init();
	RC522_Init();
  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
		DATA_Poce();
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void DATA_Poce(void)
{
	adc_read();
	BT04A_DATA_Pro();
	status = RC522_PcdRequest(PICC_REQIDL, cardType);
	if (status == MI_OK)
	{
		 status = RC522_PcdAnticoll(uid_0);
		 if (status == MI_OK)
		 {
			 if(memcmp(uid_0,uid_1,4) == 0)
			 {
				 yj45_enable = 1;
				 oled_flag = 1;
			 }
			 else if(memcmp(uid_0,uid_2,4) == 0)
			 {
				 yj45_enable = 1;
				 oled_flag = 2;
			 }
			 else if(memcmp(uid_0,uid_3,4) == 0)
			 {
				 yj45_enable = 1;
				 oled_flag = 3;
			 }
			 else
			 {
				 yj45_enable = 0;
				 yj45_num = 0;
				 oled_flag = 0;
				 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
			 	 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
				 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_RESET);
				 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11,GPIO_PIN_RESET);
			 }
			 RC522_PcdHalt();
		 }
	}
	if(yj45_enable != 0)
	{
		YJ45_Start();
		if(ADC_VAL < 0.5f)
		{
			switch(oled_flag)
			{
				case 1:
				{
					Servo_SetAngle(1,180);
					HAL_Delay(300);
					Servo_SetAngle(1,0);
					yj45_enable = 0;
					uid_0[0] = uid_0[1] = uid_0[2] = uid_0[3] = NULL;
					oled_flag = 0;
					num1++;
					break;
				}
				case 2:
				{
					Servo_SetAngle(2,180);
					HAL_Delay(300);
					Servo_SetAngle(2,0);
					yj45_enable = 0;
					uid_0[0] = uid_0[1] = uid_0[2] = uid_0[3] = NULL;
					oled_flag = 0;
					num2++;
					break;
				}
				case 3:
				{
					Servo_SetAngle(3,180);
					HAL_Delay(300);
					Servo_SetAngle(3,0);
					yj45_enable = 0;
					uid_0[0] = uid_0[1] = uid_0[2] = uid_0[3] = NULL;
					oled_flag = 0;
					num3++;
					break;
				}
			}
		}
	}
}

void OLED_Poce(void)
{
	//目标地点显示
	OLED_ShowChinese(0,0,37,16,1);
	OLED_ShowChinese(16,0,38,16,1);
	OLED_ShowChinese(32,0,39,16,1);
	OLED_ShowChar(48,0,':',16,1);
	switch(oled_flag)
	{
		case 0:
		{
			OLED_ShowChinese(56,0,23,16,1);
			break;
		}
		case 1:
		{
			OLED_ShowChinese(56,0,42,16,1);
			break;
		}
		case 2:
		{
			OLED_ShowChinese(56,0,43,16,1);
			break;
		}
		case 3:
		{
			OLED_ShowChinese(56,0,44,16,1);
			break;
		}
	}
	//目标区域计数
	//区域一
	OLED_ShowChinese(0,16,37,16,1);
	OLED_ShowChinese(16,16,38,16,1);
	OLED_ShowChinese(32,16,39,16,1);
	OLED_ShowChinese(48,16,42,16,1);
	OLED_ShowChar(64,16,':',16,1);
	OLED_ShowNum(72,16,num1,2,16,1);
	//区域二
	OLED_ShowChinese(0,32,37,16,1);
	OLED_ShowChinese(16,32,38,16,1);
	OLED_ShowChinese(32,32,39,16,1);
	OLED_ShowChinese(48,32,43,16,1);
	OLED_ShowChar(64,32,':',16,1);
	OLED_ShowNum(72,32,num2,2,16,1);
	//区域三
	OLED_ShowChinese(0,48,37,16,1);
	OLED_ShowChinese(16,48,38,16,1);
	OLED_ShowChinese(32,48,39,16,1);
	OLED_ShowChinese(48,48,44,16,1);
	OLED_ShowChar(64,48,':',16,1);
	OLED_ShowNum(72,48,num3,2,16,1);
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
