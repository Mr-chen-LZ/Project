#ifndef __ds1302_H_
#define __ds1302_H_ 
 
#include "main.h" 
 
#define RST_L HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET)
#define RST_H HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET)
#define SCLK_L HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET)
#define SCLK_H  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET)
#define DATA_L  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET)
#define DATA_H  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET)

//寄存器写入地址/指令定义
#define DS1302_SECOND		0x80
#define DS1302_MINUTE		0x82
#define DS1302_HOUR			0x84
#define DS1302_DATE			0x86
#define DS1302_MONTH		0x88
#define DS1302_DAY			0x8A
#define DS1302_YEAR			0x8C
#define DS1302_WP			0x8E
 
struct TIMEData
{
	uint16_t year;
	uint8_t  month;
	uint8_t  day;
	uint8_t  hour;
	uint8_t  minute;
	uint8_t  second;
	uint8_t  week;
};
extern struct TIMEData TimeData;
void ds1302_write_onebyte(uint8_t data);
void ds1302_wirte_rig(uint8_t address,uint8_t data);
uint8_t ds1302_read_rig(uint8_t address);
void ds1302_init(void);
void ds1302_DATAOUT_init(void);
void ds1302_DATAINPUT_init(void);
void ds1302_read_time(void);
void ds1302_read_realTime(void);
void ds1302_SetTime(void);
uint8_t DWT_Init(void);
 
#endif


