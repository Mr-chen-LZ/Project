#include "SG90.h"
#include "tim.h"

/**
  * @brief  ���ö���Ƕ�
  * @param  htim: ��ʱ�����(TIM3)
  * @param  angle: Ŀ��Ƕ�(0-180��)
  * @retval None
  */
void Servo_SetAngle(uint8_t angle)
{
    // �Ƕ��޷�
    angle = (angle > 180) ? 180 : angle;
    
    // ת��Ϊ������(500-2500us)
    uint16_t pulse = 500 + angle * 2000 / 180;
    
    // ���ñȽ�ֵ
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pulse);

}

/**
  * @brief  ���ö��������(΢��)
  * @param  htim: ��ʱ�����(TIM3)
  * @param  pulse_us: ������(500-2500us)
  * @retval None
  */
void Servo_SetPulse(TIM_HandleTypeDef *htim, uint16_t pulse_us)
{
    // �����޷�
    pulse_us = (pulse_us < 500) ? 500 : pulse_us;
    pulse_us = (pulse_us > 2500) ? 2500 : pulse_us;
    
    if(htim->Instance == TIM2)
    {
        __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, pulse_us);
    }
}


