#include	"key_4x4.h"
#include	"timer.h"


/**************************************************************************************
 * 描  述 : 4x4矩阵扫描函数
 * 入  参 : 无
 * 返回值 : 哪个按键按下的对应值
 **************************************************************************************/
uint8 KeyScan(void)
{
    uint8 X_temp = 0xF0, Y_temp = 0x0F, temp;
    uint8 key_val = 0;

    // 行输出高，列输出低，检测行
    P2M1 &= 0x3F; P2M0 |= 0xC0;        // P2.6/P2.7推挽输出
    P4M1 &= 0x0F; P4M0 |= 0xF0;        // P4.4-P4.7推挽输出
    P5M1 &= 0xF3; P5M0 |= 0x0C;        // P5.2/P5.3推挽输出

    ROW1=1; ROW2=1; ROW3=1; ROW4=1;
    COL1=0; COL2=0; COL3=0; COL4=0;

    delay_ms(10);
    P4M1 &= 0x0F; P4M0 &= 0x0F;        // P4.4-P4.7改为输入
    delay_ms(10);

    if(ROW1 == 0) {delay_ms(10); if(ROW1 == 0) Y_temp &= 0x0E;}
    if(ROW2 == 0) {delay_ms(10); if(ROW2 == 0) Y_temp &= 0x0D;}
    if(ROW3 == 0) {delay_ms(10); if(ROW3 == 0) Y_temp &= 0x0B;}
    if(ROW4 == 0) {delay_ms(10); if(ROW4 == 0) Y_temp &= 0x07;}

    // 列输出高，行输出低，检测列
    P2M1 &= 0x3F; P2M0 |= 0xC0;
    P4M1 &= 0x0F; P4M0 |= 0xF0;
    P5M1 &= 0xF3; P5M0 |= 0x0C;

    ROW1=0; ROW2=0; ROW3=0; ROW4=0;
    COL1=1; COL2=1; COL3=1; COL4=1;

    delay_ms(10);
    P2M1 &= 0x3F; P2M0 &= 0x3F;        // P2.6/P2.7改为输入
    P5M1 &= 0xF3; P5M0 &= 0xF3;        // P5.2/P5.3改为输入
    delay_ms(10);

    if(COL1 == 0) {delay_ms(10); if(COL1 == 0) X_temp &= 0xE0;}
    if(COL2 == 0) {delay_ms(10); if(COL2 == 0) X_temp &= 0xD0;}
    if(COL3 == 0) {delay_ms(10); if(COL3 == 0) X_temp &= 0xB0;}
    if(COL4 == 0) {delay_ms(10); if(COL4 == 0) X_temp &= 0x70;}

    // 计算按键值
    temp = X_temp | Y_temp;
    temp = ~temp;
    switch(temp)
    {
        case 0x11: key_val=1; break;    //1
        case 0x21: key_val=2; break;    //2    
        case 0x41: key_val=3; break;    //3
        case 0x81: key_val=4; break;    //4
        case 0x12: key_val=5; break;    //5
        case 0x22: key_val=6; break;    //6
        case 0x42: key_val=7; break;    //7
        case 0x82: key_val=8; break;    //8
        case 0x14: key_val=9; break;    //9
        case 0x24: key_val=10; break;   //0
        case 0x44: key_val=11; break;   //+
        case 0x84: key_val=12; break;   //-
        case 0x18: key_val=13; break;   //*
        case 0x28: key_val=14; break;   //=
        case 0x48: key_val=15; break;   //AC
        case 0x88: key_val=16; break;   //÷
        default: key_val=0; break;
    }

    return key_val;  // 直接返回按键值
}
/*********************************END FILE********************************************/	

