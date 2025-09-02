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
			// ���÷���������ȷ��
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
	
	// ���ø��ٵ��Թ���
	DEM_CR |= DEM_CR_TRCENA;
	
	// �������ڼ�����
	DWT_CR |= DWT_CR_CYCCNTENA;
	
	// �������ڼ�����
	DWT_CYCCNT = 0;
	
	// ����Ƿ��ʼ���ɹ�
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
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;      // �������
	GPIO_InitStruct.Pull  = GPIO_NOPULL;              // ��������
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;     // ���٣�50MHz��

	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void DS18B20_GPIO_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Pin   = GPIO_PIN_10;
	GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;          // ����ģʽ
	GPIO_InitStruct.Pull  = GPIO_PULLUP;              // ���գ������������裩

	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void DS18B20_RST(void) 																				
{
	DS18B20_GPIO_OUT(); 																				//�л�Ϊ���ģʽ
	DS18B20_OUT_RESET;																						//����
	Delay_us(700);
	DS18B20_OUT_SET;																						//����
	Delay_us(60);
}

int DS18B20_Check(void) 																			//��Ӧ1--ʧ��		��Ӧ0----�ɹ�
{	
	uint16_t retry=0;
	DS18B20_GPIO_IN();																					//�л�������ģʽ
	while(DS18B20_IN && retry<10000) 														//����һֱΪ�ߣ�δ���豸�������͡��ṩ200us�ĳ�ʱʱ��
	{
		retry++;
		Delay_us(1);
	};
	if(retry>=200) return 1;																		//��ʱ��Ϊ��Ӧ��200us��
    else retry=0;
	while(!DS18B20_IN && retry<240)															// ������Ӧ��  �ж����ŵ͵�ƽʱ���Ƿ����60us��С��240us��������Ӧ����
	{
		retry++;
		Delay_us(1);
	}
	if(retry>=240)return 1;  																		//Ӧ���ʱ�����ʧ��     
  return 0; 																								//����ɹ�������0
	
}

uint8_t DS18B20_Read_Bit(void) 															
{
	uint8_t data;																								//�ݴ�����
	DS18B20_GPIO_OUT();																					//�л����ģʽ
	DS18B20_OUT_RESET; 																						//����
	Delay_us(2);
	DS18B20_OUT_SET;																						//�ͷ�
	DS18B20_GPIO_IN();																					//�л�����ģʽ
	Delay_us(12);
	if(DS18B20_IN)data =1 ;
	else data = 0;
	Delay_us(50);
	//printf("���ݣ�%d\r\n",data);
	return data;

}

void DS18B20_Write_One(void) 
{
  DS18B20_GPIO_OUT();        																//SET PG11 OUTPUT;
	DS18B20_OUT_RESET;																						//����2us
	Delay_us(2);                           
	DS18B20_OUT_SET;
	Delay_us(60);
}

void DS18B20_Write_Zero(void)  
{
	DS18B20_GPIO_OUT();        																	//SET PG11 OUTPUT;
	DS18B20_OUT_RESET;																						//����2us   
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
   DS18B20_RST();            																//��λ
   DS18B20_Check();    		 																	//�ȴ���Ӧ
   DS18B20_Write_Byte(0xcc);  															// ����һ�� ROM ָ��		0ccH����ROM
   DS18B20_Write_Byte(0x44);    														// ���ʹ洢��ָ��		�����¶�ת��	
}
  	 
uint8_t DS18B20_Init(void)
{
	DS18B20_RST();

	return DS18B20_Check();
}  
// ��ȡ��������ֵ
short	DS18B20_Get_Temp(void)
{
	uint8_t temp;
	uint8_t TH,TL;
	short tem;
	DS18B20_Start();
	DS18B20_RST();
	DS18B20_Check();
	DS18B20_Write_Byte(0xCC); 																// ����ROM��ַ��ֱ�ӷ�������
	DS18B20_Write_Byte(0xBE); 																// ��ȡ�ݴ�����9�ֽ�����
	TL = DS18B20_Read_Byte(); 																// ��8λ
	TH = DS18B20_Read_Byte(); 																// �Ͱ�λ
	
	if(TH>7)
	{
			TH=~TH;
			TL=~TL; 
			temp=0;																								//�¶�Ϊ��  
	}else temp=1;																							//�¶�Ϊ��	  	  
	tem=TH; 																									//��ø߰�λ
	tem<<=8;    
	tem+=TL;																									//��õװ�λ
	tem=(float)tem*0.625;																			//ת��     
	if(temp)return tem; 																			//�����¶�ֵ
	else return -tem;    
}


