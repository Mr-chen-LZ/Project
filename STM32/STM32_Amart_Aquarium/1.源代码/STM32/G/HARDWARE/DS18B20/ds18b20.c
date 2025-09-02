#include "ds18b20.h"

static uint8_t dwt_initialized = 0;
#define DWT_CR      *(volatile uint32_t *)0xE0001000
#define DWT_CYCCNT  *(volatile uint32_t *)0xE0001004
#define DEM_CR      *(volatile uint32_t *)0xE000EDFC
#define DEM_CR_TRCENA   (1 << 24)
#define DWT_CR_CYCCNTENA (1 << 0)

static void Delay_us(uint16_t us) 
{
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

void DS18B20_GPIO_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Pin   = GPIO_PIN_10;
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;      // 推挽输出
	GPIO_InitStruct.Pull  = GPIO_NOPULL;              // 无上下拉
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;     // 高速（50MHz）

	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void DS18B20_GPIO_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Pin   = GPIO_PIN_10;
	GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;          // 输入模式
	GPIO_InitStruct.Pull  = GPIO_PULLUP;              // 浮空（无上下拉电阻）

	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void DS18B20_RST(void) 																				
{
	DS18B20_GPIO_OUT(); 																				//切换为输出模式
	DS18B20_OUT_RESET;																						//拉低
	Delay_us(700);
	DS18B20_OUT_SET;																						//拉高
	Delay_us(60);
}

int DS18B20_Check(void) 																			//响应1--失败		响应0----成功
{	
	uint16_t retry=0;
	DS18B20_GPIO_IN();																					//切换到输入模式
	while(DS18B20_IN && retry<10000) 														//引脚一直为高，未被设备主动拉低。提供200us的超时时间
	{
		retry++;
		Delay_us(1);
	};
	if(retry>=200) return 1;																		//超时仍为响应（200us）
    else retry=0;
	while(!DS18B20_IN && retry<240)															// 引脚响应则  判断引脚低电平时间是否大于60us、小于240us——返回应答结果
	{
		retry++;
		Delay_us(1);
	}
	if(retry>=240)return 1;  																		//应答过时，检查失败     
  return 0; 																								//检验成功，返回0
	
}

uint8_t DS18B20_Read_Bit(void) 															
{
	uint8_t data;																								//暂存数据
	DS18B20_GPIO_OUT();																					//切换输出模式
	DS18B20_OUT_RESET; 																						//拉低
	Delay_us(2);
	DS18B20_OUT_SET;																						//释放
	DS18B20_GPIO_IN();																					//切换输入模式
	Delay_us(12);
	if(DS18B20_IN)data =1 ;
	else data = 0;
	Delay_us(50);
	//printf("数据：%d\r\n",data);
	return data;

}

void DS18B20_Write_One(void) 
{
  DS18B20_GPIO_OUT();        																//SET PG11 OUTPUT;
	DS18B20_OUT_RESET;																						//拉低2us
	Delay_us(2);                           
	DS18B20_OUT_SET;
	Delay_us(60);
}

void DS18B20_Write_Zero(void)  
{
	DS18B20_GPIO_OUT();        																	//SET PG11 OUTPUT;
	DS18B20_OUT_RESET;																						//拉低2us   
	Delay_us(60);                           
	DS18B20_OUT_SET;
	Delay_us(2);
}

uint8_t DS18B20_Read_Byte(void)   
{
	uint8_t i,j,dat;
	dat=0;
	for (i=1;i<=8;i++) 
	{
			j=DS18B20_Read_Bit();
			dat=(j<<7)|(dat>>1);
	}						    
  return dat;
}

void DS18B20_Write_Byte(uint8_t dat)  
 {            
	uint8_t j;
	uint8_t testb;
	DS18B20_GPIO_OUT();																				//SET PA0 OUTPUT;
	for (j=1;j<=8;j++) 
	{
		testb=dat&0x01;
		dat=dat>>1;
		if (testb) 
		{
			DS18B20_OUT_RESET;																	// Write 1
			Delay_us(2);                            
			DS18B20_OUT_SET;
			Delay_us(60);             
		}
		else 
		{
			DS18B20_OUT_RESET;// Write 0
			Delay_us(60); 
			DS18B20_OUT_SET;
			Delay_us(2);                          
		}
   }
}

void DS18B20_Start(void) 
{                                                                  
   DS18B20_RST();            																//复位
   DS18B20_Check();    		 																	//等待响应
   DS18B20_Write_Byte(0xcc);  															// 发送一条 ROM 指令		0ccH跳过ROM
   DS18B20_Write_Byte(0x44);    														// 发送存储器指令		启动温度转化	
}
  	 
uint8_t DS18B20_Init(void)
{
	DS18B20_RST();

	return DS18B20_Check();
}  
// 读取传感器的值
short	DS18B20_Get_Temp(void)
{
	uint8_t temp;
	uint8_t TH,TL;
	short tem;
	DS18B20_Start();
	DS18B20_RST();
	DS18B20_Check();
	DS18B20_Write_Byte(0xCC); 																// 忽略ROM地址，直接发送命令
	DS18B20_Write_Byte(0xBE); 																// 读取暂存器中9字节数据
	TL = DS18B20_Read_Byte(); 																// 低8位
	TH = DS18B20_Read_Byte(); 																// 低八位
	
	if(TH>7)
	{
			TH=~TH;
			TL=~TL; 
			temp=0;																								//温度为负  
	}else temp=1;																							//温度为正	  	  
	tem=TH; 																									//获得高八位
	tem<<=8;    
	tem+=TL;																									//获得底八位
	tem=(float)tem*0.625;																			//转换     
	if(temp)return tem; 																			//返回温度值
	else return -tem;    
}


