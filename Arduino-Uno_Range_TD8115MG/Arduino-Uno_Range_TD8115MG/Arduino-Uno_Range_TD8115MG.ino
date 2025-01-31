#include <Servo.h>

// 定义舵机对象
Servo myServo;

// 定义滑动变阻器连接的引脚
const int potPin = A0;
// 定义LED连接的引脚
const int ledPin = 13;

// 定义舵机PWM信号范围
const int minPulse = 500;  // 最小PWM信号（500us）
const int maxPulse = 2500; // 最大PWM信号（2500us）

// 定义闪烁间隔时间范围
const int minBlinkInterval = 10;  // 最快闪烁间隔（100ms）
const int maxBlinkInterval = 1000; // 最慢闪烁间隔（1000ms）

// 记录上一次闪烁的时间
unsigned long previousMillis = 0;
// 当前闪烁间隔时间
int blinkInterval = maxBlinkInterval;
// LED状态
bool ledState = LOW;

void setup() {
  // 初始化舵机
  myServo.attach(9, minPulse, maxPulse);
  
  // 初始化LED引脚为输出模式
  pinMode(ledPin, OUTPUT);
  
  // 打开串口通信
  Serial.begin(9600);
}

void loop() {
  // 读取滑动变阻器的模拟值
  int potValue = analogRead(potPin);
  
  // 将模拟值映射到舵机的角度范围（0 - 180度）
  int servoAngle = map(potValue, 0,1023, 0, 180);
  
  // 将模拟值映射到闪烁间隔时间范围
  blinkInterval = map(potValue, 0, 1023, maxBlinkInterval, minBlinkInterval);
  
  // 设置舵机角度
  myServo.write(servoAngle);
  
  // 获取当前时间
  unsigned long currentMillis = millis();
  
  // 判断是否到了闪烁时间
  if (currentMillis - previousMillis >= blinkInterval) {
    // 记录上一次闪烁的时间
    previousMillis = currentMillis;
    
    // 切换LED状态
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
  }
  
  // 打印滑动变阻器的值、舵机角度和闪烁间隔时间
  Serial.print("Potentiometer Value: ");
  Serial.print(potValue);
  Serial.print(", Servo Angle: ");
  Serial.print(servoAngle);
  Serial.print(", Blink Interval: ");
  Serial.println(blinkInterval);
  
  // 稍微延迟一下，避免读取过快
  delay(10);
}