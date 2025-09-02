#include "key.h"

uint8_t key_read(void)
{
	uint8_t val = 0;
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) == GPIO_PIN_RESET)
	{
		HAL_Delay(10);
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) == GPIO_PIN_RESET)
			val = 1;
	}
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1) == GPIO_PIN_RESET)
	{
		HAL_Delay(10);
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1) == GPIO_PIN_RESET)
			val = 2;
	}
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2) == GPIO_PIN_RESET)
	{
		HAL_Delay(10);
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2) == GPIO_PIN_RESET)
			val = 3;
	}
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3) == GPIO_PIN_RESET)
	{
		HAL_Delay(10);
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3) == GPIO_PIN_RESET)
			val = 4;
	}
	return val;
}

