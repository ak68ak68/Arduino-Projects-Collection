#include <Servo.h>

// 定义触发引脚和回声引脚
// 触发引脚用于向超声波模块发送触发信号，启动测距过程
const int trigPin = 9;
// 回声引脚用于接收超声波模块返回的回声信号，通过测量回声信号的持续时间来计算距离
const int echoPin = 10;

// 定义内置 LED 引脚
// 该引脚连接 Arduino 板上的内置 LED，用于根据测量距离控制 LED 闪烁
const int ledPin = 13;

// 定义舵机控制引脚
// 该引脚连接 SG90 舵机的信号引脚，用于向舵机发送控制信号，改变舵机的角度
const int servoPin = 11;

// 创建舵机对象
// 使用 Servo 库创建一个舵机对象，方便后续对舵机进行控制
Servo myServo;

// 定义变量用于存储距离
// 存储超声波模块测量得到的距离值
float distance;
// 存储上一次测量的距离值，用于计算距离的变化量
float lastDistance = 0;

// 定义测量模式
// 布尔变量，用于控制测量模式，true 表示自动测量模式，false 表示手动测量模式
bool autoMeasure = false;

// 上次测量的时间
// 记录上一次进行距离测量的时间，用于实现定时测量功能
unsigned long previousMillis = 0;
// 上次 LED 状态改变的时间
// 记录上一次改变 LED 状态（亮/灭）的时间，用于控制 LED 的闪烁频率
unsigned long previousLedMillis = 0;
// 测量间隔时间（毫秒）
// 定义自动测量模式下每次测量的时间间隔，这里设置为 100 毫秒
const long interval = 100;
// LED 闪烁间隔
// 存储根据测量距离计算得到的 LED 闪烁间隔时间
unsigned long ledInterval;

// LED 状态
// 布尔变量，用于记录 LED 的当前状态，LOW 表示熄灭，HIGH 表示点亮
bool ledState = LOW;

// 定义变量用于存储多次测量的总和
// 用于累加多次测量得到的距离值，以便计算平均值，提高测量的准确性
float totalDistance = 0;
// 定义变量用于存储测量次数
// 记录当前已经进行的测量次数，当达到一定次数后计算平均值
int measurementCount = 0;
// 定义测量次数上限
// 规定进行多少次测量后计算一次平均值，这里设置为 5 次
const int maxMeasurements = 1;

// 舵机当前角度
// 记录舵机当前所处的角度
int currentServoAngle = 0;
// 最小步长
// 定义舵机转动时的最小步长，当距离变化较小时，舵机以这个步长转动，保证转动的丝滑性
const int minServoStep = 15;
// 最大步长
// 定义舵机转动时的最大步长，当距离变化较大时，舵机以这个步长转动，提高响应速度
const int maxServoStep = 30;

void setup() {
  // 初始化串口通信
  // 以 9600 的波特率开启串口通信，方便与计算机进行数据交互，输出测量结果等信息
  Serial.begin(9600);
  // 将触发引脚设置为输出模式
  // 因为触发引脚需要向超声波模块发送触发信号，所以将其设置为输出模式
  Serial.println("请选择：自动测量或手动测量");
  pinMode(trigPin, OUTPUT);
  // 将回声引脚设置为输入模式
  // 回声引脚用于接收超声波模块返回的回声信号，所以将其设置为输入模式
  pinMode(echoPin, INPUT);
  // 将 LED 引脚设置为输出模式
  // LED 引脚需要控制 LED 的亮灭，所以将其设置为输出模式
  pinMode(ledPin, OUTPUT);

  // 连接舵机到指定引脚
  // 将舵机对象 myServo 连接到指定的控制引脚 servoPin
  myServo.attach(servoPin);
  // 初始化舵机角度
  // 将舵机初始角度设置为 0 度
  myServo.write(currentServoAngle);
}

