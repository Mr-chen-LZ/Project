//��Ƭ��ͷ�ļ�
#include "main.h"

//�����豸����
#include "esp-01.h"

//Ӳ������
#include "usart.h"

//C��
#include <string.h>
#include <stdio.h>

//OLED��ʾ
#include "oled.h"


unsigned char esp8266_buf[128];
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;
uint8_t esp8266_buff = 0;

uint8_t OLED_ESP[128] = {0};

//==========================================================
//	�������ƣ�	ESP8266_Clear
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;

}

//==========================================================
//	�������ƣ�	ESP8266_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//	��ڲ�����	��
//
//	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		esp8266_cnt = 0;							//��0���ռ���
			
		return REV_OK;								//���ؽ�����ɱ�־
	}
		
	esp8266_cntPre = esp8266_cnt;					//��Ϊ��ͬ
	
	return REV_WAIT;								//���ؽ���δ��ɱ�־

}

//==========================================================
//	�������ƣ�	ESP8266_SendCmd
//
//	�������ܣ�	��������
//
//	��ڲ�����	cmd������
//				res����Ҫ���ķ���ָ��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;

	HAL_UART_Transmit(&huart1,(unsigned char *)cmd,strlen((const char *)cmd),50);
	
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//����յ�����
		{
			if(strstr((const char *)esp8266_buf, res) != NULL)		//����������ؼ���
			{
				ESP8266_Clear();									//��ջ���
				
				return 0;
			}
		}
		
		HAL_Delay(10);
	}
	
	return 1;

}

//==========================================================
//	�������ƣ�	ESP8266_SendData
//
//	�������ܣ�	��������
//
//	��ڲ�����	data������
//				len������
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	ESP8266_Clear();								//��ս��ջ���
	sprintf(cmdBuf, "AT+CIPSEND=0,%d\r\n", len);		//��������
	if(!ESP8266_SendCmd(cmdBuf, "OK"))				//�յ���>��ʱ���Է�������
	{
		HAL_UART_Transmit(&huart1,data,len,50);
	}

}

//==========================================================
//	�������ƣ�	ESP8266_GetIPD
//
//	�������ܣ�	��ȡƽ̨���ص�����
//
//	��ڲ�����	�ȴ���ʱ��(����10ms)
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
//				��ESP8266�ķ��ظ�ʽΪ	"+IPD,x:yyy"	x�������ݳ��ȣ�yyy����������
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	
	do
	{
		if(ESP8266_WaitRecive() == REV_OK)								//����������
		{
			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//������IPD��ͷ
			if(ptrIPD == NULL)											//���û�ҵ���������IPDͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
			{
//				OLED_ShowString(0,0,"IPD not found",12,1);
			}
			else
			{
//				sprintf((char *)OLED_ESP,"             ");
//				OLED_ShowString(0,0,OLED_ESP,12,1);
				ptrIPD = strchr(ptrIPD, ':');							//�ҵ�':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
				
			}
		}
		
		HAL_Delay(5);
    timeOut--;		//��ʱ�ȴ�
	} while(timeOut>0);
	
	return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��

}
//==========================================================
//	�������ƣ�	ESP8266_Init
//
//	�������ܣ�	��ʼ��ESP8266
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_Init(void)
{
	HAL_UART_Receive_IT(&huart1,(uint8_t *)&esp8266_buff,1);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET);
	HAL_Delay(250);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_SET);
	HAL_Delay(1000);
                                  
	ESP8266_Clear();
  sprintf((char *)OLED_ESP,"Init....    ");
	OLED_ShowString(0,0,OLED_ESP,12,1);
	while(ESP8266_SendCmd("AT\r\n", "OK"))
	{
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET);
		HAL_Delay(250);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_SET);
		HAL_Delay(1000);		
	}
  sprintf((char *)OLED_ESP,"Library    ");
	OLED_ShowString(0,0,OLED_ESP,12,1);
	sprintf((char *)OLED_ESP,"reservation");
	OLED_ShowString(0,12,OLED_ESP,12,1);
	sprintf((char *)OLED_ESP,"system");
	OLED_ShowString(0,24,OLED_ESP,12,1);
	sprintf((char *)OLED_ESP,"Init....");
	OLED_ShowString(0,36,OLED_ESP,12,1);
	while(ESP8266_SendCmd("AT+CWMODE=2\r\n", "OK"))
	HAL_Delay(500);
	while(ESP8266_SendCmd("AT+CWSAP=\"ESP8266\",\"123456789\",4,4\r\n", "OK"))
	HAL_Delay(500);
	while(ESP8266_SendCmd("AT+RST\r\n", ""));
	HAL_Delay(500);	
	while(ESP8266_SendCmd("AT+CIPMUX=1\r\n", "OK"));
	HAL_Delay(500);	
	while(ESP8266_SendCmd("AT+CIPSERVER=1,5050\r\n", "OK"));
	OLED_Clear();
	HAL_Delay(500);	
	sprintf((char *)OLED_ESP,"Init Succeed");
	OLED_ShowString(0,48,OLED_ESP,12,1);
	HAL_Delay(500);	
	OLED_Clear();
	ESP8266_Clear();

}

//==========================================================
//	�������ƣ�	USART1_IRQHandler
//
//	�������ܣ�	����1�շ��ж�
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(esp8266_cnt >= sizeof(esp8266_buf))	esp8266_cnt = 0;
	esp8266_buf[esp8266_cnt++] = esp8266_buff;
	HAL_UART_Receive_IT(&huart1,(uint8_t *)&esp8266_buff,1);
}
