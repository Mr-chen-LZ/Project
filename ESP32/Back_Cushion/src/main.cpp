#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <driver/timer.h>

// 定义 ADC 通道对应的 GPIO
#define ADC1_CH0_GPIO    0  // GPIO0 = ADC1_CH0
#define ADC1_CH1_GPIO    1  // GPIO1 = ADC1_CH1
#define ADC1_CH2_GPIO    2  // GPIO1 = ADC1_CH2
#define ADC1_CH3_GPIO    3  // GPIO1 = ADC1_CH3

Adafruit_NeoPixel  rgb_display_9(10); //定义PWM引脚为数字I/O口10，LED灯数量为10.


// ADC 通道配置
adc1_channel_t adc_channels[] = {ADC1_CHANNEL_0, ADC1_CHANNEL_1, ADC1_CHANNEL_2, ADC1_CHANNEL_3}; // 通道数组
const int adc_channel_count = sizeof(adc_channels) / sizeof(adc1_channel_t);

// ADC 校准参数
esp_adc_cal_characteristics_t adc_chars;

#define ADC_THRESHOLD_HIGH  2000  // 高阈值（单位：mV）
#define ADC_THRESHOLD_LOW   1000  // 低阈值（单位：mV）
uint32_t voltage[4] = {0};

//压力计时
volatile uint32_t timer_count = 0; // 计时变量（注意 volatile 修饰）
bool is_timer_running = false;     // 定时器状态标志

//压力消失计时
volatile uint32_t timer_count_once = 0; // 计时变量（注意 volatile 修饰）
bool is_timer_running_once = false;     // 定时器状态标志

//声明中断函数
void IRAM_ATTR onTimer();
void IRAM_ATTR onTimer_once();

// 硬件定时器初始化
void init_hw_timer() {
    hw_timer_t *timer = timerBegin(0, 80, true); // 定时器0，分频系数80（1MHz计数频率），向上计数
    timerAttachInterrupt(timer, &onTimer, true); // 绑定中断函数
    timerAlarmWrite(timer, 1000000, true);       // 1秒触发一次（1000000us）
    timerAlarmEnable(timer);                     // 启用定时器
}

void init_hw_timer_once() {
    hw_timer_t *timer = timerBegin(1, 80, true); // 定时器1，分频系数80（1MHz计数频率），向上计数
    timerAttachInterrupt(timer, &onTimer_once, true); // 绑定中断函数
    timerAlarmWrite(timer, 1000000, true);       // 1秒触发一次（1000000us）
    timerAlarmEnable(timer);                     // 启用定时器
}

// 定时器中断回调函数（在IRAM_ATTR中执行）
void IRAM_ATTR onTimer() {
    if (is_timer_running) {
        timer_count++; // 计时+1秒
    }
}

void IRAM_ATTR onTimer_once() {
    if (is_timer_running_once) {
        timer_count_once++; // 计时+1秒
    }
}

void setup() {
    setCpuFrequencyMhz(80);
    Serial.begin(115200);

    //灯带初始化 
    rgb_display_9.begin(); //引脚初始化
    rgb_display_9.setPin(9);
    for (int i = 1; i <= 10; i = i + (1))
    {
        rgb_display_9.setPixelColor(i-1, 255,255,255);//显示白色
        rgb_display_9.show();
    }

    // 初始化 ADC1
    adc1_config_width(ADC_WIDTH_BIT_12); // 12 位分辨率

    // 配置每个通道的衰减（11dB 对应满量程约 3.3V）
    for (int i = 0; i < adc_channel_count; i++) {
        adc1_config_channel_atten(adc_channels[i], ADC_ATTEN_DB_12);
    }

    // 初始化 ADC 校准（使用默认 Vref=1100mV）
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12, 1100, &adc_chars);
    // 初始化硬件定时器
    init_hw_timer(); 
    init_hw_timer_once();


}

void loop() {
    static uint32_t last_read_time = 0;
    uint32_t now = millis();
    // 轮询读取所有通道
    if (now - last_read_time >= 100) {
        for (int i = 0; i < adc_channel_count; i++) {
            int raw_value = adc1_get_raw(adc_channels[i]);
            voltage[i] = esp_adc_cal_raw_to_voltage(raw_value, &adc_chars); // 转换为电压（mV）
        }
        //待机状态判断
        if(is_timer_running == false)
        {
            is_timer_running_once = true;
            Serial.println("Timer_Once STARTED Timer");
        }
        else
        {
            is_timer_running_once = false; // 停止计时
            Serial.printf("Timer_Once STOPPED. Total time: %d seconds\n", timer_count_once);
            timer_count_once = 0; // 清零
        }
        if(is_timer_running_once == true && is_timer_running == false)
        {
            if(timer_count_once < 20)
            {
                for (int i = 1; i <= 10; i = i + (1))
                {
                    rgb_display_9.setPixelColor(i-1, 255,255,255);//显示白色
                    rgb_display_9.show();
                }
            }
            else
            {
                for (int i = 1; i <= 10; i = i + (1))
                {
                    rgb_display_9.setPixelColor(i-1, 100, 150, 255);//显示淡蓝色
                    rgb_display_9.show();
                }
            }
        }

        // 逻辑触摸启动定时判断
        if ((voltage[0] > ADC_THRESHOLD_HIGH || voltage[1] > ADC_THRESHOLD_HIGH || voltage[2] > ADC_THRESHOLD_HIGH || voltage[3] > ADC_THRESHOLD_HIGH) && !is_timer_running) {
            is_timer_running = true; // 启动计时
            Serial.println("Timer STARTED Timer");
        } 
        else if ((voltage[0] < ADC_THRESHOLD_LOW && voltage[1] < ADC_THRESHOLD_LOW && voltage[2] < ADC_THRESHOLD_LOW && voltage[3] < ADC_THRESHOLD_LOW) && is_timer_running) {
            is_timer_running = false; // 停止计时
            Serial.printf("Timer STOPPED. Total time: %d seconds\n", timer_count);
            timer_count = 0; // 清零
        }
        if(timer_count > 0 && timer_count <= 10)
        {
             for (int i = 1; i <= 10; i = i + (1))
             {
                rgb_display_9.setPixelColor(i-1, 0,0,255);//显示蓝色
                rgb_display_9.show();
            }
        }
        else if(timer_count > 10)
        {
            for (int i = 1; i <= 10; i = i + (1)) //显示红色
            {
                rgb_display_9.setPixelColor(i-1, 255,0,0); 
                rgb_display_9.show();//开始显示
            }
            delay(500);
            for (int i = 1; i <= 10; i = i + (1))
            {
                rgb_display_9.setPixelColor(i-1, 255,255,0);//显示黄色
                rgb_display_9.show();
            }
            delay(500);
        }
    }
}
