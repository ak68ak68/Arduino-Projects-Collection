#include <Servo.h>

// 定义触发引脚和回声引脚
const int trigPin = 9;
const int echoPin = 10;

// 定义内置 LED 引脚
const int ledPin = 13;

// 定义舵机控制引脚
const int servoPin = 11;

// 创建舵机对象
Servo myServo;

// 定义变量用于存储距离
float distance;

// 定义测量模式
bool autoMeasure = false;

// 上次测量的时间
unsigned long previousMillis = 0;
// 上次 LED 状态改变的时间
unsigned long previousLedMillis = 0;
// 测量间隔时间（毫秒）
const long interval = 100;
// LED 闪烁间隔
unsigned long ledInterval;

// LED 状态
bool ledState = LOW;

// 定义变量用于存储多次测量的总和
float totalDistance = 0;
// 定义变量用于存储测量次数
int measurementCount = 0;
// 定义测量次数上限
const int maxMeasurements = 2;

// 舵机当前角度
int currentServoAngle = 0;
// 舵机运动步长
const int servoStep = 5;

void setup() {
  // 初始化串口通信
  Serial.begin(9600);
  // 将触发引脚设置为输出模式
  pinMode(trigPin, OUTPUT);
  // 将回声引脚设置为输入模式
  pinMode(echoPin, INPUT);
  // 将 LED 引脚设置为输出模式
  pinMode(ledPin, OUTPUT);

  // 连接舵机到指定引脚
  myServo.attach(servoPin);
  // 初始化舵机角度
  myServo.write(currentServoAngle);
}

void loop() {
  // 检查串口是否有数据输入
  if (Serial.available() > 0) {
    // 读取串口输入的字符串
    String command = Serial.readStringUntil('\n');
    if (command == "自动测量") {
      autoMeasure = true;
      Serial.println("已开启自动测量模式，每 1 秒更新一次距离信息。");
    } else if (command == "手动测量") {
      autoMeasure = false;
      Serial.println("已开启手动测量模式，输入 '测距' 进行一次测量。");
    } else if (command == "测距" &&!autoMeasure) {
      measureDistance();
      Serial.print("距离: ");
      Serial.print(distance);
      Serial.println(" cm");
    }
  }

  // 自动测量模式
  if (autoMeasure) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      measureDistance();
      Serial.print("距离: ");
      Serial.print(distance);
      Serial.println(" cm");

      // 根据距离计算 LED 闪烁间隔
      // 这里简单将距离映射到 100 - 1000 毫秒的闪烁间隔
      ledInterval = map(distance, 0, 200, 100, 1000);

      // 根据距离计算目标舵机角度，将 2 - 20cm 映射到 0 - 180°
      int targetServoAngle = map(distance, 2, 20, 0, 180);
      // 确保目标角度在 0 - 180° 范围内
      targetServoAngle = constrain(targetServoAngle, 0, 180);

      // 逐步调整舵机角度
      if (currentServoAngle < targetServoAngle) {
        currentServoAngle = min(currentServoAngle + servoStep, targetServoAngle);
      } else if (currentServoAngle > targetServoAngle) {
        currentServoAngle = max(currentServoAngle - servoStep, targetServoAngle);
      }
      myServo.write(currentServoAngle);
    }

    // 控制 LED 闪烁
    if (currentMillis - previousLedMillis >= ledInterval) {
      previousLedMillis = currentMillis;
      ledState =!ledState;
      digitalWrite(ledPin, ledState);
    }
  }
}

// 测量距离的函数
void measureDistance() {
  // 清空触发引脚
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // 发送 10 微秒的高电平脉冲触发超声波模块
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // 读取回声引脚的脉冲持续时间
  long duration = pulseIn(echoPin, HIGH);
  // 根据声速（340m/s）计算距离
  float currentDistance = duration * 0.034 / 2;

  // 过滤异常数据
  if (currentDistance > 0) {
    totalDistance += currentDistance;
    measurementCount++;
  }

  // 达到测量次数上限时计算平均值
  if (measurementCount >= maxMeasurements) {
    distance = totalDistance / measurementCount;
    totalDistance = 0;
    measurementCount = 0;
  }
}