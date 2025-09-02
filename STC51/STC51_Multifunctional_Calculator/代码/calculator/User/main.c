#include "delay.h"
#include "lcd12864.h"
#include "key_4x4.h"
#include "timer.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <math.h>


// 按键值映射宏
#define KEY_1       1
#define KEY_2       2
#define KEY_3       3
#define KEY_4       4
#define KEY_5       5
#define KEY_6       6
#define KEY_7       7
#define KEY_8       8
#define KEY_9       9
#define KEY_0       10
#define KEY_PLUS    11    // +
#define KEY_MINUS   12    // -
#define KEY_MULTI   13    // *
#define KEY_EQUAL   14    // =
#define KEY_AC      15    // AC
#define KEY_DIV     16    // ÷

// 特殊按键值
#define KEY_BACKSPACE 0xAA  // 退格

// 扩展运算符类型
#define OP_POWER        17  // 幂运算
#define OP_PERCENT      18  // 取余运算
#define OP_SQRT         19  // 开方运算
#define OP_RECIPROCAL   20  // 倒数运算

// 运算状态宏
#define OP_STD_PLUS     0
#define OP_ALT_PLUS     1
#define OP_STD_MINUS    0
#define OP_ALT_MINUS    1
#define OP_STD_MULTI    0
#define OP_ALT_MULTI    1
#define OP_STD_DIV      0
#define OP_ALT_DIV      1

// 长按阈值（毫秒）
#define LONG_PRESS_THRESHOLD 500

// 最大输入长度
#define MAX_INPUT_LENGTH 15
#define MAX_DISPLAY_LENGTH 8

// 计算器状态
typedef enum {
    STATE_INPUT_FIRST,
    STATE_OPERATOR,
    STATE_INPUT_SECOND,
    STATE_RESULT,
    STATE_ERROR
} calc_state_t;

// 全局变量
calc_state_t calc_state = STATE_INPUT_FIRST;
uint8 plus_state = OP_STD_PLUS;
uint8 minus_state = OP_STD_MINUS;
uint8 multi_state = OP_STD_MULTI;
uint8 div_state = OP_STD_DIV;

uint8 is_long_press = 0;  // 长按标志

char display_line1[9] = {0};  // 第一行：第一个数 + 运算符
char display_line2[9] = {0};  // 第二行：第二个数或当前输入
char display_line3[9] = {0};  // 第三行：结果

char input_buffer[16] = {0};
uint8 buffer_index = 0;

char num2_display[9] = {0};  // 用于保存第二个数的显示

double num1 = 0, num2 = 0, result = 0;
uint8 current_operator = 0;
uint8 is_negative = 0;
uint8 has_decimal = 0;
uint8 decimal_places = 0;
uint8 error_flag = 0;

// 函数声明
void clear_all(void);
void process_key(uint8 key_val);
void update_display(void);
void display_error(const char* message);
void add_char_to_buffer(char c);
void backspace(void);
void calculate_result(void);
void reset_operator_states(void);
double get_current_number(void);
void format_number_for_display(double value, char* buffer, uint8 max_len);
void handle_operator(uint8 op_key);
void toggle_operator_state(uint8 op_key);
uint8 check_overflow(double value);
void process_digit_key(uint8 key_val);
void process_equal_key(uint32 press_duration);
void handle_single_operand_operator(uint8 op_key);
void handle_double_operand_operator(uint8 op_key);

// 主函数
void main(void)
{
    uint8 key_val;
    static uint32 equal_press_time = 0;
    static uint8 equal_pressed = 0;
    uint32 press_duration = 0;
    
    // 端口初始化
    P0M1 = 0x00; P0M0 = 0x00;
    P2M1 &= 0xE5; P2M0 &= 0xE5;
    P2M1 &= 0x3F; P2M0 &= 0x3F;
    P4M1 &= 0x0F; P4M0 &= 0x0F;
    P5M1 &= 0xF3; P5M0 &= 0xF3;
    
    Timer0_Init();
    LCD_Init();
    clear_all();
    
    while(1)
    {
        key_val = KeyScan();
        
        if(key_val != 0)
        {
            delay_ms(20);
            if(KeyScan() == key_val)
            {
                if (key_val == KEY_EQUAL)
                {
                    // 开始计时等号键按下
                    equal_pressed = 1;
                    equal_press_time = GetSysTick();
                }
                else
                {
                    process_key(key_val);
                }
                while(KeyScan() != 0); // 等待按键释放
                
                // 等号键释放后的处理
                if (equal_pressed)
                {
                    equal_pressed = 0;
                    press_duration = GetSysTick() - equal_press_time;
                    process_equal_key(press_duration);
                }
            }
        }
    }
}

