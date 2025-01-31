#include <Servo.h>

//程序存在问题，舵机运动引起超声波数据异常


// 定义触发引脚和回声引脚
// trigPin 用于向超声波传感器发送触发信号，启动测距操作
const int trigPin = 9;
// echoPin 用于接收超声波传感器返回的回声信号，根据回声信号的持续时间计算距离
const int echoPin = 10;

// 定义内置 LED 引脚
// ledPin 连接到 Arduino 板上的内置 LED，用于根据测量距离控制 LED 闪烁
const int ledPin = 13;

// 定义舵机控制引脚
// servoPin 用于向舵机发送控制信号，控制舵机的转动角度
const int servoPin = 11;

// 创建舵机对象
// 使用 Servo 库创建一个舵机对象 myServo，方便后续对舵机进行控制
Servo myServo;

// 定义变量用于存储距离
// distance 存储超声波传感器测量得到的当前距离值
float distance;
// lastDistance 存储上一次测量得到的距离值，用于计算距离的变化量
float lastDistance = 0;

// 定义测量模式
// autoMeasure 是一个布尔变量，用于控制测量模式
// true 表示自动测量模式，Arduino 会按照设定的时间间隔自动进行距离测量
// false 表示手动测量模式，需要用户输入指令来触发距离测量
bool autoMeasure = false;

// 上次测量的时间
// previousMillis 记录上一次进行距离测量的时间戳，用于实现定时测量功能
unsigned long previousMillis = 0;
// 上次 LED 状态改变的时间
// previousLedMillis 记录上一次改变 LED 状态（亮或灭）的时间戳，用于控制 LED 的闪烁频率
unsigned long previousLedMillis = 0;
// 测量间隔时间（毫秒）
// interval 定义了自动测量模式下，两次距离测量之间的时间间隔，这里设置为 100 毫秒
const long interval = 200;
// LED 闪烁间隔
// ledInterval 存储根据测量距离计算得到的 LED 闪烁间隔时间，距离不同，闪烁间隔也不同
unsigned long ledInterval;

// LED 状态
// ledState 是一个布尔变量，用于记录 LED 的当前状态
// LOW 表示 LED 熄灭，HIGH 表示 LED 点亮
bool ledState = LOW;

// 定义变量用于存储多次测量的总和
// totalDistance 用于累加多次测量得到的距离值，以便后续计算平均值
float totalDistance = 0;
// 定义变量用于存储测量次数
// measurementCount 记录当前已经进行的距离测量次数，用于判断是否达到计算平均值的条件
int measurementCount = 0;
// 定义测量次数上限
// maxMeasurements 规定了进行多少次测量后计算一次平均值，这里设置为 5 次
const int maxMeasurements = 2;

// 舵机当前 PWM 信号值
// currentServoPWM 记录当前发送给舵机的 PWM 信号值，单位为微秒
int currentServoPWM = 500;
// 最小步长
// minServoStep 定义了舵机 PWM 信号值每次变化的最小步长，用于保证舵机转动的平滑性
const int minServoStep = 100;
// 最大步长
// maxServoStep 定义了舵机 PWM 信号值每次变化的最大步长，当距离变化较快时，使用较大步长提高响应速度
const int maxServoStep = 500;

void setup() {
    // 初始化串口通信
    // 以 9600 的波特率开启串口通信，方便与计算机进行数据交互，输出测量结果和提示信息
    Serial.begin(9600);
    // 输出提示信息，引导用户选择测量模式
    // 程序启动时，在串口监视器中显示提示，让用户选择自动测量或手动测量模式
    Serial.println("请选择：自动测量或手动测量");

    // 将触发引脚设置为输出模式
    // 因为 trigPin 要向超声波传感器发送触发信号，所以将其设置为输出模式
    pinMode(trigPin, OUTPUT);
    // 将回声引脚设置为输入模式
    // echoPin 用于接收超声波传感器的回声信号，所以将其设置为输入模式
    pinMode(echoPin, INPUT);
    // 将 LED 引脚设置为输出模式
    // ledPin 用于控制 LED 的亮灭，所以将其设置为输出模式
    pinMode(ledPin, OUTPUT);

    // 连接舵机到指定引脚
    // 将舵机对象 myServo 连接到指定的控制引脚 servoPin，并指定舵机所需的最小和最大 PWM 信号范围
    // 这里的 500 和 2500 分别表示最小和最大 PWM 信号值，单位为微秒
    myServo.attach(servoPin, 500, 2500);
    // 初始化舵机 PWM 信号
    // 向舵机发送初始的 PWM 信号值，将舵机初始位置设置为最小角度对应的位置
    myServo.writeMicroseconds(currentServoPWM);
}

