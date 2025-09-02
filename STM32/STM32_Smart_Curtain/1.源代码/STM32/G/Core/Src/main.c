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
#include "usart.h"
#include "gpio.h"
#include "stdio.h"
#include "string.h"
#include "oled.h"
#include "uln2003.h"
#include "ds1302.h"
#include "bt04a.h"
#include "dht11.h"
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
void OLED_Pro(void);
void DATA_Pro(void);
void ALARM_Pro(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern float ADC_VAL[2];
extern struct TIMEData TimeData;
extern int rec_data[4];
uint8_t mode = 0;
uint8_t oled[100] = {0};
uint8_t GM_Flag = 0;//窗帘开关标志位
uint8_t JG_Flag = 0;
uint8_t CL_Flag = 0;
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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */
	BT04A_Init();
	ds1302_init();
	OLED_Init();
	OLED_Clear();
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
		DATA_Pro();
		OLED_Pro();
		ALARM_Pro();
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
void OLED_Pro(void)
{
	//标题
	OLED_ShowChinese(16,0,7,16,1);
	OLED_ShowChinese(32,0,8,16,1);
	OLED_ShowChinese(48,0,9,16,1);
	OLED_ShowChinese(64,0,10,16,1);
	OLED_ShowChinese(80,0,11,16,1);
	OLED_ShowChinese(96,0,12,16,1);
	//年月日 窗帘状态
	OLED_ShowNum(24,16,TimeData.year,4,16,1);
	OLED_ShowChar(56,16,'_',16,1);
	OLED_ShowNum(64,16,TimeData.month,2,16,1);
	OLED_ShowChar(80,16,'_',16,1);
	OLED_ShowNum(88,16,TimeData.day,2,16,1);
	if(CL_Flag == 0)
		OLED_ShowChinese(0,16,20,16,1);
	else
		OLED_ShowChinese(0,16,19,16,1);
	//温度
	OLED_ShowNum(0,32,rec_data[0],2,16,1);
	OLED_ShowChar(16,32,'.',16,1);
	OLED_ShowNum(24,32,rec_data[1],2,16,1);
	//时钟
	OLED_ShowNum(45,32,TimeData.minute,2,16,1);
	OLED_ShowChar(61,32,':',16,1);
	OLED_ShowNum(67,32,TimeData.second,2,16,1);
	//湿度
	OLED_ShowNum(89,32,rec_data[2],2,16,1);
	OLED_ShowChar(105,32,'.',16,1);
	OLED_ShowNum(113,32,rec_data[3],2,16,1);
	//状态栏
	OLED_ShowChinese(0,48,2,16,1);
	OLED_ShowChinese(16,48,3,16,1);
	OLED_ShowChar(32,48,':',16,1);
	if(mode == 0)
	{
		OLED_ShowChinese(40,48,4,16,1);
		OLED_ShowChinese(56,48,5,16,1);
	}
	else
	{
		OLED_ShowChinese(40,48,6,16,1);
		OLED_ShowChinese(56,48,5,16,1);
	}
	if(JG_Flag == 0)
	{
		OLED_ShowChinese(88,48,15,16,1);
		OLED_ShowChinese(104,48,16,16,1);
	}
	else
	{
		OLED_ShowChinese(88,48,17,16,1);
		OLED_ShowChinese(104,48,18,16,1);
	}
}


void DATA_Pro(void)
{
	ds1302_read_realTime();
	adc_read();
	DHT11_REC_Data();
	BT04A_DATA_Pro();
}

void ALARM_Pro(void)
{
	if(mode == 0)
	{
		if(JG_Flag != 1)
		{
			if(ADC_VAL[1] > 1.0f && GM_Flag == 0)
			{
				CL_Flag = 1;
				BT04A_SendString("cl_k");
				YJ45_Start();
				GM_Flag = 1;
			}
			else if(ADC_VAL[1] < 1.0f && GM_Flag == 1)
			{
				CL_Flag = 0;
				BT04A_SendString("cl_g");
				YJ45_Start();
				GM_Flag = 0;
			}
		}
		if(ADC_VAL[0] < 1.0f && JG_Flag == 0)
		{
			JG_Flag = 1;
			BT04A_SendString("jg");
			
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET);
			HAL_Delay(300);
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
		}
		else if(ADC_VAL[0] > 1.0f && JG_Flag == 1)
		{
			CL_Flag = 0;
			JG_Flag = 0;
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
