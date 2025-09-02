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
#include "gpio.h"
#include "oled.h"
#include "dht11.h"
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
void OLED_Poce(void);
void DATA_Poce(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern unsigned int rec_data[4];
extern float ADC_VAL[2];
uint8_t oled[100] = {0};
//控制传感器的标志位
uint8_t yw_flag = 0;
uint8_t yw_led_flag = 0;
uint8_t yw_beep_flag = 0;
//控制行人经过的标志位
uint8_t xr_flag = 0;
uint8_t xr_led_flag = 0;
uint8_t xr_beep_flag = 0;
//控制温度的标志位
uint8_t wd_flag = 0;
uint8_t wd_led_flag = 0;
uint8_t wd_beep_flag = 0;
//led闪烁控制逻辑
uint8_t led_dis = 0;
uint8_t led_flag = 0;
uint8_t led_num = 0;
uint16_t time_led = 0;
uint16_t time_beep = 0;
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
	DWT_Init();
	OLED_Init();
	OLED_Clear();
  /* USER CODE BEGIN 2 */
	HAL_Delay(3000);
  /* USER CODE END 2 */
	DHT11_REC_Data();
	while(rec_data[2] > 60)
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
void OLED_Poce(void)
{
	//温度
	OLED_ShowChinese(0,0,37,16,1);
	OLED_ShowChinese(16,0,38,16,1);
	OLED_ShowChar(32,0,':',16,1);
	OLED_ShowNum(40,0,rec_data[2],2,16,1); 
	if(wd_flag == 0)
	{
		OLED_ShowChinese(64,0,15,16,1);
		OLED_ShowChinese(80,0,16,16,1);
	}
	else
	{
		OLED_ShowChinese(64,0,17,16,1);
		OLED_ShowChinese(80,0,18,16,1);
	}
	//烟雾
	OLED_ShowChinese(0,16,39,16,1);
	OLED_ShowChinese(16,16,40,16,1);
	OLED_ShowChar(32,16,':',16,1);
	if(yw_flag == 0 || yw_flag == 2)
	{
		OLED_ShowChinese(40,16,15,16,1);
		OLED_ShowChinese(56,16,16,16,1);
	}
	else
	{
		OLED_ShowChinese(40,16,17,16,1);
		OLED_ShowChinese(56,16,18,16,1);
	}
	//行人
	OLED_ShowChinese(0,32,41,16,1);
	OLED_ShowChinese(16,32,24,16,1);
	OLED_ShowChar(32,32,':',16,1);
	if(xr_flag == 0)
	{
		OLED_ShowChinese(40,32,23,16,1);
		OLED_ShowChinese(56,32,24,16,1);
	}
	else
	{
		OLED_ShowChinese(40,32,22,16,1);
		OLED_ShowChinese(56,32,24,16,1);
	}
}

void DATA_Poce(void)
{
	HAL_Delay(100);
	DHT11_REC_Data();
	adc_read();
	//烟雾检测
	if ((uint16_t)(ADC_VAL[0]*100) > 180 && yw_flag == 0) 
	{
		time_led = 0;
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_SET);
		HAL_Delay(300);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET);
		HAL_Delay(300);
		led_flag = 1;
		yw_flag = 1;
	}
	else if((uint16_t)(ADC_VAL[0]*100) < 160 && yw_flag == 1)
	{
		yw_flag = 0;
	}		
	//红外传感器检测行人经过
	if ((uint16_t)(ADC_VAL[1]*100) > 200 && xr_flag == 0) 
	{
		time_led = 0;
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_SET);
		HAL_Delay(300);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET);
		HAL_Delay(300);
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_SET);
		HAL_Delay(300);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET);
		led_flag = 1;
		xr_flag = 1;
	} 
	else if ((uint16_t)(ADC_VAL[1]*100) < 180 && xr_flag == 1)
	{
		xr_flag = 0;
	}		
	//温度传感器检测
	if (rec_data[2] > 28 && wd_flag == 0) 
	{
		time_led = 0;
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_SET);
		HAL_Delay(300);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET);
		HAL_Delay(300);
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_SET);
		HAL_Delay(300);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET);
		HAL_Delay(300);
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_SET);
		HAL_Delay(300);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET);
		led_flag = 1;
		wd_flag = 1;
	} 
	else if(rec_data[2] < 28 && wd_flag == 1)
	{
		wd_flag = 0;
	}
	//不管什么警报，出发后蜂鸣器启动结束触发十次led翻转
//	if(led_flag == 1)
//	{
//		if (time_led > 200) 
//		{
//			led_num++;
//			time_led = 0;
//			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);  // 直接切换状态
//			if(led_num == 10)
//			{
//				led_num = 0;
//				led_flag = 0;
//				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET);
//				if(xr_flag == 1)
//					xr_flag = 0;
//			}
//		}
//	}
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
