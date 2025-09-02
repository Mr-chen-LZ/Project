#include "ds1302.h"
 
struct TIMEData TimeData;
static uint8_t dwt_initialized = 0;
#define DWT_CR      *(volatile uint32_t *)0xE0001000
#define DWT_CYCCNT  *(volatile uint32_t *)0xE0001004
#define DEM_CR      *(volatile uint32_t *)0xE000EDFC
#define DEM_CR_TRCENA   (1 << 24)
#define DWT_CR_CYCCNTENA (1 << 0)
char DS1302_Time[]={25,7,23,14,52,55,3};
uint8_t read_time[7];

static void delay_us(uint16_t time)
{
	if(!dwt_initialized && !DWT_Init()) 
	{
			// 备用方案（不精确）
			for(volatile uint32_t i = 0; i < time * 8; i++);
			return;
	}
	
	uint32_t start = DWT_CYCCNT;
	uint32_t ticks = time * (SystemCoreClock / 1000000);
	
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

void ds1302_DATAOUT_init()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_4;               
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;      
	GPIO_InitStruct.Pull = GPIO_NOPULL;             
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;    
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);        

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET); 
}
 
void ds1302_DATAINPUT_init()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_4;           
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;    
	GPIO_InitStruct.Pull = GPIO_NOPULL;         
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);      
}
 
 
void ds1302_write_onebyte(uint8_t data)
{
	ds1302_DATAOUT_init();
	uint8_t count=0;
	SCLK_L;
	for(count=0;count<8;count++)
		{	
			SCLK_L;
			if(data&0x01)
				{DATA_H;}
			else{DATA_L;}
				SCLK_H;
			data>>=1;
		}
}
 
void ds1302_wirte_rig(uint8_t address,uint8_t data)
{
	uint8_t temp1=address;
	uint8_t temp2=data;
	RST_L;
	SCLK_L;
	delay_us(1);
	RST_H;
	delay_us(2);
	ds1302_write_onebyte(temp1);
	ds1302_write_onebyte(temp2);
	RST_L;
	SCLK_L;
	delay_us(2);
}
 
uint8_t ds1302_read_rig(uint8_t address)
{
	uint8_t temp3=address;
	uint8_t count=0;
	uint8_t return_data=0x00;
	RST_L;
	SCLK_L;
	delay_us(3);
	RST_H;
	delay_us(3);
	ds1302_write_onebyte(temp3);
	ds1302_DATAINPUT_init();
	delay_us(2);
	for(count=0;count<8;count++)
	{
		delay_us(2);
		return_data>>=1;
		SCLK_H;
		delay_us(4);
		SCLK_L;
		delay_us(14);
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4))
		{
			return_data=return_data|0x80;
		}
	}
	delay_us(2);
	RST_L;
	DATA_L;
	return return_data;
}
 
void ds1302_init(void)
{
	ds1302_wirte_rig(0x8e,0x00);
	ds1302_wirte_rig(0x80,0x37);
	ds1302_wirte_rig(0x82,0x58);
	ds1302_wirte_rig(0x84,0x23);
	ds1302_wirte_rig(0x86,0x30);
	ds1302_wirte_rig(0x88,0x09);
	ds1302_wirte_rig(0x8a,0x07);
	ds1302_wirte_rig(0x8c,0x20);
	ds1302_wirte_rig(0x8e,0x80);
	ds1302_SetTime();
}
 
void ds1302_read_time(void)
{
	read_time[0]=ds1302_read_rig(0x81);
	read_time[1]=ds1302_read_rig(0x83);
	read_time[2]=ds1302_read_rig(0x85);
	read_time[3]=ds1302_read_rig(0x87);
	read_time[4]=ds1302_read_rig(0x89);
	read_time[5]=ds1302_read_rig(0x8B);
	read_time[6]=ds1302_read_rig(0x8D);
}

void ds1302_SetTime(void)
{
	ds1302_wirte_rig(DS1302_WP,0x00);
	ds1302_wirte_rig(DS1302_YEAR,DS1302_Time[0]/10*16+DS1302_Time[0]%10);//十进制转BCD码后写入
	ds1302_wirte_rig(DS1302_MONTH,DS1302_Time[1]/10*16+DS1302_Time[1]%10);
	ds1302_wirte_rig(DS1302_DATE,DS1302_Time[2]/10*16+DS1302_Time[2]%10);
	ds1302_wirte_rig(DS1302_HOUR,DS1302_Time[3]/10*16+DS1302_Time[3]%10);
	ds1302_wirte_rig(DS1302_MINUTE,DS1302_Time[4]/10*16+DS1302_Time[4]%10);
	ds1302_wirte_rig(DS1302_SECOND,DS1302_Time[5]/10*16+DS1302_Time[5]%10);
	ds1302_wirte_rig(DS1302_DAY,DS1302_Time[6]/10*16+DS1302_Time[6]%10);
	ds1302_wirte_rig(DS1302_WP,0x80);
}

void ds1302_read_realTime(void)
{
	ds1302_read_time(); 
	TimeData.second=(read_time[0]>>4)*10+(read_time[0]&0x0f);
	TimeData.minute=((read_time[1]>>4)&(0x07))*10+(read_time[1]&0x0f);
	TimeData.hour=(read_time[2]>>4)*10+(read_time[2]&0x0f);
	TimeData.day=(read_time[3]>>4)*10+(read_time[3]&0x0f);
	TimeData.month=(read_time[4]>>4)*10+(read_time[4]&0x0f);
	TimeData.week=read_time[5];
	TimeData.year=(read_time[6]>>4)*10+(read_time[6]&0x0f)+2000;
}

