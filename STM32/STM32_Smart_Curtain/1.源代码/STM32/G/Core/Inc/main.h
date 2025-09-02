/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LH_IO2_Pin GPIO_PIN_13
#define LH_IO2_GPIO_Port GPIOC
#define LH_IO3_Pin GPIO_PIN_14
#define LH_IO3_GPIO_Port GPIOC
#define LH_IO4_Pin GPIO_PIN_15
#define LH_IO4_GPIO_Port GPIOC
#define LH_IO1_Pin GPIO_PIN_0
#define LH_IO1_GPIO_Port GPIOA
#define DHT11_DATA_Pin GPIO_PIN_4
#define DHT11_DATA_GPIO_Port GPIOA
#define OLED_SCL_Pin GPIO_PIN_12
#define OLED_SCL_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_15
#define OLED_SDA_GPIO_Port GPIOB
#define ULN_IN1_Pin GPIO_PIN_8
#define ULN_IN1_GPIO_Port GPIOA
#define ULN_IN2_Pin GPIO_PIN_9
#define ULN_IN2_GPIO_Port GPIOA
#define ULN_IN3_Pin GPIO_PIN_10
#define ULN_IN3_GPIO_Port GPIOA
#define ULN_IN4_Pin GPIO_PIN_11
#define ULN_IN4_GPIO_Port GPIOA
#define DS_CLK_Pin GPIO_PIN_3
#define DS_CLK_GPIO_Port GPIOB
#define DS_DATA_Pin GPIO_PIN_4
#define DS_DATA_GPIO_Port GPIOB
#define DS_RST_Pin GPIO_PIN_5
#define DS_RST_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
