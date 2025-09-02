#include  "dht11.h"
//����
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

//����stm32��˵�������
void DH11_GPIO_Init_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin   = GPIO_PIN_11;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;      // �������
    GPIO_InitStruct.Pull  = GPIO_NOPULL;              // ��������
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;     // ���٣�50MHz��

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

//����stm32��˵��������
void DH11_GPIO_Init_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin   = GPIO_PIN_11;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;          // ����ģʽ
    GPIO_InitStruct.Pull  = GPIO_NOPULL;              // ���գ������������裩

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}



//�������Ϳ�ʼ�ź�
void DHT11_Start(void)
{
	DH11_GPIO_Init_OUT(); //���ģʽ
	
	dht11_high; //������
	DHT11_Delay(30);
	
	dht11_low; //���͵�ƽ����18us
	HAL_Delay(20);
	
	dht11_high; //���ߵ�ƽ20~40us
	DHT11_Delay(30);
	
	DH11_GPIO_Init_IN(); //����ģʽ
}


//��ȡһ���ֽ�
char DHT11_Rec_Byte(void)
{
	unsigned char i = 0;
	unsigned char data;
	
	for(i=0;i<8;i++) //1�����ݾ���1���ֽ�byte��1���ֽ�byte��8λbit
	{
		while( Read_Data == 0); //��1bit��ʼ���͵�ƽ��ߵ�ƽ���ȴ��͵�ƽ����
		DHT11_Delay(30); //�ӳ�30us��Ϊ����������0������1��0ֻ��26~28us
		
		data <<= 1; //����
		
		if( Read_Data == 1 ) //�������30us���Ǹߵ�ƽ�Ļ���������1
		{
			data |= 1; //����+1
		}
		
		while( Read_Data == 1 ); //�ߵ�ƽ��͵�ƽ���ȴ��ߵ�ƽ����
	}
	
	return data;
}

//��ȡ����

void DHT11_REC_Data(void)
{
	unsigned int R_H,R_L,T_H,T_L;
	unsigned char RH,RL,TH,TL,CHECK;
	
	DHT11_Start(); //���������ź�
	dht11_high; //���ߵ�ƽ
	
	if( Read_Data == 0 ) //�ж�DHT11�Ƿ���Ӧ
	{
		while( Read_Data == 0); //�͵�ƽ��ߵ�ƽ���ȴ��͵�ƽ����
		while( Read_Data == 1); //�ߵ�ƽ��͵�ƽ���ȴ��ߵ�ƽ����
		
		R_H = DHT11_Rec_Byte();
		R_L = DHT11_Rec_Byte();
		T_H = DHT11_Rec_Byte();
		T_L = DHT11_Rec_Byte();
		CHECK = DHT11_Rec_Byte(); //����5������
		
		dht11_low; //�����һbit���ݴ�����Ϻ�DHT11�������� 50us
		DHT11_Delay(55); //������ʱ55us
		dht11_high; //��������������������߽������״̬��
		
		if(R_H + R_L + T_H + T_L == CHECK) //�ͼ���λ�Աȣ��ж�У����յ��������Ƿ���ȷ
		{
			RH = R_H;
			RL = R_L;
			TH = T_H;
			TL = T_L;
		}
	}
	rec_data[0] = RH;//ʪ��
	rec_data[1] = RL;
	rec_data[2] = TH;//�¶�
	rec_data[3] = TL;
}

