#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// 定义4根灯带，每根200颗LED
#define NUM_STRIPS 4
#define LEDS_PER_STRIP 200

// 定义各灯带的数据引脚
#define STRIP1_PIN 0
#define STRIP2_PIN 1
#define STRIP3_PIN 12
#define STRIP4_PIN 18

// 创建灯带对象数组
Adafruit_NeoPixel strips[NUM_STRIPS] = {
  Adafruit_NeoPixel(LEDS_PER_STRIP, STRIP1_PIN, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(LEDS_PER_STRIP, STRIP2_PIN, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(LEDS_PER_STRIP, STRIP3_PIN, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(LEDS_PER_STRIP, STRIP4_PIN, NEO_GRB + NEO_KHZ800)
};

// 温度传感器设置
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// 温度范围定义
#define MIN_TEMP -50.0    // 最低温度（线性映射起点）
#define MAX_TEMP 50.0     // 最高温度（线性映射终点）
#define CUTOFF_TEMP -30.0 // 低于此温度全灭

// 根据温度点亮所有灯带
void updateAllStripsByTemperature(float tempC) {
  int ledsToLight = 0;
  
  // 处理不同温度范围
  if (tempC <= CUTOFF_TEMP) {
    // 低于-30°C：全灭
    ledsToLight = 0;
  } else if (tempC >= MAX_TEMP) {
    // 高于+50°C：全亮
    ledsToLight = LEDS_PER_STRIP;
  } else {
    // -30°C到+50°C之间：从-50°C到+50°C线性映射
    // 将温度从[-50, 50]映射到[0, 200]
    ledsToLight = map(tempC * 10, MIN_TEMP * 10, MAX_TEMP * 10, 0, LEDS_PER_STRIP);
    ledsToLight = constrain(ledsToLight, 0, LEDS_PER_STRIP);
  }
  
  // 对所有灯带进行同样的操作
  for (int stripNum = 0; stripNum < NUM_STRIPS; stripNum++) {
    // 先清除该灯带
    strips[stripNum].clear();
    
    // 点亮相应数量的LED（红色）
    for (int i = 0; i < ledsToLight; i++) {
      strips[stripNum].setPixelColor(i, strips[stripNum].Color(255, 0, 0)); // 纯红色
    }
    
    // 更新该灯带显示
    strips[stripNum].show();
  }
  
  // 在串口显示信息
  Serial.print("温度: ");
  Serial.print(tempC);
  Serial.print("°C -> 每根灯带点亮 ");
  Serial.print(ledsToLight);
  Serial.print(" 个LED (");
  
  if (tempC <= CUTOFF_TEMP) {
    Serial.print("全灭");
  } else if (tempC >= MAX_TEMP) {
    Serial.print("全亮");
  } else {
    Serial.print((ledsToLight * 100) / LEDS_PER_STRIP);
    Serial.print("%");
  }
  Serial.println(")");
}

// 初始化所有灯带
void initAllStrips() {
  for (int i = 0; i < NUM_STRIPS; i++) {
    strips[i].begin();
    strips[i].setBrightness(100); // 设置亮度(0-255)
    strips[i].clear();
    strips[i].show();
    delay(10);
  }
  Serial.println("所有灯带初始化完成");
}

// 清除所有灯带
void clearAllStrips() {
  for (int i = 0; i < NUM_STRIPS; i++) {
    strips[i].clear();
    strips[i].show();
  }
}

// 错误显示：所有灯带红色闪烁
void showError() {
  for (int blink = 0; blink < 3; blink++) {
    for (int stripNum = 0; stripNum < NUM_STRIPS; stripNum++) {
      for (int i = 0; i < LEDS_PER_STRIP; i++) {
        strips[stripNum].setPixelColor(i, strips[stripNum].Color(255, 0, 0));
      }
      strips[stripNum].show();
    }
    delay(200);
    
    clearAllStrips();
    delay(200);
  }
}

void setup() {
  setCpuFrequencyMhz(80);
  Serial.begin(115200);
  
  Serial.println("系统启动中...");
  Serial.print("线性映射范围: ");
  Serial.print(MIN_TEMP);
  Serial.print("°C 到 ");
  Serial.print(MAX_TEMP);
  Serial.println("°C");
  Serial.print("截止温度: ");
  Serial.print(CUTOFF_TEMP);
  Serial.println("°C (低于此温度全灭)");
  
  // 初始化温度传感器
  sensors.begin();
  
  // 初始化所有灯带
  initAllStrips();
  
  // 获取温度传感器数量
  int deviceCount = sensors.getDeviceCount();
  Serial.print("发现 ");
  Serial.print(deviceCount);
  Serial.println(" 个温度传感器");
  
  Serial.println("系统就绪，开始温度监测");
}

void loop() {
  // 请求温度转换
  sensors.requestTemperatures();
  
  // 获取温度读数
  float tempC = sensors.getTempCByIndex(0);
  
  // 检查读数是否有效
  if (tempC != DEVICE_DISCONNECTED_C) {
    // 根据温度更新所有灯带
    updateAllStripsByTemperature(tempC);
    
  } else {
    Serial.println("错误: 无法读取温度数据");
    // 显示错误信号
    showError();
  }
  
  // 每0.3秒读取一次
  delay(300);
}