// 处理等号键
void process_equal_key(uint32 press_duration)
{
    if (press_duration >= LONG_PRESS_THRESHOLD)
    {
        process_key(KEY_EQUAL);  // 长按：计算
    }
    else
    {
        process_key(KEY_BACKSPACE);  // 短按：退格
    }
}

// 清除所有状态
void clear_all(void)
{
    buffer_index = 0;
    num1 = 0;
    num2 = 0;
    result = 0;
    current_operator = 0;
    is_negative = 0;
    has_decimal = 0;
    decimal_places = 0;
    error_flag = 0;
    calc_state = STATE_INPUT_FIRST;
    
    reset_operator_states();
    memset(input_buffer, 0, sizeof(input_buffer));
    memset(display_line1, 0, sizeof(display_line1));
    memset(display_line2, 0, sizeof(display_line2));
    memset(display_line3, 0, sizeof(display_line3));
    memset(num2_display, 0, sizeof(num2_display));
    
    // 清空显示
    Fill_GDRAM(0);
    LCD_ShowString(1, 0, MAX_DISPLAY_LENGTH, "        ");
    LCD_ShowString(2, 0, MAX_DISPLAY_LENGTH, "        ");
    LCD_ShowString(3, 0, MAX_DISPLAY_LENGTH, "        ");
}
// 重置运算符状态
void reset_operator_states(void)
{
    plus_state = OP_STD_PLUS;
    minus_state = OP_STD_MINUS;
    multi_state = OP_STD_MULTI;
    div_state = OP_STD_DIV;
}

// 获取当前缓冲区中的数字
double get_current_number(void)
{
    char temp_buffer[16];
    if (buffer_index == 0) return 0.0;
    
    if (is_negative) {
        sprintf(temp_buffer, "-%s", input_buffer);
    } else {
        strcpy(temp_buffer, input_buffer);
    }
    
    return atof(temp_buffer);
}

void format_number_for_display(double value, char* buffer, uint8 max_len)
{
    char temp_buffer[32];  // 使用足够大的临时缓冲区
    char *dot;
    char *p;
    
    // 初始化缓冲区
    buffer[0] = '\0';
    
    // 检查是否为特殊值
    if (value != value) {
        strncpy(buffer, "NaN", max_len);
        buffer[max_len-1] = '\0';
        return;
    }
    
    // 检查无穷大
    if (value > 1e308 || value < -1e308) {
        if (value > 0) {
            strncpy(buffer, "Inf", max_len);
        } else {
            strncpy(buffer, "-Inf", max_len);
        }
        buffer[max_len-1] = '\0';
        return;
    }
    
    // 处理很大或很小的数字使用科学计数法
    if (value > 99999999.9999 || (value < 0.0001 && value > 1e-9 && value != 0)) {
        sprintf(temp_buffer, "%.2e", value);
    } 
    // 处理整数
    else if (fabs(value - (int)value) < 1e-9) {
        sprintf(temp_buffer, "%d", (int)value);
    } 
    // 处理小数
    else {
        sprintf(temp_buffer, "%.6f", value);
        // 去除末尾的0
        dot = strchr(temp_buffer, '.');
        if (dot != NULL) {
            p = temp_buffer + strlen(temp_buffer) - 1;
            while (p > dot && *p == '0') {
                *p = '\0';
                p--;
            }
            if (p == dot) {
                *p = '\0';
            }
        }
    }
    
    // 安全地复制到目标缓冲区，确保不超过最大长度
    strncpy(buffer, temp_buffer, max_len);
    buffer[max_len-1] = '\0';  // 确保以null结尾
}


