//单片机头文件
#include "main.h"

//网络设备驱动
#include "esp-01.h"

//硬件驱动
#include "usart.h"

//C库
#include <string.h>
#include <stdio.h>

//OLED显示
#include "oled.h"


unsigned char esp8266_buf[128];
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;
uint8_t esp8266_buff = 0;

uint8_t OLED_ESP[128] = {0};

//==========================================================
//	函数名称：	ESP8266_Clear
//
//	函数功能：	清空缓存
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;

}

//==========================================================
//	函数名称：	ESP8266_WaitRecive
//
//	函数功能：	等待接收完成
//
//	入口参数：	无
//
//	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
//
//	说明：		循环调用检测是否接收完成
//==========================================================
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{
		esp8266_cnt = 0;							//清0接收计数
			
		return REV_OK;								//返回接收完成标志
	}
		
	esp8266_cntPre = esp8266_cnt;					//置为相同
	
	return REV_WAIT;								//返回接收未完成标志

}

//==========================================================
//	函数名称：	ESP8266_SendCmd
//
//	函数功能：	发送命令
//
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;

	HAL_UART_Transmit(&huart1,(unsigned char *)cmd,strlen((const char *)cmd),50);
	
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//如果收到数据
		{
			if(strstr((const char *)esp8266_buf, res) != NULL)		//如果检索到关键词
			{
				ESP8266_Clear();									//清空缓存
				
				return 0;
			}
		}
		
		HAL_Delay(10);
	}
	
	return 1;

}

//==========================================================
//	函数名称：	ESP8266_SendData
//
//	函数功能：	发送数据
//
//	入口参数：	data：数据
//				len：长度
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	ESP8266_Clear();								//清空接收缓存
	sprintf(cmdBuf, "AT+CIPSEND=0,%d\r\n", len);		//发送命令
	if(!ESP8266_SendCmd(cmdBuf, "OK"))				//收到‘>’时可以发送数据
	{
		HAL_UART_Transmit(&huart1,data,len,50);
	}

}

//==========================================================
//	函数名称：	ESP8266_GetIPD
//
//	函数功能：	获取平台返回的数据
//
//	入口参数：	等待的时间(乘以10ms)
//
//	返回参数：	平台返回的原始数据
//
//	说明：		不同网络设备返回的格式不同，需要去调试
//				如ESP8266的返回格式为	"+IPD,x:yyy"	x代表数据长度，yyy是数据内容
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	
	do
	{
		if(ESP8266_WaitRecive() == REV_OK)								//如果接收完成
		{
			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//搜索“IPD”头
			if(ptrIPD == NULL)											//如果没找到，可能是IPD头的延迟，还是需要等待一会，但不会超过设定的时间
			{
//				OLED_ShowString(0,0,"IPD not found",12,1);
			}
			else
			{
//				sprintf((char *)OLED_ESP,"             ");
//				OLED_ShowString(0,0,OLED_ESP,12,1);
				ptrIPD = strchr(ptrIPD, ':');							//找到':'
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
    timeOut--;		//延时等待
	} while(timeOut>0);
	
	return NULL;														//超时还未找到，返回空指针

}
//==========================================================
//	函数名称：	ESP8266_Init
//
//	函数功能：	初始化ESP8266
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
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
//	函数名称：	USART1_IRQHandler
//
//	函数功能：	串口1收发中断
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(esp8266_cnt >= sizeof(esp8266_buf))	esp8266_cnt = 0;
	esp8266_buf[esp8266_cnt++] = esp8266_buff;
	HAL_UART_Receive_IT(&huart1,(uint8_t *)&esp8266_buff,1);
}
