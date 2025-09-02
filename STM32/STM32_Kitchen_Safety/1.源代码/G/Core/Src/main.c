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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "dht11.h"
#include "esp-01s.h"
#include "oled.h"
#include "sg90.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
int temp_last = 0;
uint8_t oled[100] = {0};
uint8_t data[100] = {0};
uint8_t page = 0;
uint8_t key = 0;
uint8_t key1 = 0;
uint8_t key2 = 0;
extern unsigned int rec_data[4];
extern uint8_t yanwu;
//烟雾报警参数
uint8_t yanwu_max = 50;
//温度报警参数
uint8_t temp_max = 30;
//警报标志位
uint8_t f_flag = 0;//火灾
uint8_t w_flag = 0;//温度
uint8_t y_flag = 0;//烟雾
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void OLED_Poce(void);
void DATA_Poce(void);
void KEY_Poce(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void OLED_Poce(void)
{
	switch(page)
	{
		case 0://主界面
		{
			//标题
			OLED_ShowChinese(32,0,0,16,1);
			OLED_ShowChinese(48,0,1,16,1);
			OLED_ShowChinese(64,0,2,16,1);
			OLED_ShowChinese(80,0,3,16,1);
			//温度
			OLED_ShowChinese(16,16,6,16,1);
			OLED_ShowChinese(32,16,7,16,1);
			OLED_ShowChar(48,16,':',16,1);
			OLED_ShowNum(56,16,rec_data[2],2,16,1);
			OLED_ShowChar(72,16,'.',16,1);
			OLED_ShowNum(80,16,rec_data[3],2,16,1);
			//湿度
			OLED_ShowChinese(16,32,24,16,1);
			OLED_ShowChinese(32,32,7,16,1);
			OLED_ShowChar(48,32,':',16,1);
			OLED_ShowNum(56,32,rec_data[0],2,16,1);
			OLED_ShowChar(72,32,'.',16,1);
			OLED_ShowNum(80,32,rec_data[1],2,16,1);
			//烟雾
			OLED_ShowChinese(16,48,8,16,1);
			OLED_ShowChinese(32,48,9,16,1);
			OLED_ShowChar(48,48,':',16,1);
			sprintf((char *)oled,"%d %%",yanwu);
			OLED_ShowString(56,48,oled,16,1);

			break;
		}
		case 1://参数
		{
			//温度
			OLED_ShowChinese(16,16,6,16,1);
			OLED_ShowChinese(32,16,7,16,1);
			OLED_ShowChar(48,16,':',16,1);
			OLED_ShowNum(56,16,temp_max,2,16,1);
			break;
		}
		case 2://参数
		{
			//烟雾
			OLED_ShowChinese(16,16,8,16,1);
			OLED_ShowChinese(32,16,9,16,1);
			OLED_ShowChar(48,16,':',16,1);
			OLED_ShowNum(56,16,yanwu_max,2,16,1);
			break;
		}
		case 3://火灾警告
		{
			OLED_ShowPicture(48,24,32,16,1);
		}
	}
}

void DATA_Poce(void)
{
	Esp_Data_Pro();
	if(page == 0 || page == 3)
	{
		DHT11_REC_Data();
		adc_read();
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3) == GPIO_PIN_RESET && f_flag == 0)//着火
		{
			f_flag = 1;
			OLED_Clear();
			page = 3;
			HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);//蜂鸣器启动
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);//抽水继电器启动
			sprintf((char *)data,"ZHAO_HUO\r\n");
			ESP8266_SendData(data,strlen((const char *)data));
		}
		else if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3) == GPIO_PIN_SET && f_flag == 1)
		{
			OLED_Clear();
			HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_3);
			f_flag = 0;
			page = 0;
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
		}
		
		if(rec_data[2] > temp_max && w_flag == 0 && rec_data[2] - temp_last < 10)//高温启动舵机开窗
		{
			w_flag = 1;
			Servo_SetAngle(90);
			sprintf((char *)data,"GAO_WEM\r\n");
			ESP8266_SendData(data,strlen((const char *)data));
		}
		else if(rec_data[2] < temp_max && w_flag == 1)
		{
			w_flag = 0;
			Servo_SetAngle(0);
		}
		if(yanwu > yanwu_max && y_flag == 0)//烟雾启动电机抽风
		{
			y_flag = 1;
			HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
			sprintf((char *)data,"YAN_WU\r\n");
			ESP8266_SendData(data,strlen((const char *)data));
		}
		else if(yanwu < yanwu_max && y_flag == 1)
		{
			y_flag = 0;
			HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_1);
		}
		temp_last = rec_data[2];
	}
}

void KEY_Poce(void)
{
		key1 = key_val(); // 读取当前按键状态
    // 边沿检测：检测按键从按下到抬起的下降沿
    if ((key == 1 || key == 2) && (key1 == 0)) {
        // 上次是按下的，现在抬起了，说明完成了一次完整的按键动作
        key2 = key; // 保存抬起时对应的按键值
    } else {
        // 其他情况，无有效按键动作
        key2 = 0;
    }
    
    // 更新上一次的按键状态
    key = key1;
    
	switch(key2)
	{
		case 1:
		{
			page++;
			OLED_Clear();
			if(page > 2)
				page = 0;
			break;
		}
		case 2:
		{
			switch(page)
			{
				case 1:
				{
					temp_max+=10;
					if(temp_max > 100)
						temp_max = 0;
					break;
				}
				case 2:
				{
					yanwu_max+=10;
					if(yanwu_max > 100)
						yanwu_max = 0;
					break;
				}
			}
			break;
		}
	}
}
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
  MX_USART3_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
	DWT_Init();
	OLED_Init();
	OLED_Clear();
	ESP8266_Init();
	OLED_Clear();
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
	Servo_SetAngle(0);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
		KEY_Poce();
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
