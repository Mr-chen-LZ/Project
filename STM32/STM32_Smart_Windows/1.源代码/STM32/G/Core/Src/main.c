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
#include "dht11.h"
#include "bt04a.h"
#include "uln2003.h"
#include "stdio.h"
#include "string.h"
#include "SG90.h"

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
void KEY_Poce(void);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern float ADC_VAL[3];//
uint8_t oled[100] = {0};
int TIME[3] = {16,12,24};
float Threshold[3] = {2.0f,0.5f,1.5f};
uint8_t ch_flag = 0;//窗户标志位
uint8_t cl_flag = 0;//窗帘标志位
uint8_t bj_flag = 0;
uint8_t dj_flag = 0;
int cl_time = 0;//定时打开窗帘时间
int cl_max = 10 * 1000;
extern unsigned int rec_data[4];
uint8_t ren_flag = 0;//行人靠近标志位
uint8_t koqi_flag = 0;//空气质量标志位
uint8_t yu_flag = 0;//有无下雨标志位
uint8_t mode = 0;//运行模式标志位
uint8_t page = 0;
uint8_t key = 0;
uint8_t key1;
uint8_t select_flag = 0;
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
  MX_TIM3_Init();
  MX_TIM2_Init();
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
  /* USER CODE BEGIN 2 */
	DWT_Init();
	BT04A_Init();
	OLED_Init();
	OLED_Clear();
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
void OLED_Poce(void)
{
	switch(page)
	{
		case 0:
		{
		//标题 + 模式
		OLED_ShowChinese(16,0,7,16,1);
		OLED_ShowChinese(32,0,8,16,1);
		OLED_ShowChinese(48,0,9,16,1);
		OLED_ShowChinese(64,0,10,16,1);
		
		if(mode == 0)
		{
			OLED_ShowChinese(96,0,4,16,1);
			OLED_ShowChinese(112,0,5,16,1);
		}
		else
		{
			OLED_ShowChinese(96,0,6,16,1);
			OLED_ShowChinese(112,0,5,16,1);
		}
		//湿度、时间、温度
		OLED_ShowNum(0,16,rec_data[0],2,16,1);
		OLED_ShowNum(32,16,TIME[0],2,16,1);
		OLED_ShowChar(48,16,':',16,1);
		OLED_ShowNum(54,16,TIME[1],2,16,1);
		OLED_ShowChar(70,16,':',16,1);
		OLED_ShowNum(78,16,TIME[2],2,16,1);
		OLED_ShowNum(110,16,rec_data[2],2,16,1);
		//窗户状态
		if(ch_flag == 0)
		{
			OLED_ShowChinese(0,32,9,16,1);
			OLED_ShowChinese(16,32,10,16,1);
			OLED_ShowChar(32,32,':',16,1);
			OLED_ShowChinese(40,32,21,16,1);
		}
		else if(ch_flag == 1)
		{
			OLED_ShowChinese(0,32,9,16,1);
			OLED_ShowChinese(16,32,10,16,1);
			OLED_ShowChar(32,32,':',16,1);
			OLED_ShowChinese(40,32,20,16,1);
		}//窗帘状态
		if(cl_flag == 0)
		{
			OLED_ShowChinese(72,32,9,16,1);
			OLED_ShowChinese(88,32,19,16,1);
			OLED_ShowChar(104,32,':',16,1);
			OLED_ShowChinese(112,32,20,16,1);
		}
		else if(cl_flag == 1)
		{
			OLED_ShowChinese(72,32,9,16,1);
			OLED_ShowChinese(88,32,19,16,1);
			OLED_ShowChar(104,32,':',16,1);
			OLED_ShowChinese(112,32,21,16,1);
		}
		//有无雨
		if(yu_flag == 0)
		{
			OLED_ShowChinese(0,48,23,16,1);
			OLED_ShowChinese(16,48,29,16,1);
		}
		else
		{
			OLED_ShowChinese(0,48,22,16,1);
			OLED_ShowChinese(16,48,29,16,1);
		}
		//有无行人靠近
		if(ren_flag == 0)
		{
			OLED_ShowChinese(48,48,23,16,1);
			OLED_ShowChinese(64,48,24,16,1);
		}
		else
		{
			OLED_ShowChinese(48,48,22,16,1);
			OLED_ShowChinese(64,48,24,16,1);
		}
		//空气状态
		if(koqi_flag == 0)
		{
			OLED_ShowChinese(96,48,15,16,1);
			OLED_ShowChinese(112,48,16,16,1);
		}
		else
		{
			OLED_ShowChinese(96,48,17,16,1);
			OLED_ShowChinese(112,48,18,16,1);
		}
			break;
		}
		case 1:
		{
			//参数标题
			OLED_ShowChinese(0,0,30,16,1);
			OLED_ShowChinese(16,0,31,16,1);
			sprintf((char *)oled,"            ");
			OLED_ShowString(32,0,oled,16,1);
			//雨量参数
			OLED_ShowChinese(0,16,29,16,1);
			OLED_ShowChinese(16,16,32,16,1);
			OLED_ShowChar(32,16,':',16,1);
			sprintf((char *)oled,"%.2f",Threshold[0]);
			OLED_ShowString(40,16,oled,16,1);
			sprintf((char *)oled,"         ");
			OLED_ShowString(72,16,oled,16,1);
			//空气质量参数
			OLED_ShowChinese(0,32,33,16,1);
			OLED_ShowChinese(16,32,34,16,1);
			OLED_ShowChar(32,32,':',16,1);
			sprintf((char *)oled,"%.2f",Threshold[1]);
			OLED_ShowString(40,32,oled,16,1);
			sprintf((char *)oled,"         ");
			OLED_ShowString(72,32,oled,16,1);
			//行人距离参数
			OLED_ShowChinese(0,48,35,16,1);
			OLED_ShowChinese(16,48,36,16,1);
			OLED_ShowChar(32,48,':',16,1);
			sprintf((char *)oled,"%.2f",Threshold[2]);
			OLED_ShowString(40,48,oled,16,1);
			sprintf((char *)oled,"         ");
			OLED_ShowString(72,48,oled,16,1);
			break;
		}
	}
}