// 添加字符到缓冲区
void add_char_to_buffer(char c)
{
    if (buffer_index >= MAX_INPUT_LENGTH) return;
    
    // 处理小数点
    if (c == '.') {
        if (has_decimal) return;  // 已经有一个小数点了
        if (buffer_index == 0) {
            // 第一个字符是小数点，自动补0
            input_buffer[buffer_index++] = '0';
        }
        has_decimal = 1;
        input_buffer[buffer_index++] = c;
    }
    // 处理数字
    else if (c >= '0' && c <= '9') {
        if (has_decimal) {
            decimal_places++;
            if (decimal_places > 6) return;  // 最多6位小数
        }
        input_buffer[buffer_index++] = c;
    }
    
    input_buffer[buffer_index] = '\0';
}

// 退格处理
void backspace(void)
{
    if (buffer_index > 0) {
        if (input_buffer[buffer_index-1] == '.') {
            has_decimal = 0;
        }
        buffer_index--;
        input_buffer[buffer_index] = '\0';
        
        // 重新计算小数位数
        if (has_decimal) {
            char *dot = strchr(input_buffer, '.');
            if (dot) {
                decimal_places = strlen(dot + 1);
            } else {
                has_decimal = 0;
                decimal_places = 0;
            }
        }
    } else if (is_negative) {
        is_negative = 0;
    }
}

// 溢出检查函数
uint8 check_overflow(double value)
{
    if (value != value) return 1;  // NaN
    
    // 兼容性方法检查无穷大
    if (value > 1e308 || value < -1e308) return 1;  // 近似无穷大检查
    
    if (fabs(value) > 1e15) return 1;  // 溢出
    return 0;
}

// 处理数字键
void process_digit_key(uint8 key_val)
{
    char digit_char;
    
    // 确定数字字符
    if (key_val == KEY_0) {
        digit_char = '0';
    } else {
        digit_char = '0' + (key_val - KEY_1 + 1);
    }
    
    // 状态处理
    if (calc_state == STATE_RESULT || calc_state == STATE_ERROR) {
        clear_all();
        calc_state = STATE_INPUT_FIRST;
    } else if (calc_state == STATE_OPERATOR) {
        clear_all();
        calc_state = STATE_INPUT_FIRST;
    }
    
    // 添加数字到缓冲区
    add_char_to_buffer(digit_char);
}

// 处理单操作数运算符
void handle_single_operand_operator(uint8 op_key)
{
    num1 = get_current_number();
    format_number_for_display(num1, display_line1, sizeof(display_line1));
    
    switch (op_key) {
        case KEY_MINUS:
            minus_state = OP_ALT_MINUS;
            strcpy(display_line2, "r");
            current_operator = OP_SQRT;
            break;
        case KEY_MULTI:
            multi_state = OP_ALT_MULTI;
            strcpy(display_line2, "1/x");
            current_operator = OP_RECIPROCAL;
            break;
    }
    
    calc_state = STATE_OPERATOR;
}

// 处理双操作数运算符
void handle_double_operand_operator(uint8 op_key)
{
    num1 = get_current_number();
    format_number_for_display(num1, display_line1, sizeof(display_line1));
    
    switch (op_key) {
        case KEY_PLUS:
            plus_state = !plus_state;
            current_operator = (plus_state == OP_STD_PLUS) ? KEY_PLUS : OP_POWER;
            strcpy(display_line2, (plus_state == OP_STD_PLUS) ? "+" : "^");
            break;
        case KEY_MINUS:
            minus_state = OP_STD_MINUS;
            current_operator = KEY_MINUS;
            strcpy(display_line2, "-");
            break;
        case KEY_MULTI:
            multi_state = OP_STD_MULTI;
            current_operator = KEY_MULTI;
            strcpy(display_line2, "*");
            break;
        case KEY_DIV:
            div_state = !div_state;
            current_operator = (div_state == OP_STD_DIV) ? KEY_DIV : OP_PERCENT;
            strcpy(display_line2, (div_state == OP_STD_DIV) ? "/" : "%");
            break;
    }
    
    calc_state = STATE_INPUT_SECOND;
    
    // 清空输入缓冲区
    buffer_index = 0;
    is_negative = 0;
    has_decimal = 0;
    memset(input_buffer, 0, sizeof(input_buffer));
    // 注意：这里不要清空 num2_display，让它保持之前的值
}

