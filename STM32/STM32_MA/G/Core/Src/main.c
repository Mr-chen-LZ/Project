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
#include "dma.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <oled.h>
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
// DMA ADC ����
#define ADC_BUFFER_SIZE 100  // DMA��������С���㹻���Խ�����Ч�˲�
uint32_t adc_dma_buffer[ADC_BUFFER_SIZE]; // DMAĿ�껺����

// �˲�����
#define TRIM_PERCENTAGE 20   // �޼��ٷֱȣ�ÿ���޼�10%���ܹ��޼�20%��

//��ʾ������
uint8_t oled[100] = {0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint32_t filter_adc_readings(uint32_t* buffer, uint32_t size);
int compare_uint32(const void *a, const void *b);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	OLED_Init();
	OLED_Clear();
	// ����DMA���䣬��ADC�����Զ���䵽adc_dma_buffer
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_dma_buffer, ADC_BUFFER_SIZE);

	// ������ʱ��6�����������Եش���ADCת��
	HAL_TIM_Base_Start(&htim3);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		// ���ø߼��˲���������DMA�������е�����
		uint32_t filtered_adc_value = filter_adc_readings(adc_dma_buffer, ADC_BUFFER_SIZE);

		// ��ADCֵת��Ϊ��ѹ (����Vref+ = 3.3V)
		float filtered_voltage = (filtered_adc_value * 3.3f) / 4095.0f;

		// ������ĵ�·������� (����: V = I * R -> I = V / R)
		const float R_SHUNT = 10.0f;
		float current_mA = (filtered_voltage / R_SHUNT) * 1000.0f;

		// �жϵ����Ƿ񳬹���ֵ
		if (current_mA >= 10.0f) {
				// ִ�ж������������LED(PC13)
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
		} else {
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
		}
		
		sprintf((char *)oled,"%.2fV     ",filtered_voltage);
		OLED_ShowString(0,0,oled,16,1);
		
		sprintf((char *)oled,"%.3fmA    ",current_mA);
		OLED_ShowString(0,16,oled,16,1);
		
		// ÿ�δ�����ȴ�һ��ʱ�䣬����Ҫ̫��
		HAL_Delay(500); // ÿ500ms���һ�ν��
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
// qsort����ıȽϺ���
int compare_uint32(const void *a, const void *b) {
  uint32_t arg1 = *((uint32_t*)a);
  uint32_t arg2 = *((uint32_t*)b);
  if (arg1 < arg2) return -1;
  if (arg1 > arg2) return 1;
  return 0;
}
// �߼�����˲���������ֵ�˲���ȥ��ֵ�� + ��ֵ�˲�
uint32_t filter_adc_readings(uint32_t* buffer, uint32_t size) 
{
	// 1. ������ʱ���������������ݣ������ƻ�DMAԭʼ����
	uint32_t temp_buffer[ADC_BUFFER_SIZE];
	memcpy(temp_buffer, buffer, size * sizeof(uint32_t));

	// 2. ����ʱ����������������ֵ�˲��ĵ�һ����
	qsort(temp_buffer, size, sizeof(uint32_t), compare_uint32);

	// 3. ������Ҫ�޼������������������磬�޼�����ߺ���͵�10%��
	uint32_t samples_to_trim = (size * TRIM_PERCENTAGE) / 100;
	uint32_t start_index = samples_to_trim / 2;      // ��ʼ���������޼����Ͷˣ�
	uint32_t end_index = size - (samples_to_trim / 2); // �������������޼����߶ˣ�
	uint32_t valid_sample_count = end_index - start_index;

	// 4. ���޼���ʣ��ġ���Ч��������ƽ��ֵ
	uint64_t sum = 0;
	for (uint32_t i = start_index; i < end_index; i++) {
			sum += temp_buffer[i];
	}

	// 5. ����ƽ��ֵ
	return (uint32_t)(sum / valid_sample_count);
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