void loop() {
  // 检查串口是否有数据输入
  // 通过 Serial.available() 函数检查串口是否有用户输入的数据
  if (Serial.available() > 0) {
    // 读取串口输入的字符串
    // 读取串口输入的字符串，直到遇到换行符 '\n' 为止
    String command = Serial.readStringUntil('\n');
    if (command == "自动测量") {
      // 如果用户输入的命令是 "自动测量"，则将测量模式设置为自动测量模式
      autoMeasure = true;
      // 向串口输出提示信息，告知用户已开启自动测量模式
      Serial.println("已开启自动测量模式，每 1 秒更新一次距离信息。");
    } else if (command == "手动测量") {
      // 如果用户输入的命令是 "手动测量"，则将测量模式设置为手动测量模式
      autoMeasure = false;
      // 向串口输出提示信息，告知用户已开启手动测量模式
      Serial.println("已开启手动测量模式，输入 '测距' 进行一次测量。");
    } else if (command == "测距" &&!autoMeasure) {
      // 如果用户输入的命令是 "测距" 且当前处于手动测量模式，则调用 measureDistance 函数进行一次距离测量
      measureDistance();
      // 向串口输出测量得到的距离信息
      Serial.print("距离: ");
      Serial.print(distance);
      Serial.println(" cm");
    }
  }

  // 自动测量模式
  if (autoMeasure) {
    // 获取当前时间
    unsigned long currentMillis = millis();
    // 判断是否达到测量间隔时间
    if (currentMillis - previousMillis >= interval) {
      // 更新上次测量的时间
      previousMillis = currentMillis;
      // 保存上一次测量的距离
      lastDistance = distance;
      // 调用 measureDistance 函数进行一次距离测量
      measureDistance();
      // 向串口输出测量得到的距离信息
      Serial.print("距离: ");
      Serial.print(distance);
      Serial.println(" cm");

      // 根据距离计算 LED 闪烁间隔
      // 使用 map 函数将测量距离映射到 100 - 1000 毫秒的闪烁间隔范围内
      ledInterval = map(distance, 0, 200, 100, 1000);

      // 根据距离计算目标舵机角度，将 2 - 20cm 映射到 0 - 180°
      // 使用 map 函数将测量距离映射到 0 - 180 度的舵机角度范围内
      int targetServoAngle = map(distance, 2, 20, 0, 180);
      // 确保目标角度在 0 - 180° 范围内
      // 使用 constrain 函数将目标角度限制在 0 - 180 度之间，避免舵机角度超出有效范围
      targetServoAngle = constrain(targetServoAngle, 0, 180);

      // 计算距离变化量
      // 通过当前距离与上一次距离的差值计算距离变化量
      float distanceChange = abs(distance - lastDistance);
      // 根据距离变化量动态调整步长
      // 使用 map 函数将距离变化量映射到最小步长和最大步长之间，实现动态调整步长的功能
      int servoStep = map(distanceChange, 0, 2, minServoStep, maxServoStep);

      // 逐步调整舵机角度
      if (currentServoAngle < targetServoAngle) {
        // 如果当前角度小于目标角度，则以步长为增量逐步增加当前角度，直到达到目标角度
        currentServoAngle = min(currentServoAngle + servoStep, targetServoAngle);
      } else if (currentServoAngle > targetServoAngle) {
        // 如果当前角度大于目标角度，则以步长为减量逐步减小当前角度，直到达到目标角度
        currentServoAngle = max(currentServoAngle - servoStep, targetServoAngle);
      }
      // 将调整后的角度发送给舵机，控制舵机转动
      myServo.write(currentServoAngle);
    }

    // 控制 LED 闪烁
    // 判断是否达到 LED 闪烁间隔时间
    if (currentMillis - previousLedMillis >= ledInterval) {
      // 更新上次 LED 状态改变的时间
      previousLedMillis = currentMillis;
      // 切换 LED 状态
      ledState =!ledState;
      // 根据 LED 状态控制 LED 引脚的电平，实现 LED 闪烁效果
      digitalWrite(ledPin, ledState);
    }
  }
}

// 测量距离的函数
void measureDistance() {
  // 清空触发引脚
  // 将触发引脚设置为低电平，确保触发信号的准确性
  digitalWrite(trigPin, LOW);
  // 延迟 2 微秒，让触发引脚稳定在低电平状态
  delayMicroseconds(2);
  // 发送 10 微秒的高电平脉冲触发超声波模块
  // 向触发引脚发送一个 10 微秒的高电平脉冲，启动超声波模块的测距过程
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  // 再次将触发引脚设置为低电平，结束触发信号
  digitalWrite(trigPin, LOW);

  // 读取回声引脚的脉冲持续时间
  // 使用 pulseIn 函数读取回声引脚的高电平脉冲持续时间，单位为微秒
  long duration = pulseIn(echoPin, HIGH);
  // 根据声速（340m/s）计算距离
  // 根据声速公式：距离 = 声速 × 时间 / 2，将脉冲持续时间转换为距离，单位为厘米
  float currentDistance = duration * 0.034 / 2;

  // 过滤异常数据
  // 如果测量得到的距离大于 0，则认为是有效数据，将其累加到总距离中，并增加测量次数
  if (currentDistance > 0) {
    totalDistance += currentDistance;
    measurementCount++;
  }

  // 达到测量次数上限时计算平均值
  // 当测量次数达到预设的上限时，计算多次测量的平均值作为最终的距离值，并重置总距离和测量次数
  if (measurementCount >= maxMeasurements) {
    distance = totalDistance / measurementCount;
    totalDistance = 0;
    measurementCount = 0;
  }
}