void DATA_Poce(void)
{
	data_read();
	BT04A_DATA_Pro();
	DHT11_REC_Data();
	//雨量,有雨关窗并发送数据给上位机，没雨开窗并发送数据给上位机，更新窗户标志位
	if(mode == 0)
	{
		if(ADC_VAL[0] < Threshold[0])
		{
			ch_flag = 1;
			yu_flag = 1;
			if(dj_flag == 0)
			{
				dj_flag = 1;
				Servo_SetAngle(&htim3,0);
				BT04A_SendString("youyu_guanchuan");
			}
		}
		else
		{
			ch_flag = 0;
			yu_flag = 0;
			if(dj_flag == 1)
			{
				dj_flag = 0;
				Servo_SetAngle(&htim3,90);
				BT04A_SendString("meiyu_kaichuan");
			}
		}
	}
	//定时开窗帘晒太阳，自动模式按照定时器启动，手动模式下发命令启动
	if(mode == 1)
		cl_time = 0;
	if(cl_flag == 0)
	{
		if(bj_flag == 0)
		{
			bj_flag = 1;
			YJ45_Start();
		}
	}
	else
	{
		if(bj_flag == 1)
		{
			bj_flag = 0;
			YJ45_Start();
		}
	}
	//检测空气中污染空气含量，超过一个值更新显示并通过蓝牙报警
	if(ADC_VAL[1] > Threshold[1])
	{
		koqi_flag = 1;
		BT04A_SendString("koqi_jinbao");
	}
	else
		koqi_flag = 0;
	//检测行人靠近距离，靠太近了触发报警，亮LED灯并通过蓝牙发送警报
	if(ADC_VAL[2] < Threshold[2])
	{
		ren_flag = 1;
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_SET);
		HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);
		BT04A_SendString("youren_kaojin");
	}
	else
	{
		ren_flag = 0;
		HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_3);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);
	}
}

void KEY_Poce(void)
{
	key1 = key_read();
	if(key != key1)
		key = key1;
	else
		key1 = 0;
	switch(key1)
	{
		case 1://页面切换
		{
			page++;
			if(page > 1)
				page = 0;
			OLED_Clear();
			if(page == 0)
				select_flag = 0;
			break;
		}
		case 2://主页面下按键控制模式切换，参数页面下控制选择位
		{
			if(page == 0)
			{
				mode++;
				if(mode > 1)
					mode = 0;
			}
			else
			{
				select_flag++;
				if(select_flag > 2)
					select_flag = 0;
			}
			break;
		}
		case 3://主页面下，手动模式打开窗帘,参数页面下+0.3f
		{
			if(page == 0)
			{
				if(mode == 1)
				{
					cl_flag++;
					if(cl_flag > 1)
						cl_flag = 0;
				}
			}
			else
			{
				switch(select_flag)
				{
					case 0://雨量参数
					{
						Threshold[0] += 0.3f;
						if(Threshold[0] > 3.0f)
							Threshold[0] = 0.0f;
						break;
					}
					case 1://距离参数
					{
						Threshold[1] += 0.3f;
						if(Threshold[1] > 3.0f)
							Threshold[1] = 0.0f;
						break;
					}
					case 2://空气质量参数
					{
						Threshold[2] += 0.3f;
						if(Threshold[2] > 3.0f)
							Threshold[2] = 0.0f;
						break;
					}
				}
			}
			break;
		}
		case 4://主页面下，手动模式打开窗户,参数页面下-0.3f
		{
			if(page == 0)
			{
				if(mode == 1)
				{
					ch_flag++;
					if(ch_flag > 1)
						ch_flag = 0;
					if(ch_flag == 1)
						Servo_SetAngle(&htim3,0);
					else
						Servo_SetAngle(&htim3,90);
				}
			}
			else
			{
				switch(select_flag)
				{
					case 0:
					{
						break;
					}
					case 1:
					{
						break;
					}
					case 2:
					{
						break;
					}
				}
			}
			break;
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
