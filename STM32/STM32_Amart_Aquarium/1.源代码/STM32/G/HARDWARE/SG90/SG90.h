#ifndef __SG90_H_
#define __SG90_H_
#include "main.h"              


void Servo_SetAngle(TIM_HandleTypeDef *htim, uint8_t angle);
void Servo_SetPulse(TIM_HandleTypeDef *htim, uint16_t pulse_us);



#endif