// 处理运算符
void handle_operator(uint8 op_key)
{
    if (calc_state == STATE_INPUT_FIRST && buffer_index > 0) {
        // 处理减号的特殊逻辑（负号或运算符）
        if (op_key == KEY_MINUS && buffer_index == 0 && !is_negative) {
            is_negative = 1;
            return;
        }
        
        // 区分单操作数和双操作数运算符
        if ((op_key == KEY_MINUS && minus_state == OP_ALT_MINUS) || 
            (op_key == KEY_MULTI && multi_state == OP_ALT_MULTI)) {
            handle_single_operand_operator(op_key);
        } else {
            handle_double_operand_operator(op_key);
        }
    }
    else if (calc_state == STATE_OPERATOR || calc_state == STATE_INPUT_SECOND) {
        // 切换运算符状态
        switch (op_key) {
            case KEY_PLUS:
                plus_state = !plus_state;
                current_operator = (plus_state == OP_STD_PLUS) ? KEY_PLUS : OP_POWER;
                strcpy(display_line2, (plus_state == OP_STD_PLUS) ? "+" : "^");
                calc_state = STATE_INPUT_SECOND;
                break;
            case KEY_MINUS:
                minus_state = !minus_state;
                if (minus_state == OP_ALT_MINUS) {
                    strcpy(display_line2, "r");
                    current_operator = OP_SQRT;
                    calc_state = STATE_OPERATOR;
                } else {
                    strcpy(display_line2, "-");
                    current_operator = KEY_MINUS;
                    calc_state = STATE_INPUT_SECOND;
                }
                break;
            case KEY_MULTI:
                multi_state = !multi_state;
                if (multi_state == OP_ALT_MULTI) {
                    strcpy(display_line2, "1/x");
                    current_operator = OP_RECIPROCAL;
                    calc_state = STATE_OPERATOR;
                } else {
                    strcpy(display_line2, "*");
                    current_operator = KEY_MULTI;
                    calc_state = STATE_INPUT_SECOND;
                }
                break;
            case KEY_DIV:
                div_state = !div_state;
                current_operator = (div_state == OP_STD_DIV) ? KEY_DIV : OP_PERCENT;
                strcpy(display_line2, (div_state == OP_STD_DIV) ? "/" : "%");
                calc_state = STATE_INPUT_SECOND;
                break;
        }
        
        if (calc_state == STATE_INPUT_SECOND) {
            buffer_index = 0;
            is_negative = 0;
            has_decimal = 0;
            memset(input_buffer, 0, sizeof(input_buffer));
        }
    }
}

// 更新显示
void update_display(void)
{
    char line1_buf[16] = {0};
    char temp_buf[16] = {0};
    
    // 清空显示区域
    LCD_ShowString(1, 0, MAX_DISPLAY_LENGTH, "        ");
    LCD_ShowString(2, 0, MAX_DISPLAY_LENGTH, "        ");
    LCD_ShowString(3, 0, MAX_DISPLAY_LENGTH, "        ");
    
    // 第一行：第一个数 + 运算符
    if (strlen(display_line1) > 0) {
        if (strlen(display_line2) > 0) {
            sprintf(line1_buf, "%s%s", display_line1, display_line2);
        } else {
            strcpy(line1_buf, display_line1);
        }
        if (strlen(line1_buf) > MAX_DISPLAY_LENGTH) {
            line1_buf[MAX_DISPLAY_LENGTH] = '\0';
        }
        LCD_ShowString(1, 0, strlen(line1_buf), (uint8*)line1_buf);
    }
    
    // 第二行：当前输入的数字或第二个数
    if (calc_state == STATE_INPUT_FIRST || calc_state == STATE_INPUT_SECOND) {
        if (buffer_index > 0) {
            if (is_negative) {
                sprintf(temp_buf, "-%s", input_buffer);
            } else {
                strcpy(temp_buf, input_buffer);
            }
        } else {
            if (calc_state == STATE_INPUT_SECOND) {
                strcpy(temp_buf, "0");
            } else {
                strcpy(temp_buf, "");
            }
        }
        
        if (strlen(temp_buf) > MAX_DISPLAY_LENGTH) {
            temp_buf[MAX_DISPLAY_LENGTH] = '\0';
        }
        LCD_ShowString(2, 0, strlen(temp_buf), (uint8*)temp_buf);
    }
    else if (calc_state == STATE_OPERATOR) {
        LCD_ShowString(2, 0, 8, "Press =");
    }
    else if (calc_state == STATE_RESULT || calc_state == STATE_ERROR) {
        // 结果显示时，第二行显示第二个数
        if (strlen(num2_display) > 0) {
            // 确保显示长度不超过限制
            if (strlen(num2_display) > MAX_DISPLAY_LENGTH) {
                char truncated[MAX_DISPLAY_LENGTH + 1];
                strncpy(truncated, num2_display, MAX_DISPLAY_LENGTH);
                truncated[MAX_DISPLAY_LENGTH] = '\0';
                LCD_ShowString(2, 0, MAX_DISPLAY_LENGTH, (uint8*)truncated);
            } else {
                LCD_ShowString(2, 0, strlen(num2_display), (uint8*)num2_display);
            }
        } else {
            // 如果没有第二个数（单操作数运算），显示运算符描述
            if (strlen(display_line2) > 0) {
                LCD_ShowString(2, 0, strlen(display_line2), (uint8*)display_line2);
            } else {
                LCD_ShowString(2, 0, MAX_DISPLAY_LENGTH, "        ");
            }
        }
    }
    
    // 第三行：结果或错误信息
    if (calc_state == STATE_RESULT || calc_state == STATE_ERROR) {
        LCD_ShowString(3, 0, strlen(display_line3), (uint8*)display_line3);
    } else {
        LCD_ShowString(3, 0, MAX_DISPLAY_LENGTH, "        ");
    }
}


