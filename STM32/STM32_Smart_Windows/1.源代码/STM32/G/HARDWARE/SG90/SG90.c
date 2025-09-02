#include "SG90.h"
#include "tim.h"

/**
  * @brief  设置舵机角度
  * @param  htim: 定时器句柄(TIM3)
  * @param  angle: 目标角度(0-180度)
  * @retval None
  */
void Servo_SetAngle(TIM_HandleTypeDef *htim, uint8_t angle)
{
    // 角度限幅
    angle = (angle > 180) ? 180 : angle;
    
    // 转换为脉冲宽度(500-2500us)
    uint16_t pulse = 500 + angle * 2000 / 180;
     if(htim->Instance == TIM3)
			__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, pulse);
}


