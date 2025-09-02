#include "SG90.h"
#include "tim.h"

/**
  * @brief  ���ö���Ƕ�
  * @param  htim: ��ʱ�����(TIM3)
  * @param  angle: Ŀ��Ƕ�(0-180��)
  * @retval None
  */
void Servo_SetAngle(TIM_HandleTypeDef *htim, uint8_t angle)
{
    // �Ƕ��޷�
    angle = (angle > 180) ? 180 : angle;
    
    // ת��Ϊ������(500-2500us)
    uint16_t pulse = 500 + angle * 2000 / 180;
     if(htim->Instance == TIM3)
			__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, pulse);
}