// 处理按键输入
void process_key(uint8 key_val)
{
    if (error_flag && key_val != KEY_AC) return;
    
    switch(key_val)
    {
        case KEY_0: 
        case KEY_1: 
        case KEY_2: 
        case KEY_3: 
        case KEY_4: 
        case KEY_5: 
        case KEY_6: 
        case KEY_7: 
        case KEY_8: 
        case KEY_9: 
            process_digit_key(key_val);
            break;
            
        case KEY_BACKSPACE:
            backspace();
            break;
            
        case KEY_PLUS:
        case KEY_MINUS:
        case KEY_MULTI:
        case KEY_DIV:
            handle_operator(key_val);
            break;
            
        case KEY_EQUAL:
            if (calc_state == STATE_INPUT_SECOND || calc_state == STATE_OPERATOR) {
                if (calc_state == STATE_INPUT_SECOND) {
                    num2 = get_current_number();
                    // 在计算前保存第二个数的显示（使用原始输入）
                    if (buffer_index > 0) {
                        if (is_negative) {
                            sprintf(num2_display, "-%s", input_buffer);
                        } else {
                            strcpy(num2_display, input_buffer);
                        }
                    } else {
                        strcpy(num2_display, "0");
                    }
                } else {
                    // 单操作数运算时，使用运算符描述作为第二个数显示
                    strcpy(num2_display, display_line2);
                }
                calculate_result();
            }
            break;
            
        case KEY_AC:
            clear_all();
            break;
            
        default:
            break;
    }
    
    update_display();
}


// 显示错误信息
void display_error(const char* message)
{
    error_flag = 1;
    calc_state = STATE_ERROR;
    strncpy(display_line3, message, MAX_DISPLAY_LENGTH);
    display_line3[MAX_DISPLAY_LENGTH] = '\0';
}

// 计算结果
void calculate_result(void)
{    
    switch(current_operator)
    {
        case KEY_PLUS: result = num1 + num2; break;
        case KEY_MINUS: result = num1 - num2; break;
        case KEY_MULTI: result = num1 * num2; break;
        case KEY_DIV:
            if (fabs(num2) < 1e-9) {
                display_error("Div by 0");
                return;
            }
            result = num1 / num2;
            break;
        case OP_POWER: result = pow(num1, num2); break;
        case OP_PERCENT:
            if (fabs(num2) < 1e-9) {
                display_error("Mod by 0");
                return;
            }
            result = fmod(num1, num2);
            break;
        case OP_SQRT:
            if (num1 < 0) {
                display_error("SQRT neg");
                return;
            }
            result = sqrt(num1);
            break;
        case OP_RECIPROCAL:
            if (fabs(num1) < 1e-9) {
                display_error("Div by 0");
                return;
            }
            result = 1.0 / num1;
            break;
    }
    
    if (check_overflow(result)) {
        display_error("Overflow");
        return;
    }
    
    calc_state = STATE_RESULT;
    format_number_for_display(result, display_line3, sizeof(display_line3));
}