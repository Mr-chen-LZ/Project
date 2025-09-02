#include <REGX52.H>
#include "LCD1602.h"
#include "DS1302.h"
#include "Key.h"
#include "Delay.h"
#include "Buzzer.h"
#include "Timer0.h"
#include "DHT11.h"

unsigned char KeyNum, MODE, TimeSetSelect, TimeSetFlashFlag;
unsigned char AlarmSetSelect, AlarmSetFlashFlag;
unsigned char AlarmTime[6] = {25, 9, 2, 00, 00, 10}; // 默认闹钟时间
bit AlarmTriggered = 0;
extern unsigned int rec_dat[9];

// 添加日期动画相关变量
unsigned char DateAnimationState = 0;  // 0:无动画 1-6:动画阶段
unsigned int DateAnimationCounter = 0;
unsigned char LastDate[3] = {0, 0, 0};  // 保存上次的日期(年、月、日)
unsigned char NewDate[3] = {0, 0, 0};   // 保存新的日期(年、月、日)
unsigned char AnimationBuffer[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void TimeShow(void)//时间显示功能
{
	DS1302_ReadTime();//读取时间
	
	// 检查日期是否变化
	if((LastDate[0] != DS1302_Time[0] || LastDate[1] != DS1302_Time[1] || LastDate[2] != DS1302_Time[2]) && DateAnimationState == 0)
	{
		// 日期变化，启动动画
		NewDate[0] = DS1302_Time[0];
		NewDate[1] = DS1302_Time[1];
		NewDate[2] = DS1302_Time[2];
		DateAnimationState = 1;
		DateAnimationCounter = 0;
		
		// 初始化动画缓冲区 - 只初始化前6位
		AnimationBuffer[0] = LastDate[0] / 10;  // 年的十位
		AnimationBuffer[1] = LastDate[0] % 10;  // 年的个位
		AnimationBuffer[2] = LastDate[1] / 10;  // 月的十位
		AnimationBuffer[3] = LastDate[1] % 10;  // 月的个位
		AnimationBuffer[4] = LastDate[2] / 10;  // 日的十位
		AnimationBuffer[5] = LastDate[2] % 10;  // 日的个位
	}
	
	// 如果有动画，显示动画内容
	if(DateAnimationState > 0)
	{
		LCD_ShowNum(1,2,AnimationBuffer[0]*10+AnimationBuffer[1],2);
		LCD_ShowNum(1,5,AnimationBuffer[2]*10+AnimationBuffer[3],2);
		LCD_ShowNum(1,8,AnimationBuffer[4]*10+AnimationBuffer[5],2);
	}
	else
	{
		// 无动画时正常显示
		LCD_ShowNum(1,2,DS1302_Time[0],2);//显示年
		LCD_ShowNum(1,5,DS1302_Time[1],2);//显示月
		LCD_ShowNum(1,8,DS1302_Time[2],2);//显示日
		
		// 更新上次日期
		LastDate[0] = DS1302_Time[0];
		LastDate[1] = DS1302_Time[1];
		LastDate[2] = DS1302_Time[2];
	}
	
	// 正常显示时分秒
	LCD_ShowNum(2,2,DS1302_Time[3],2);//显示时
	LCD_ShowNum(2,5,DS1302_Time[4],2);//显示分
	LCD_ShowNum(2,8,DS1302_Time[5],2);//显示秒
	
	// 显示当前模式
	if(MODE == 0) LCD_ShowString(1,1,"A");
}

// 处理日期动画
void ProcessDateAnimation(void)
{
  unsigned char i; // 变量定义在函数最前面
    
	if(DateAnimationState == 0) return;
	
	// 每500ms切换一帧 (因为DateAnimationCounter每1ms增加一次)
	if(DateAnimationCounter < 500) return;
	
	DateAnimationCounter = 0;
	
	// 执行动画步骤
	switch(DateAnimationState)
	{
		case 1: // 第一步：准备新日期数字
			// 将新日期的数字添加到缓冲区末尾
			AnimationBuffer[6] = NewDate[0] / 10;
			AnimationBuffer[7] = NewDate[0] % 10;
			AnimationBuffer[8] = NewDate[1] / 10;
			AnimationBuffer[9] = NewDate[1] % 10;
			AnimationBuffer[10] = NewDate[2] / 10;
			AnimationBuffer[11] = NewDate[2] % 10;
			DateAnimationState = 2;
			break;
			
		case 2: // 第二步：向左移动一位
		case 3: // 第三步：向左移动一位
		case 4: // 第四步：向左移动一位
		case 5: // 第五步：向左移动一位
		case 6: // 第六步：向左移动一位，完成动画
			for(i=0; i<10; i++)
			{
				AnimationBuffer[i] = AnimationBuffer[i+1];
			}
			
			if(DateAnimationState == 6)
			{
				// 更新最后日期
				LastDate[0] = NewDate[0];
				LastDate[1] = NewDate[1];
				LastDate[2] = NewDate[2];
				DateAnimationState = 0; // 结束动画
			}
			else
			{
				DateAnimationState++; // 进入下一阶段
			}
			break;
	}
}


void TimeSet(void)//时间设置功能
{
	if(MODE == 1) LCD_ShowString(1,1,"B");
	if(KeyNum==2)//按键2按下
	{
		TimeSetSelect++;//设置选择位加1
		TimeSetSelect%=6;//越界清零
	}
	if(KeyNum==3)//按键3按下
	{
		DS1302_Time[TimeSetSelect]++;//时间设置位数值加1
		if(DS1302_Time[0]>99){DS1302_Time[0]=0;}//年越界判断
		if(DS1302_Time[1]>12){DS1302_Time[1]=1;}//月越界判断
		if( DS1302_Time[1]==1 || DS1302_Time[1]==3 || DS1302_Time[1]==5 || DS1302_Time[1]==7 || 
			DS1302_Time[1]==8 || DS1302_Time[1]==10 || DS1302_Time[1]==12)//日越界判断
		{
			if(DS1302_Time[2]>31){DS1302_Time[2]=1;}//大月
		}
		else if(DS1302_Time[1]==4 || DS1302_Time[1]==6 || DS1302_Time[1]==9 || DS1302_Time[1]==11)
		{
			if(DS1302_Time[2]>30){DS1302_Time[2]=1;}//小月
		}
		else if(DS1302_Time[1]==2)
		{
			if(DS1302_Time[0]%4==0)
			{
				if(DS1302_Time[2]>29){DS1302_Time[2]=1;}//闰年2月
			}
			else
			{
				if(DS1302_Time[2]>28){DS1302_Time[2]=1;}//平年2月
			}
		}
		if(DS1302_Time[3]>23){DS1302_Time[3]=0;}//时越界判断
		if(DS1302_Time[4]>59){DS1302_Time[4]=0;}//分越界判断
		if(DS1302_Time[5]>59){DS1302_Time[5]=0;}//秒越界判断
	}
	if(KeyNum==4)//按键4按下
	{
		DS1302_Time[TimeSetSelect]--;//时间设置位数值减1
		if(DS1302_Time[0]<0){DS1302_Time[0]=99;}//年越界判断
		if(DS1302_Time[1]<1){DS1302_Time[1]=12;}//月越界判断
		if( DS1302_Time[1]==1 || DS1302_Time[1]==3 || DS1302_Time[1]==5 || DS1302_Time[1]==7 || 
			DS1302_Time[1]==8 || DS1302_Time[1]==10 || DS1302_Time[1]==12)//日越界判断
		{
			if(DS1302_Time[2]<1){DS1302_Time[2]=31;}//大月
			if(DS1302_Time[2]>31){DS1302_Time[2]=1;}
		}
		else if(DS1302_Time[1]==4 || DS1302_Time[1]==6 || DS1302_Time[1]==9 || DS1302_Time[1]==11)
		{
			if(DS1302_Time[2]<1){DS1302_Time[2]=30;}//小月
			if(DS1302_Time[2]>30){DS1302_Time[2]=1;}
		}
		else if(DS1302_Time[1]==2)
		{
			if(DS1302_Time[0]%4==0)
			{
				if(DS1302_Time[2]<1){DS1302_Time[2]=29;}//闰年2月
				if(DS1302_Time[2]>29){DS1302_Time[2]=1;}
			}
			else
			{
				if(DS1302_Time[2]<1){DS1302_Time[2]=28;}//平年2月
				if(DS1302_Time[2]>28){DS1302_Time[2]=1;}
			}
		}
		if(DS1302_Time[3]<0){DS1302_Time[3]=23;}//时越界判断
		if(DS1302_Time[4]<0){DS1302_Time[4]=59;}//分越界判断
		if(DS1302_Time[5]<0){DS1302_Time[5]=59;}//秒越界判断
	}
	//更新显示，根据TimeSetSelect和TimeSetFlashFlag判断可完成闪烁功能
	if(TimeSetSelect==0 && TimeSetFlashFlag==1){LCD_ShowString(1,2,"  ");}
	else {LCD_ShowNum(1,2,DS1302_Time[0],2);}
	if(TimeSetSelect==1 && TimeSetFlashFlag==1){LCD_ShowString(1,5,"  ");}
	else {LCD_ShowNum(1,5,DS1302_Time[1],2);}
	if(TimeSetSelect==2 && TimeSetFlashFlag==1){LCD_ShowString(1,8,"  ");}
	else {LCD_ShowNum(1,8,DS1302_Time[2],2);}
	if(TimeSetSelect==3 && TimeSetFlashFlag==1){LCD_ShowString(2,2,"  ");}
	else {LCD_ShowNum(2,2,DS1302_Time[3],2);}
	if(TimeSetSelect==4 && TimeSetFlashFlag==1){LCD_ShowString(2,5,"  ");}
	else {LCD_ShowNum(2,5,DS1302_Time[4],2);}
	if(TimeSetSelect==5 && TimeSetFlashFlag==1){LCD_ShowString(2,8,"  ");}
	else {LCD_ShowNum(2,8,DS1302_Time[5],2);}
}

void AlarmSet(void)//闹钟设置功能
{
	if(MODE == 2) LCD_ShowString(1,1,"C");
	if(KeyNum==2)//按键2按下
	{
		AlarmSetSelect++;//设置选择位加1
		AlarmSetSelect%=6;//越界清零
	}
	if(KeyNum==3)//按键3按下
	{
		AlarmTime[AlarmSetSelect]++;//闹钟设置位数值加1
		if(AlarmTime[0]>99){AlarmTime[0]=0;}//年越界判断
		if(AlarmTime[1]>12){AlarmTime[1]=1;}//月越界判断
		if( AlarmTime[1]==1 || AlarmTime[1]==3 || AlarmTime[1]==5 || AlarmTime[1]==7 || 
			AlarmTime[1]==8 || AlarmTime[1]==10 || AlarmTime[1]==12)//日越界判断
		{
			if(AlarmTime[2]>31){AlarmTime[2]=1;}//大月
		}
		else if(AlarmTime[1]==4 || AlarmTime[1]==6 || AlarmTime[1]==9 || AlarmTime[1]==11)
		{
			if(AlarmTime[2]>30){AlarmTime[2]=1;}//小月
		}
		else if(AlarmTime[1]==2)
		{
			if(AlarmTime[0]%4==0)
			{
				if(AlarmTime[2]>29){AlarmTime[2]=1;}//闰年2月
			}
			else
			{
				if(AlarmTime[2]>28){AlarmTime[2]=1;}//平年2月
			}
		}
		if(AlarmTime[3]>23){AlarmTime[3]=0;}//时越界判断
		if(AlarmTime[4]>59){AlarmTime[4]=0;}//分越界判断
		if(AlarmTime[5]>59){AlarmTime[5]=0;}//秒越界判断
	}
	if(KeyNum==4)//按键4按下
	{
		AlarmTime[AlarmSetSelect]--;//闹钟设置位数值减1
		if(AlarmTime[0]<0){AlarmTime[0]=99;}//年越界判断
		if(AlarmTime[1]<1){AlarmTime[1]=12;}//月越界判断
		if( AlarmTime[1]==1 || AlarmTime[1]==3 || AlarmTime[1]==5 || AlarmTime[1]==7 || 
			AlarmTime[1]==8 || AlarmTime[1]==10 || AlarmTime[1]==12)//日越界判断
		{
			if(AlarmTime[2]<1){AlarmTime[2]=31;}//大月
			if(AlarmTime[2]>31){AlarmTime[2]=1;}
		}
		else if(AlarmTime[1]==4 || AlarmTime[1]==6 || AlarmTime[1]==9 || AlarmTime[1]==11)
		{
			if(AlarmTime[2]<1){AlarmTime[2]=30;}//小月
			if(AlarmTime[2]>30){AlarmTime[2]=1;}
		}
		else if(AlarmTime[1]==2)
		{
			if(AlarmTime[0]%4==0)
			{
				if(AlarmTime[2]<1){AlarmTime[2]=29;}//闰年2月
				if(AlarmTime[2]>29){AlarmTime[2]=1;}
			}
			else
			{
				if(AlarmTime[2]<1){AlarmTime[2]=28;}//平年2月
				if(AlarmTime[2]>28){AlarmTime[2]=1;}
			}
		}
		if(AlarmTime[3]<0){AlarmTime[3]=23;}//时越界判断
		if(AlarmTime[4]<0){AlarmTime[4]=59;}//分越界判断
		if(AlarmTime[5]<0){AlarmTime[5]=59;}//秒越界判断
	}
	//更新显示，根据AlarmSetSelect和AlarmSetFlashFlag判断可完成闪烁功能
	if(AlarmSetSelect==0 && AlarmSetFlashFlag==1){LCD_ShowString(1,2,"  ");}
	else {LCD_ShowNum(1,2,AlarmTime[0],2);}
	if(AlarmSetSelect==1 && AlarmSetFlashFlag==1){LCD_ShowString(1,5,"  ");}
	else {LCD_ShowNum(1,5,AlarmTime[1],2);}
	if(AlarmSetSelect==2 && AlarmSetFlashFlag==1){LCD_ShowString(1,8,"  ");}
	else {LCD_ShowNum(1,8,AlarmTime[2],2);}
	if(AlarmSetSelect==3 && AlarmSetFlashFlag==1){LCD_ShowString(2,2,"  ");}
	else {LCD_ShowNum(2,2,AlarmTime[3],2);}
	if(AlarmSetSelect==4 && AlarmSetFlashFlag==1){LCD_ShowString(2,5,"  ");}
	else {LCD_ShowNum(2,5,AlarmTime[4],2);}
	if(AlarmSetSelect==5 && AlarmSetFlashFlag==1){LCD_ShowString(2,8,"  ");}
	else {LCD_ShowNum(2,8,AlarmTime[5],2);}
}

void CheckAlarm(void)
{
    static unsigned char lastSecond = 0;
    
    if(AlarmTriggered == 0)
    {
        if(DS1302_Time[0] == AlarmTime[0] &&
           DS1302_Time[1] == AlarmTime[1] &&
           DS1302_Time[2] == AlarmTime[2] &&
           DS1302_Time[3] == AlarmTime[3] &&
           DS1302_Time[4] == AlarmTime[4] &&
           DS1302_Time[5] == AlarmTime[5] &&
           DS1302_Time[5] != lastSecond) // 确保每秒只触发一次
        {
            AlarmTriggered = 1;
        }
    }
    lastSecond = DS1302_Time[5];
}

void led_dis(void)
{
	P2=0xFE;//1111 1110
	Delay(100);
	P2=0xFD;//1111 1101
	Delay(100);
	P2=0xFB;//1111 1011
	Delay(100);
	P2=0xF7;//1111 0111
	Delay(100);
	P2=0xEF;//1110 1111
	Delay(100);
	P2=0xDF;//1101 1111
	Delay(100);
	P2=0xBF;//1011 1111
	Delay(100);
	P2=0x7F;//0111 1111
	Delay(100);
}

void main()
{
	LCD_Init();
	DHT11_receive();
	DS1302_Init();
	Timer0Init();
	DS1302_SetTime();//设置时间
	
	// 初始化上次日期
	DS1302_ReadTime();
	LastDate[0] = DS1302_Time[0];
	LastDate[1] = DS1302_Time[1];
	LastDate[2] = DS1302_Time[2];
	
	Delay(1000);			//等待转换完成
	LCD_ShowString(1,2,"  -  -  ");//静态字符初始化显示
	LCD_ShowString(2,2,"  :  :  ");
	LCD_ShowChar(1,12,'T');
	LCD_ShowChar(2,12,'H');
	LCD_ShowChar(1,13,':');
	LCD_ShowChar(2,13,':');
	LCD_ShowChar(1,16,'C');
	LCD_ShowChar(2,16,'%');
	
	
	
	while(1)
	{
		Delay(100);
		DHT11_receive();
		LCD_ShowNum(1,14,rec_dat[2],2);
		LCD_ShowNum(2,14,rec_dat[0],2);
		Delay(100);
		
		if(AlarmTriggered == 0)
		{
			KeyNum=Key();//读取键码
			if(KeyNum==1)//按键1按下
			{
				MODE++;//功能切换
				if(MODE>2) MODE=0;
				if(MODE==1){TimeSetSelect=0;}//时间设置模式
				else if(MODE==2){AlarmSetSelect=0;}//闹钟设置模式
				else if(MODE==0){DS1302_SetTime();}//正常显示模式
			}
			
			switch(MODE)//根据不同的功能执行不同的函数
			{
				case 0:
					TimeShow();
					ProcessDateAnimation(); // 处理日期动画
					break;
				case 1:TimeSet();break;
				case 2:AlarmSet();break;
			}
			CheckAlarm();//检查闹钟
		}
		else if(AlarmTriggered == 1)
		{
			Buzzer_Time(2000);
			led_dis();
			AlarmTriggered = 0;
		}
	}
}

void Timer0_Routine() interrupt 1
{
	static unsigned int T0Count;
	static unsigned int AnimationCount;
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	T0Count++;
	AnimationCount++;
	
	if(T0Count >= 500)  // 500ms - 用于闪烁标志
	{
		T0Count = 0;
		AlarmSetFlashFlag = !AlarmSetFlashFlag;
		TimeSetFlashFlag  = !TimeSetFlashFlag;
	}
	
	// 日期动画计数器 - 每1ms增加一次
	if(DateAnimationState != 0)
	{
		DateAnimationCounter++;
	}
}
