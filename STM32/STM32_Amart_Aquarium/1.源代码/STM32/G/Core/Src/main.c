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
#include "bt04a.h"
#include "ds18b20.h"
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
void OLED_Poce(void);
void DATA_Poce(void);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern float ADC_VAL[2];
uint8_t yw_flag = 0;
uint8_t oled[100] ={0};
short temp = 0;
//投食舵机运行标志位
uint16_t time_ts_ms = 0;
uint8_t time_ts_s = 5;
uint8_t ts_flag = 0;
//加热棒标志位
uint8_t jr_flag = 0;
uint8_t jr_max = 28;
uint8_t jr_min = 24;
//定时抽水逻辑
uint16_t time_gl_ms = 0;
uint8_t time_gl_s = 10;
uint8_t gl_flag = 0;
//过滤水泵标志位
uint8_t cs_flag = 0;
//模式选择标志位
uint8_t mode = 0;
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
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
	OLED_Init();
	BT04A_Init();
	DS18B20_Init();
	OLED_Clear();
  /* USER CODE BEGIN 2 */
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);
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
void OLED_Poce(void)
{
	//标题
	OLED_ShowChinese(32,0,7,16,1);
	OLED_ShowChinese(48,0,8,16,1);
	OLED_ShowChinese(64,0,19,16,1);
	OLED_ShowChinese(80,0,20,16,1);
	//模式
	if(mode == 0)
	{
		OLED_ShowChinese(0,0,4,16,1);
		OLED_ShowChinese(16,0,5,16,1);
	}
	else
	{
		OLED_ShowChinese(0,0,6,16,1);
		OLED_ShowChinese(16,0,5,16,1);

	}
	//温度与TDS值和当前水位状态
	sprintf((char *)oled,"%.1f",temp*0.1f);
	OLED_ShowString(0,16,oled,16,1);
	sprintf((char *)oled,"%d",(int)ADC_VAL[0]);
	OLED_ShowString(48,16,oled,16,1);
	if(yw_flag == 0)
	{
		OLED_ShowChinese(96,16,23,16,1);
		OLED_ShowChinese(112,16,28,16,1);
	}
	else
	{
		OLED_ShowChinese(96,16,15,16,1);
		OLED_ShowChinese(112,16,16,16,1);
	}
	//抽水水泵跟过滤水泵
	//抽水
	OLED_ShowChinese(0,32,21,16,1);
	OLED_ShowChinese(16,32,22,16,1);
	OLED_ShowChar(32,32,':',16,1);
	if(cs_flag == 0)
		OLED_ShowChinese(40,32,27,16,1);
	else
		OLED_ShowChinese(40,32,26,16,1);
	//过滤
	OLED_ShowChinese(72,32,23,16,1);
	OLED_ShowChinese(88,32,24,16,1);
	OLED_ShowChar(104,32,':',16,1);
	if(gl_flag == 0)
		OLED_ShowChinese(112,32,27,16,1);
	else
		OLED_ShowChinese(112,32,26,16,1);
	//投食舵机定时启动，5s一次投食显示倒计时,抽水定时启动，10s一次投食显示倒计时
	//时间
	OLED_ShowNum(0,48,time_ts_s,2,16,1);
	OLED_ShowNum(24,48,time_gl_s,2,16,1);
	//舵机状态
	if(ts_flag == 0)
	{
		OLED_ShowChinese(48,48,32,16,1);
		OLED_ShowChinese(64,48,33,16,1);
	}
	else
	{
		OLED_ShowChinese(48,48,30,16,1);
		OLED_ShowChinese(64,48,31,16,1);
	}
	//加热棒运行情况
	if(jr_flag == 0)
	{
		OLED_ShowChinese(96,48,32,16,1);
		OLED_ShowChinese(112,48,33,16,1);
	}
	else
	{
		OLED_ShowChinese(96,48,34,16,1);
		OLED_ShowChinese(112,48,35,16,1);
	}
}

void DATA_Poce(void)
{
	adc_read();
	BT04A_DATA_Pro();
	temp = DS18B20_Get_Temp();
	if(mode == 0)
	{
		if(ADC_VAL[1] > 0.04f)//水位正常关闭水泵
		{
			yw_flag = 1;
			cs_flag = 0;
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
		}
		else//水位过低打开抽水水泵
		{
			yw_flag = 0;
			cs_flag = 1;
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);
		}
		//投食逻辑
		if(time_ts_s == 0)
		{
			Servo_SetAngle(&htim3,90);
			HAL_Delay(300);
			Servo_SetAngle(&htim3,0);
			time_ts_ms = 0;
			time_ts_s = 5;
			ts_flag = 0;
		}
		//加热逻辑
		if(temp < jr_min)
		{
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
			jr_flag = 1;
		}
		else if(temp >= jr_max)
		{
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);
			jr_flag = 0;
		}
		//TDS逻辑,超过最大值时LED亮，并执行过滤动作
		if((int)ADC_VAL[0] > 10)
		{
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
			if(gl_flag == 0)
				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
			gl_flag = 1;
		}
		else
		{
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
			if(gl_flag == 1)
				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
			gl_flag = 0;
		}
		//定时过滤逻辑
		if(time_gl_s == 0)
		{
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
			time_gl_ms = 0;
			time_gl_s = 10;
			gl_flag = 1;
		}
		else
		{
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
			gl_flag = 0;
		}
	}
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
