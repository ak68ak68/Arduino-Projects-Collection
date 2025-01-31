// 引入 Servo 库，该库用于控制舵机
#include <Servo.h>

// 定义舵机对象，后续将使用这个对象来控制舵机的转动
Servo myServo;

// 定义滑动变阻器连接的引脚，这里使用 Arduino 的模拟输入引脚 A0
const int potPin = A0;
// 定义 LED 连接的引脚，使用 Arduino 开发板上的数字引脚 13
const int ledPin = 13;

// 定义舵机 PWM 信号范围
// 舵机的最小 PWM 信号宽度为 500 微秒，对应舵机的最小转动角度
const int minPulse = 500;
// 舵机的最大 PWM 信号宽度为 2500 微秒，对应舵机的最大转动角度
const int maxPulse = 2500;

// 定义 LED 闪烁间隔时间范围
// 最快闪烁间隔为 100 毫秒，即 LED 闪烁最快的时间间隔
const int minBlinkInterval = 100;
// 最慢闪烁间隔为 1000 毫秒，即 LED 闪烁最慢的时间间隔
const int maxBlinkInterval = 1000;

// 记录上一次 LED 闪烁的时间，用于实现定时闪烁功能
unsigned long previousMillis = 0;
// 当前 LED 的闪烁间隔时间，会根据滑动变阻器的位置而改变
int blinkInterval = maxBlinkInterval;
// LED 的当前状态，LOW 表示熄灭，HIGH 表示点亮
bool ledState = LOW;

// 模式选择标志，用于记录用户选择的工作模式，初始值为空格表示未选择
char mode = ' ';

// setup() 函数在程序开始时只执行一次，用于初始化硬件和设置初始参数
void setup() {
  // 初始化舵机，将舵机连接到数字引脚 9，并设置其 PWM 信号的最小和最大宽度
  myServo.attach(9, minPulse, maxPulse);
  
  // 将 LED 引脚设置为输出模式，以便可以控制 LED 的点亮和熄灭
  pinMode(ledPin, OUTPUT);
  
  // 打开串口通信，设置波特率为 9600，用于与计算机进行数据交互
  Serial.begin(9600);
  
  // 在串口监视器上显示提示信息，让用户选择工作模式
  Serial.println("选择模式：电阻模拟(A)或串口发送角度(B)");
}

// loop() 函数会不断循环执行，实现程序的主要功能
void loop() {
  // 如果还没有选择工作模式
  if (mode == ' ') {
    // 检查串口是否有数据可读
    if (Serial.available() > 0) {
      // 读取串口接收到的字符，并将其赋值给模式选择标志
      mode = Serial.read();
      // 如果用户输入的字符不是 'A' 也不是 'B'
      if (mode != 'A' && mode != 'B') {
        // 在串口监视器上显示错误提示信息，让用户重新输入
        Serial.println("无效的选择，请输入 A 或 B");
        // 将模式选择标志重置为空格，表示未选择模式
        mode = ' ';
      }
    }
  } 
  // 如果用户选择了电阻模拟模式（A）
  else if (mode == 'A') {
    // 读取滑动变阻器连接的模拟引脚 A0 的值，范围是 0 - 1023
    int potValue = analogRead(potPin);
    
    // 将滑动变阻器读取到的模拟值（0 - 1023）映射到舵机的角度范围（0 - 180 度）
    int servoAngle = map(potValue, 0, 1023, 0, 180);
    
    // 将滑动变阻器的模拟值映射到 LED 的闪烁间隔时间范围（maxBlinkInterval - minBlinkInterval）
    blinkInterval = map(potValue, 0, 1023, maxBlinkInterval, minBlinkInterval);
    
    // 根据计算得到的角度值，控制舵机转动到相应的角度
    myServo.write(servoAngle);
    
    // 在串口监视器上打印滑动变阻器的值、舵机的角度和 LED 的闪烁间隔时间，方便调试和观察
    Serial.print("Potentiometer Value: ");
    Serial.print(potValue);
    Serial.print(", Servo Angle: ");
    Serial.print(servoAngle);
    Serial.print(", Blink Interval: ");
    Serial.println(blinkInterval);
  } 
  // 如果用户选择了串口发送角度模式（B）
  else if (mode == 'B') {
    // 检查串口是否有数据可读
    if (Serial.available() > 0) {
      // 从串口读取一个整数值，作为用户输入的舵机角度
      int angle = Serial.parseInt();
      // 检查输入的角度是否在 0 - 180 度的有效范围内
      if (angle >= 0 && angle <= 180) {
        // 根据用户输入的有效角度，控制舵机转动到相应的角度
        myServo.write(angle);
        // 在串口监视器上显示舵机已经转动到的角度
        Serial.print("舵机已转到角度: ");
        Serial.println(angle);
      } else {
        // 如果输入的角度无效，在串口监视器上显示错误提示信息
        Serial.println("输入的角度无效，请输入 0 - 180 度之间的角度");
      }
    }
  }
  
  // 获取当前的系统时间，单位为毫秒
  unsigned long currentMillis = millis();
  
  // 判断是否已经达到了 LED 的闪烁间隔时间
  if (currentMillis - previousMillis >= blinkInterval) {
    // 更新上一次 LED 闪烁的时间为当前时间
    previousMillis = currentMillis;
    
    // 切换 LED 的状态，即如果当前是点亮状态则熄灭，如果是熄灭状态则点亮
    ledState = !ledState;
    // 根据切换后的状态控制 LED 引脚的电平，实现 LED 的闪烁
    digitalWrite(ledPin, ledState);
  }
  
  // 稍微延迟 10 毫秒，避免程序读取数据过快，减少资源占用
  delay(10);
}