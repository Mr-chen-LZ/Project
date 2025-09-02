/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32f1xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

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
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart1;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
	extern uint8_t seat_Sate[4];
	extern uint8_t seat_Flag[4];
	extern uint8_t seat_Weiyue[4];

	extern uint16_t time_Seat1[2];
	extern uint16_t time_Seat2[2];
	extern uint16_t time_Seat3[2];
	extern uint16_t time_Seat4[2];
  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */
	if(seat_Flag[0] != 0)//座位1定时，预约未签到5s后取消，预约签到后10s取消
	{
		time_Seat1[0]++;
		if(time_Seat1[0] > 1000)
		{
			time_Seat1[0] = 0;
			time_Seat1[1]++;
			if(seat_Sate[0] == 1)
			{
				if(time_Seat1[1] > 5)
				{
					time_Seat1[1] = 0;
					seat_Flag[0] = 0;
					seat_Weiyue[0] = 1;
				}
			}
			if(seat_Sate[0] == 2)
			{
				if(time_Seat1[1] > 10)
				{
					time_Seat1[1] = 0;
					seat_Flag[0] = 0;
					seat_Sate[0] = 0;
					seat_Weiyue[0] = 0;
				}
			}
		}
	}
	
	if(seat_Flag[1] != 0)//座位2定时，预约未签到5s后取消，预约签到后10s取消
	{
		time_Seat2[0]++;
		if(time_Seat2[0] > 1000)
		{
			time_Seat2[0] = 0;
			time_Seat2[1]++;
			if(seat_Sate[1] == 1)
			{
				if(time_Seat2[1] > 5)
				{
					time_Seat2[1] = 0;
					seat_Flag[1] = 0;
					seat_Weiyue[1] = 1;
				}
			}
			if(seat_Sate[1] == 2)
			{
				if(time_Seat2[1] > 10)
				{
					time_Seat2[1] = 0;
					seat_Flag[1] = 0;
					seat_Sate[1] = 0;
					seat_Weiyue[1] = 0;
				}
			}
		}
	}
	
	if(seat_Flag[2] == 1)//座位3定时，预约未签到5s后取消，预约签到后10s取消
	{
		time_Seat3[0]++;
		if(time_Seat3[0] > 1000)
		{
			time_Seat3[0] = 0;
			time_Seat3[1]++;
			if(seat_Sate[2] == 1)
			{
				if(time_Seat3[1] > 5)
				{
					time_Seat3[1] = 0;
					seat_Flag[2] = 0;
					seat_Weiyue[2] = 1;
				}
			}
			if(seat_Sate[2] == 2)
			{
				if(time_Seat3[1] > 10)
				{
					time_Seat3[1] = 0;
					seat_Flag[2] = 0;
					seat_Sate[2] = 0;
					seat_Weiyue[2] = 0;
				}
			}
		}
	}
	
	if(seat_Flag[3] == 1)//座位4定时，预约未签到5s后取消，预约签到后10s取消
	{
		time_Seat4[0]++;
		if(time_Seat4[0] > 1000)
		{
			time_Seat4[0] = 0;
			time_Seat4[1]++;
			if(seat_Sate[3] == 1)
			{
				if(time_Seat4[1] > 5)
				{
					time_Seat4[1] = 0;
					seat_Flag[3] = 0;
					seat_Weiyue[3] = 1;
				}
			}
			if(seat_Sate[3] == 2)
			{
				if(time_Seat4[1] > 10)
				{
					time_Seat4[1] = 0;
					seat_Flag[3] = 0;
					seat_Sate[3] = 0;
					seat_Weiyue[3] = 0;
				}
			}
		}
	}
  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
	
	
	
  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
