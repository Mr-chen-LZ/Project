#include "SG90.h"
#include "tim.h"

/**
  * @brief  设置舵机角度
  * @param  htim: 定时器句柄(TIM3)
  * @param  angle: 目标角度(0-180度)
  * @retval None
  */
void Servo_SetAngle(uint8_t angle)
{
    // 角度限幅
    angle = (angle > 180) ? 180 : angle;
    
    // 转换为脉冲宽度(500-2500us)
    uint16_t pulse = 500 + angle * 2000 / 180;
    
    // 设置比较值
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pulse);

}

/**
  * @brief  设置舵机脉冲宽度(微秒)
  * @param  htim: 定时器句柄(TIM3)
  * @param  pulse_us: 脉冲宽度(500-2500us)
  * @retval None
  */
void Servo_SetPulse(TIM_HandleTypeDef *htim, uint16_t pulse_us)
{
    // 脉冲限幅
    pulse_us = (pulse_us < 500) ? 500 : pulse_us;
    pulse_us = (pulse_us > 2500) ? 2500 : pulse_us;
    
    if(htim->Instance == TIM2)
    {
        __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, pulse_us);
    }
}