void loop() {
    // 检查串口是否有数据输入
    // 通过 Serial.available() 函数检查串口是否接收到用户输入的数据
    if (Serial.available() > 0) {
        // 读取串口输入的字符串
        // 读取串口接收到的字符串，直到遇到换行符 '\n' 为止
        String command = Serial.readStringUntil('\n');
        if (command == "自动测量") {
            // 如果用户输入的命令是 "自动测量"，则将测量模式设置为自动测量模式
            autoMeasure = true;
            // 向串口输出提示信息，告知用户已开启自动测量模式及更新频率
            Serial.println("已开启自动测量模式，每 1 秒更新一次距离信息。");
        } else if (command == "手动测量") {
            // 如果用户输入的命令是 "手动测量"，则将测量模式设置为手动测量模式
            autoMeasure = false;
            // 向串口输出提示信息，告知用户已开启手动测量模式及触发测量的方式
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
        // 使用 millis() 函数获取当前的时间戳
        unsigned long currentMillis = millis();
        // 判断是否达到测量间隔时间
        // 通过比较当前时间和上次测量时间的差值与测量间隔时间，判断是否需要进行下一次测量
        if (currentMillis - previousMillis >= interval) {
            // 更新上次测量的时间
            // 将上次测量时间更新为当前时间，为下一次判断做准备
            previousMillis = currentMillis;
            // 保存上一次测量的距离
            // 将当前距离值保存到 lastDistance 变量中，用于后续计算距离变化量
            lastDistance = distance;
            // 调用 measureDistance 函数进行一次距离测量
            measureDistance();
            // 向串口输出测量得到的距离信息
            Serial.print("距离: ");
            Serial.print(distance);
            Serial.println(" cm");

            // 根据距离计算 LED 闪烁间隔
            // 使用 map 函数将测量距离映射到 100 - 1000 毫秒的闪烁间隔范围内
            // 距离越远，闪烁间隔越长；距离越近，闪烁间隔越短
            ledInterval = map(distance, 0, 200, 100, 1000);

            // 根据距离计算目标舵机 PWM 信号，将 2 - 20cm 映射到 500 - 2500us
            // 使用 map 函数将测量距离映射到 500 - 2500 微秒的 PWM 信号范围内
            // 距离越远，PWM 信号值越大，舵机转动角度越大；距离越近，PWM 信号值越小，舵机转动角度越小
            int targetServoPWM = map(distance, 5, 25, 500, 2500);
            // 确保目标 PWM 信号在 500 - 2500us 范围内
            // 使用 constrain 函数将目标 PWM 信号值限制在 500 - 2500 微秒之间，避免超出舵机的有效控制范围
            targetServoPWM = constrain(targetServoPWM, 500, 2500);

            // 计算距离变化量
            // 通过当前距离与上一次距离的差值计算距离变化量，取绝对值确保变化量为正数
            float distanceChange = abs(distance - lastDistance);
            // 根据距离变化量动态调整步长
            // 使用 map 函数将距离变化量映射到最小步长和最大步长之间
            // 距离变化量越大，步长越大；距离变化量越小，步长越小
            int servoStep = map(distanceChange, 0, 2, minServoStep, maxServoStep);

            // 逐步调整舵机 PWM 信号
            if (currentServoPWM < targetServoPWM) {
                // 如果当前 PWM 信号值小于目标值，则以步长为增量逐步增加当前 PWM 信号值
                // 使用 min 函数确保增加后的 PWM 信号值不超过目标值
                currentServoPWM = min(currentServoPWM + servoStep, targetServoPWM);
            } else if (currentServoPWM > targetServoPWM) {
                // 如果当前 PWM 信号值大于目标值，则以步长为减量逐步减小当前 PWM 信号值
                // 使用 max 函数确保减小后的 PWM 信号值不低于目标值
                currentServoPWM = max(currentServoPWM - servoStep, targetServoPWM);
            }
            // 向舵机发送 PWM 信号
            // 使用 writeMicroseconds 函数将当前的 PWM 信号值发送给舵机，控制舵机转动
            myServo.writeMicroseconds(currentServoPWM);
        }

        // 控制 LED 闪烁
        // 判断是否达到 LED 闪烁间隔时间
        // 通过比较当前时间和上次 LED 状态改变时间的差值与 LED 闪烁间隔时间，判断是否需要改变 LED 状态
        if (currentMillis - previousLedMillis >= ledInterval) {
            // 更新上次 LED 状态改变的时间
            // 将上次 LED 状态改变时间更新为当前时间，为下一次判断做准备
            previousLedMillis = currentMillis;
            // 切换 LED 状态
            // 通过取反操作切换 LED 状态，实现 LED 的闪烁效果
            ledState =!ledState;
            // 根据 LED 状态控制 LED 引脚的电平
            // 将 LED 状态写入 ledPin 引脚，实现 LED 的亮灭控制
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
    // 向触发引脚发送一个 10 微秒的高电平脉冲，启动超声波传感器的测距过程
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
    // 当测量次数达到预设的上限时，计算多次测量的平均值作为最终的距离值
    // 并重置总距离和测量次数，为下一次计算平均值做准备
    if (measurementCount >= maxMeasurements) {
        distance = totalDistance / measurementCount;
        totalDistance = 0;
        measurementCount = 0;
    }
}
