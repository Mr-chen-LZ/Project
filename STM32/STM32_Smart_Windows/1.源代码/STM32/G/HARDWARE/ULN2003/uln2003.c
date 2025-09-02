#include "uln2003.h"
#include "time.h"


// 启动函数
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
        // 检查是否达到延时时间
        if(HAL_GetTick() - lastTick >= delayMs)
        {
            // 全部引脚置低
            for(uint32_t j = 0; j < pinCount; j++)
            {
                HAL_GPIO_WritePin(GPIOA, pins[j], GPIO_PIN_RESET);
            }
            
            // 当前引脚置高
            HAL_GPIO_WritePin(GPIOA, pins[currentPin], GPIO_PIN_SET);
            
            // 更新状态
            currentPin = (currentPin + 1) % pinCount;
            if(currentPin == 0) loopCount--;
            
            // 记录时间
            lastTick = HAL_GetTick();
        }
    }
		for(uint32_t j = 0; j < pinCount; j++)
		{
				HAL_GPIO_WritePin(GPIOA, pins[j], GPIO_PIN_RESET);
		}
}