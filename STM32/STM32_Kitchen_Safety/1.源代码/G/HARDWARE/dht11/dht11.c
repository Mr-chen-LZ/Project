#include  "dht11.h"
//数据
static uint8_t dwt_initialized = 0;
unsigned int rec_data[4];

#define DWT_CR      *(volatile uint32_t *)0xE0001000
#define DWT_CYCCNT  *(volatile uint32_t *)0xE0001004
#define DEM_CR      *(volatile uint32_t *)0xE000EDFC
#define DEM_CR_TRCENA   (1 << 24)
#define DWT_CR_CYCCNTENA (1 << 0)

static void DHT11_Delay(uint16_t us) {
    if(!dwt_initialized && !DWT_Init()) 
    {
        // 备用方案（不精确）
        for(volatile uint32_t i = 0; i < us * 8; i++);
        return;
    }
    
    uint32_t start = DWT_CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000);
    
    while((DWT_CYCCNT - start) < ticks);
}

uint8_t DWT_Init(void)
{
    if(dwt_initialized) return 1;
    
    // 启用跟踪调试功能
    DEM_CR |= DEM_CR_TRCENA;
    
    // 启用周期计数器
    DWT_CR |= DWT_CR_CYCCNTENA;
    
    // 重置周期计数器
    DWT_CYCCNT = 0;
    
    // 检查是否初始化成功
    if(DWT_CYCCNT == 0)
    {
        dwt_initialized = 1;
        return 1;
    }
    return 0;
}

//对于stm32来说，是输出
void DH11_GPIO_Init_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin   = GPIO_PIN_11;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;      // 推挽输出
    GPIO_InitStruct.Pull  = GPIO_NOPULL;              // 无上下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;     // 高速（50MHz）

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

//对于stm32来说，是输入
void DH11_GPIO_Init_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin   = GPIO_PIN_11;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;          // 输入模式
    GPIO_InitStruct.Pull  = GPIO_NOPULL;              // 浮空（无上下拉电阻）

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}



//主机发送开始信号
void DHT11_Start(void)
{
	DH11_GPIO_Init_OUT(); //输出模式
	
	dht11_high; //先拉高
	DHT11_Delay(30);
	
	dht11_low; //拉低电平至少18us
	HAL_Delay(20);
	
	dht11_high; //拉高电平20~40us
	DHT11_Delay(30);
	
	DH11_GPIO_Init_IN(); //输入模式
}


//获取一个字节
char DHT11_Rec_Byte(void)
{
	unsigned char i = 0;
	unsigned char data;
	
	for(i=0;i<8;i++) //1个数据就是1个字节byte，1个字节byte有8位bit
	{
		while( Read_Data == 0); //从1bit开始，低电平变高电平，等待低电平结束
		DHT11_Delay(30); //延迟30us是为了区别数据0和数据1，0只有26~28us
		
		data <<= 1; //左移
		
		if( Read_Data == 1 ) //如果过了30us还是高电平的话就是数据1
		{
			data |= 1; //数据+1
		}
		
		while( Read_Data == 1 ); //高电平变低电平，等待高电平结束
	}
	
	return data;
}

//获取数据

void DHT11_REC_Data(void)
{
	unsigned int R_H,R_L,T_H,T_L;
	unsigned char RH,RL,TH,TL,CHECK;
	
	DHT11_Start(); //主机发送信号
	dht11_high; //拉高电平
	
	if( Read_Data == 0 ) //判断DHT11是否响应
	{
		while( Read_Data == 0); //低电平变高电平，等待低电平结束
		while( Read_Data == 1); //高电平变低电平，等待高电平结束
		
		R_H = DHT11_Rec_Byte();
		R_L = DHT11_Rec_Byte();
		T_H = DHT11_Rec_Byte();
		T_L = DHT11_Rec_Byte();
		CHECK = DHT11_Rec_Byte(); //接收5个数据
		
		dht11_low; //当最后一bit数据传送完毕后，DHT11拉低总线 50us
		DHT11_Delay(55); //这里延时55us
		dht11_high; //随后总线由上拉电阻拉高进入空闲状态。
		
		if(R_H + R_L + T_H + T_L == CHECK) //和检验位对比，判断校验接收到的数据是否正确
		{
			RH = R_H;
			RL = R_L;
			TH = T_H;
			TL = T_L;
		}
	}
	rec_data[0] = RH;//湿度
	rec_data[1] = RL;
	rec_data[2] = TH;//温度
	rec_data[3] = TL;
}

