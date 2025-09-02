#include "uln2003.h"
#include "time.h"


// ��������
void YJ45_Start(void)
{
    const uint16_t pins[] = {GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11};
    const uint32_t pinCount = sizeof(pins) / sizeof(pins[0]);
    
    uint32_t loopCount = 100;
    uint32_t currentPin = 0;
    uint32_t lastTick = HAL_GetTick();
    const uint32_t delayMs = 5;
    
    while(loopCount > 0)
    {
        // ����Ƿ�ﵽ��ʱʱ��
        if(HAL_GetTick() - lastTick >= delayMs)
        {
            // ȫ�������õ�
            for(uint32_t j = 0; j < pinCount; j++)
            {
                HAL_GPIO_WritePin(GPIOA, pins[j], GPIO_PIN_RESET);
            }
            
            // ��ǰ�����ø�
            HAL_GPIO_WritePin(GPIOA, pins[currentPin], GPIO_PIN_SET);
            
            // ����״̬
            currentPin = (currentPin + 1) % pinCount;
            if(currentPin == 0) loopCount--;
            
            // ��¼ʱ��
            lastTick = HAL_GetTick();
        }
    }
		for(uint32_t j = 0; j < pinCount; j++)
		{
				HAL_GPIO_WritePin(GPIOA, pins[j], GPIO_PIN_RESET);
		}
}