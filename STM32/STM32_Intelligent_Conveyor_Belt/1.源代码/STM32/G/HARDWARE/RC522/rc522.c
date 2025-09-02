#include "RC522.h"
#include "string.h"
#include "usart.h"
 
/*
函数功能：移植接口--SPI时序读写一个字节
函数参数：data:要写入的数据
返 回 值：读到的数据
*/
uint8_t RC522_SPI_ReadWriteOneByte(uint8_t tx_data)
{			  	 
	uint8_t rx_data=0;				 
  uint8_t i;
  for(i=0;i<8;i++)
	{
		RC522_SCLK_RESET;  
		if(tx_data&0x80){RC522_OUTPUT_SET;}
		else {RC522_OUTPUT_RESET;}
		tx_data<<=1;	
		RC522_SCLK_SET;
		rx_data<<=1;
		if(RC522_INPUT)rx_data|=0x01;
	}
	return rx_data; 

}
 
 
/*
函数功能：初始化RC522的IO口	 
*/
void RC522_IO_Init(void)
{
//	RCC->APB2ENR|=1<<2;     //PA时钟使能 
//	RCC->APB2ENR|=1<<7;     //PF时钟使能
//	
//	//PA5  时钟 RC522_SCLK
//	//PA6  输入 RC522_INPUT
//	//PA7  输出 RC522_OUTPUT 
//	GPIOA->CRL&=0x000FFFFF;
//	GPIOA->CRL|=0x38300000;
//	GPIOA->ODR|=0x3<<5;
//	
//	//RC522_RST <----->PF1--复位脚
//	//RC522_SDA <----->PF0--片选脚
//	GPIOF->CRL&=0xFFFFFF00;
//	GPIOF->CRL|=0x00000033;
//	GPIOF->ODR|=0x3<<0;
}	
 
 
/*
功能描述:选卡读取卡存储器容量
输入参数:serNum 传入卡序列号
返 回 值:成功返回卡容量
*/
uint8_t RC522_MFRC522_SelectTag(uint8_t *serNum) //读取卡存储器容量
{     
	uint8_t i;     
	uint8_t status;     
	uint8_t size;     
	uint8_t recvBits;     
	uint8_t buffer[9];
	     
	buffer[0]=PICC_ANTICOLL1;	  //防撞码1     
	buffer[1]=0x70;
	buffer[6]=0x00;						     
	for(i=0;i<4;i++)					
	{
		buffer[i+2]=*(serNum+i);	//buffer[2]-buffer[5]为卡序列号
		buffer[6]^=*(serNum+i);	  //卡校验码
	}
	
	RC522_CalulateCRC(buffer,7,&buffer[7]);	//buffer[7]-buffer[8]为RCR校验码
	RC522_ClearBitMask(Status2Reg,0x08);
	status=RC522_PcdComMF522(PCD_TRANSCEIVE,buffer,9,buffer,&recvBits);
	
	if((status==MI_OK)&&(recvBits==0x18))    
		size=buffer[0];     
	else    
		size=0;
	
	return size; 
}
 
 
///*
//延时函数，纳秒级
//*/
//void RC522_Delay(uint32_t ns)
//{
//  uint32_t i;
//  for(i=0;i<ns;i++)
//  {
//    __nop();
//    __nop();
//    __nop();
//  }
//}
// 
// 使用 SysTick 实现微秒级延时
void RC522_Delay(uint32_t us) {
    uint32_t ticks = us * (SystemCoreClock / 1000000);
    uint32_t start = DWT->CYCCNT;
    while((DWT->CYCCNT - start) < ticks);
}

// 初始化 DWT(Data Watchpoint and Trace)
void DWT_Init(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}
 
