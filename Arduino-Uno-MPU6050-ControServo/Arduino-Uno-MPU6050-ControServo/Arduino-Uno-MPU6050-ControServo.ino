#include <Wire.h>  // 引入 I2C 通信库
#include <MPU6050_tockn.h>  // 引入 MPU6050_tockn 库
#include <Servo.h>  // 引入舵机控制库

// 创建 MPU6050 对象，传入 Wire 对象
MPU6050 mpu(Wire);

// 创建舵机对象
Servo servo;

// 定义舵机控制引脚
const int servoPin = 9;

// 定义变量用于存储传感器数据和计算的角度
float angle;  // 计算得到的倾斜角度
int servoAngle;  // 映射到舵机控制范围的角度

void setup() {
  // 初始化串口通信，波特率设置为 9600，用于调试信息输出
  Serial.begin(9600);

  // 初始化 I2C 总线
  Wire.begin();

  // 初始化 MPU6050 传感器
  mpu.begin();

  // 将舵机连接到指定的引脚
  servo.attach(servoPin);
}

void loop() {
  // 更新 MPU6050 的数据
  mpu.update();

  // 根据加速度计的 y 轴和 z 轴数据计算倾斜角度
  angle = atan2(mpu.getAccY(), mpu.getAccZ()) * 180 / PI;

  // 将计算得到的倾斜角度映射到舵机的控制范围（0° - 180°）
  servoAngle = map(angle, -90, 90, 0, 180);

  // 对舵机角度进行限制，确保其在 0° 到 180° 之间
  if (servoAngle < 0) {
    servoAngle = 0;
  } else if (servoAngle > 180) {
    servoAngle = 180;
  }

  // 控制舵机转动到指定的角度
  servo.write(servoAngle);

  // 通过串口输出当前的倾斜角度和舵机角度，方便调试
  Serial.print("倾斜角度: ");
  Serial.print(angle);
  Serial.print("°  舵机角度: ");
  Serial.print(servoAngle);
  Serial.println("°");

  // 延时 100 毫秒，避免过于频繁地读取数据和控制舵机
  delay(100);
}