/*
函数功能：RC522芯片初始化
*/
void RC522_Init(void)
{
  RC522_IO_Init();	//RC522初始化
  RC522_PcdReset();  			//复位RC522 
  RC522_PcdAntennaOff();	//关闭天线
  HAL_Delay(2);  		  //延时2毫秒
  RC522_PcdAntennaOn();		//开启天线
  M500PcdConfigISOType('A'); //设置RC632的工作方式
}
 
 
/*
函数功能：复位RC522
*/
void RC522_Reset(void)
{
  RC522_PcdReset();				//复位RC522
  RC522_PcdAntennaOff();	//关闭天线
  HAL_Delay(2);  		  //延时2毫秒
  RC522_PcdAntennaOn();		//开启天线  	
}     
 
 
/*
功    能: 寻卡
参数说明: req_code[IN]:寻卡方式
                0x52   = 寻感应区内所有符合14443A标准的卡
                0x26   = 寻未进入休眠状态的卡
          			pTagType[OUT]:卡片类型代码
                0x4400 = Mifare_UltraLight
                0x0400 = Mifare_One(S50)
                0x0200 = Mifare_One(S70)
                0x0800 = Mifare_Pro(X)
                0x4403 = Mifare_DESFire
返 回 值: 成功返回MI_OK
*/
char RC522_PcdRequest(uint8_t req_code,uint8_t *pTagType)
{
	char status;  
	uint8_t unLen;
	uint8_t ucComMF522Buf[MAXRLEN];  	   // MAXRLEN  18
 
	RC522_ClearBitMask(Status2Reg,0x08);	//清RC522寄存器位,/接收数据命令
	RC522_WriteRawRC(BitFramingReg,0x07); //写RC632寄存器
	RC522_SetBitMask(TxControlReg,0x03);  //置RC522寄存器位
 
	ucComMF522Buf[0]=req_code; 	    //寻卡方式
	
	status=RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen); //通过RC522和ISO14443卡通讯
	
	if((status==MI_OK)&&(unLen==0x10))
	{    
		*pTagType=ucComMF522Buf[0];
		*(pTagType+1)=ucComMF522Buf[1];
	}
	else
	{
	  status = MI_ERR;
	}  
	return status;
}
 
 
/*
功    能: 防冲撞
参数说明: pSnr[OUT]:卡片序列号，4字节
返    回: 成功返回MI_OK
*/
char RC522_PcdAnticoll(uint8_t *pSnr)
{
    char status;
    uint8_t i,snr_check=0;
    uint8_t unLen;
    uint8_t ucComMF522Buf[MAXRLEN]; 
    
    RC522_ClearBitMask(Status2Reg,0x08);  //清RC522寄存器位 
    RC522_WriteRawRC(BitFramingReg,0x00); //写
    RC522_ClearBitMask(CollReg,0x80);     //清
 
    ucComMF522Buf[0]=PICC_ANTICOLL1;   //PICC_ANTICOLL1 = 0x93
    ucComMF522Buf[1]=0x20;
	
    status=RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen); //0x0c,通过RC522和ISO14443卡通讯
											 //PCD_TRANSCEIVE =发送并接收数据
											 //2：写入卡里的数据字节长度
											 //ucComMF522Buf:存放数据的地址
											 //unLen：从卡里读出的数据长度
    if(status==MI_OK)
    {
    	 for(i=0;i<4;i++)
			 {   
					 *(pSnr+i)=ucComMF522Buf[i];  //把读到的卡号赋值给pSnr
					 snr_check^=ucComMF522Buf[i];
			 }
			 if(snr_check!=ucComMF522Buf[i])
			 {
					status = MI_ERR;
			 }
    }   
    RC522_SetBitMask(CollReg,0x80);
    return status;
}
 
 
/*
功    能：选定卡片
参数说明：pSnr[IN]:卡片序列号，4字节
返    回：成功返回MI_OK
*/
char RC522_PcdSelect(uint8_t *pSnr)
{
    char status;
    uint8_t i;
    uint8_t unLen;
    uint8_t ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0]=PICC_ANTICOLL1;
    ucComMF522Buf[1]=0x70;
    ucComMF522Buf[6]=0;
	
    for(i=0;i<4;i++)
    {
    	ucComMF522Buf[i+2]=*(pSnr+i);
    	ucComMF522Buf[6]^=*(pSnr+i);
    }
		
    RC522_CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]); //用MF522计算CRC16函数，校验数据
    RC522_ClearBitMask(Status2Reg,0x08);	                //清RC522寄存器位
    status=RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
    if((status==MI_OK)&&(unLen==0x18))status=MI_OK;
    else status=MI_ERR;
		
    return status;
}
 
 
/*
功    能：验证卡片密码
参数说明：auth_mode[IN]: 密码验证模式
                 0x60 = 验证A密钥
                 0x61 = 验证B密钥 
          addr[IN]：块地址
          pKey[IN]：扇区密码
          pSnr[IN]：卡片序列号，4字节
返    回：成功返回MI_OK
*/               
char RC522_PcdAuthState(uint8_t auth_mode,uint8_t addr,uint8_t *pKey,uint8_t *pSnr)
{
    char status;
    uint8_t unLen;
    uint8_t ucComMF522Buf[MAXRLEN];  //MAXRLEN  18(数组的大小)
	  
	  //验证模式+块地址+扇区密码+卡序列号   
    ucComMF522Buf[0]=auth_mode;		
    ucComMF522Buf[1]=addr;				
    memcpy(&ucComMF522Buf[2],pKey,6); //拷贝，复制
    memcpy(&ucComMF522Buf[8],pSnr,4); 
	 
    status=RC522_PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if((status!= MI_OK)||(!(RC522_ReadRawRC(Status2Reg)&0x08)))status = MI_ERR;
    return status;
}
 
 
/*
功    能：读取M1卡一块数据
参数说明： 
					addr：块地址
          p   ：读出的块数据，16字节
返    回：成功返回MI_OK
*/ 
char RC522_PcdRead(uint8_t addr,uint8_t *p)
{
    char status;
    uint8_t unLen;
    uint8_t i,ucComMF522Buf[MAXRLEN]; //18
 
    ucComMF522Buf[0]=PICC_READ;
    ucComMF522Buf[1]=addr;
    RC522_CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
    status=RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);//通过RC522和ISO14443卡通讯
    if((status==MI_OK&&(unLen==0x90)))
    {
        for(i=0;i<16;i++)
				{
						*(p +i)=ucComMF522Buf[i];
				}
    }
    else
    {   
			status=MI_ERR;
		}
    return status;
}
 
 
/*
功    能：写数据到M1卡指定块
参数说明：addr：块地址
          p   ：向块写入的数据，16字节
返    回：成功返回MI_OK
*/                  
char RC522_PcdWrite(uint8_t addr,uint8_t *p)
{
    char status;
    uint8_t unLen;
    uint8_t i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0]=PICC_WRITE;// 0xA0 //写块
    ucComMF522Buf[1]=addr;      //块地址
    RC522_CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status=RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
 
    if((status!= MI_OK)||(unLen != 4)||((ucComMF522Buf[0]&0x0F)!=0x0A))
    {
				status = MI_ERR;
		}
		
    if(status==MI_OK)
    {
        for(i=0;i<16;i++)//向FIFO写16Byte数据 
        {    
        	ucComMF522Buf[i]=*(p +i);   
        }
        RC522_CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);
        status = RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if((status != MI_OK)||(unLen != 4)||((ucComMF522Buf[0]&0x0F)!=0x0A))
        {   
					status = MI_ERR;   
				}
    }
    return status;
}
 
 
/*
功    能：命令卡片进入休眠状态
返    回：成功返回MI_OK
*/
char RC522_PcdHalt(void)
{
    uint8_t status;
    uint8_t unLen;
    uint8_t ucComMF522Buf[MAXRLEN]; //MAXRLEN==18
	  status=status;
    ucComMF522Buf[0]=PICC_HALT;
    ucComMF522Buf[1]=0;
    RC522_CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
    status=RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    return MI_OK;
}
 
 
/*
功    能：用MF522计算CRC16函数
参    数：
				*pIn ：要读数CRC的数据
				len：-数据长度
				*pOut：计算的CRC结果
*/
void RC522_CalulateCRC(uint8_t *pIn ,uint8_t len,uint8_t *pOut )
{
    uint8_t i,n;
    RC522_ClearBitMask(DivIrqReg,0x04);  //CRCIrq = 0  
    RC522_WriteRawRC(CommandReg,PCD_IDLE);
    RC522_SetBitMask(FIFOLevelReg,0x80); //清FIFO指针
    
	  //向FIFO中写入数据  
		for(i=0;i<len;i++)
    {  
			RC522_WriteRawRC(FIFODataReg,*(pIn +i));  //开始RCR计算
		}
		
		RC522_WriteRawRC(CommandReg,PCD_CALCCRC);   //等待CRC计算完成 
		i=0xFF;
    do 
    {
        n=RC522_ReadRawRC(DivIrqReg);
        i--;
    }
    while((i!=0)&&!(n&0x04));//CRCIrq = 1
	  
		//读取CRC计算结果 
		pOut[0]=RC522_ReadRawRC(CRCResultRegL);
    pOut[1]=RC522_ReadRawRC(CRCResultRegM);
}
 
 
/*
功    能：复位RC522
返    回：成功返回MI_OK
*/
char RC522_PcdReset(void)
{
	  RC522_RST_SET;   //PF1写1
    RC522_Delay(10);
	  RC522_RST_RESET;   //PF1清0
    RC522_Delay(10);
	  RC522_RST_SET;	 //PF1写1
    RC522_Delay(10);
    RC522_WriteRawRC(CommandReg,PCD_RESETPHASE);  //写RC632寄存器，复位
	  RC522_WriteRawRC(CommandReg,PCD_RESETPHASE);	//写RC632寄存器，复位
    RC522_Delay(10);
    
    RC522_WriteRawRC(ModeReg,0x3D);             //和Mifare卡通讯，CRC初始值0x6363
    RC522_WriteRawRC(TReloadRegL,30);           //写RC632寄存器   
    RC522_WriteRawRC(TReloadRegH,0);
    RC522_WriteRawRC(TModeReg,0x8D);
    RC522_WriteRawRC(TPrescalerReg,0x3E);
	
	  RC522_WriteRawRC(TxAutoReg,0x40);//必须要
    return MI_OK;
}
 
 
/*
函数功能：设置RC632的工作方式 
*/
char M500PcdConfigISOType(uint8_t type)
{
   if(type=='A')                        //ISO14443_A
   { 
		 RC522_ClearBitMask(Status2Reg,0x08);     //清RC522寄存器位
		 RC522_WriteRawRC(ModeReg,0x3D);          //3F//CRC初始值0x6363
		 RC522_WriteRawRC(RxSelReg,0x86);         //84
		 RC522_WriteRawRC(RFCfgReg,0x7F);         //4F  //调整卡的感应距离//RxGain = 48dB调节卡感应距离  
		 RC522_WriteRawRC(TReloadRegL,30);        //tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
	   RC522_WriteRawRC(TReloadRegH,0);
		 RC522_WriteRawRC(TModeReg,0x8D);
	   RC522_WriteRawRC(TPrescalerReg,0x3E);
	   RC522_Delay(1000);
     RC522_PcdAntennaOn();		//开启天线 
   }
   else return 1;       //失败，返回1
   return MI_OK;				//成功返回0
}
 
 
/*
功    能：读RC632寄存器
参数说明：Address[IN]:寄存器地址
返    回：读出的值
*/
uint8_t RC522_ReadRawRC(uint8_t Address)
{
    uint8_t ucAddr;
    uint8_t ucResult=0;		
		RC522_CS_RESET;						//片选选中RC522
    ucAddr=((Address<<1)&0x7E)|0x80;
		RC522_SPI_ReadWriteOneByte(ucAddr);		  //发送命令
		ucResult=RC522_SPI_ReadWriteOneByte(0); //读取RC522返回的数据
		RC522_CS_SET;						   //释放片选线(PF0)
		return ucResult;         //返回读到的数据
}
 
 
/*
功    能：写RC632寄存器
参数说明：Address[IN]:寄存器地址
          value[IN] :写入的值
*/
void RC522_WriteRawRC(uint8_t Address,uint8_t value)
{  
  uint8_t ucAddr;
	RC522_CS_RESET; //PF0写 0 (SDA)(SPI1片选线，低电平有效)
  ucAddr=((Address<<1)&0x7E); 
	RC522_SPI_ReadWriteOneByte(ucAddr); //SPI1发送一个字节
	RC522_SPI_ReadWriteOneByte(value);  //SPI1发送一个字节
	RC522_CS_SET;										      //PF1写1(SDA)(SPI1片选线)
}
 
 
/*
功    能：置RC522寄存器位
参数说明：reg[IN]:寄存器地址
          mask[IN]:置位值
*/
void RC522_SetBitMask(uint8_t reg,uint8_t mask)  
{
    char tmp=0x0;
    tmp=RC522_ReadRawRC(reg);					//读RC632寄存器
    RC522_WriteRawRC(reg,tmp|mask);   //写RC632寄存器
}
 
 
/*
功    能：清RC522寄存器位
参数说明：reg[IN]:寄存器地址
         mask[IN]:清位值
*/
void RC522_ClearBitMask(uint8_t reg,uint8_t mask)  
{
    char tmp=0x0;
    tmp=RC522_ReadRawRC(reg);        //读RC632寄存器
    RC522_WriteRawRC(reg,tmp&~mask); // clear bit mask
} 
 
 
/*
功    能：通过RC522和ISO14443卡通讯
参数说明：Command[IN]:RC522命令字
          pIn [IN]:通过RC522发送到卡片的数据
          InLenByte[IN]:发送数据的字节长度
          pOut [OUT]:接收到的卡片返回数据
          *pOutLenBit[OUT]:返回数据的位长度
*/
char RC522_PcdComMF522(uint8_t Command,uint8_t *pIn,uint8_t InLenByte,uint8_t *pOut,uint8_t *pOutLenBit)
{
    char status=MI_ERR;
    uint8_t irqEn=0x00;
    uint8_t waitFor=0x00;
    uint8_t lastBits;
    uint8_t n;
    uint16_t i;
	
    switch(Command)
    {
			case PCD_AUTHENT:    //验证密钥
					 irqEn=0x12;
					 waitFor=0x10;
					 break;
			case PCD_TRANSCEIVE: //发送并接收数据
					 irqEn=0x77;
					 waitFor=0x30;
					 break;
			default:
					 break;
    }
    RC522_WriteRawRC(ComIEnReg,irqEn|0x80);	
    RC522_ClearBitMask(ComIrqReg,0x80);			//清所有中断位
    RC522_WriteRawRC(CommandReg,PCD_IDLE);	
    RC522_SetBitMask(FIFOLevelReg,0x80);	 	//清FIFO缓存
    
    for(i=0;i<InLenByte;i++)
    {   
				RC522_WriteRawRC(FIFODataReg,pIn[i]);
		}
		
		RC522_WriteRawRC(CommandReg,Command);	 
		if(Command==PCD_TRANSCEIVE)
		{  
			RC522_SetBitMask(BitFramingReg,0x80);	 //开始传送
		}
    
		//有问题，下面的循环
    //i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
		  i=2000;
			do 
			{
				n=RC522_ReadRawRC(ComIrqReg);
				i--;
			}
			while((i!=0)&&!(n&0x01)&&!(n&waitFor));
				
			RC522_ClearBitMask(BitFramingReg,0x80);
			if(i!=0)
			{    
        if(!(RC522_ReadRawRC(ErrorReg)&0x1B))
        {
            status=MI_OK;
            if(n&irqEn&0x01)
            {
							status=MI_NOTAGERR;
						}
            if(Command==PCD_TRANSCEIVE)
            {
               	n=RC522_ReadRawRC(FIFOLevelReg);
              	lastBits=RC522_ReadRawRC(ControlReg)&0x07;
                if(lastBits)
                {
									*pOutLenBit=(n-1)*8+lastBits;
								}
                else
                {   
									*pOutLenBit=n*8;   
								}
								
                if(n==0)n=1;
                if(n>MAXRLEN)n=MAXRLEN;
                for(i=0; i<n; i++)
                {   
									pOut[i]=RC522_ReadRawRC(FIFODataReg);    
								}
            }
        }
        else
        {   
					status=MI_ERR;   
				}
    }
    RC522_SetBitMask(ControlReg,0x80);// stop timer now
    RC522_WriteRawRC(CommandReg,PCD_IDLE); 
    return status;
}
 
 
/*
函数功能：开启天线  
参    数：每次启动或关闭天险发射之间应至少有1ms的间隔
*/
void RC522_PcdAntennaOn(void)
{
    uint8_t i;
    i=RC522_ReadRawRC(TxControlReg);
    if(!(i&0x03))
    {
        RC522_SetBitMask(TxControlReg,0x03);
    }
}
 
 
/*
函数功能：关闭天线  
参    数：每次启动或关闭天险发射之间应至少有1ms的间隔
*/
void RC522_PcdAntennaOff(void)
{
	RC522_ClearBitMask(TxControlReg,0x03); //清RC522寄存器位
}


