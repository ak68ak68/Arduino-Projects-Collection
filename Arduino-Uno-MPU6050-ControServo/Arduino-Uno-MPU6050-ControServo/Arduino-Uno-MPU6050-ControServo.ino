#include <Wire.h>  // 引入 I2C 通信库，用于与 MPU6050 进行通信
#include <MPU6050_tockn.h>  // 引入 MPU6050_tockn 库来操作 MPU6050 传感器
#include <Servo.h>  // 引入舵机控制库

// 创建 MPU6050 对象，传入 Wire 对象以进行 I2C 通信
MPU6050 mpu(Wire);

// 创建舵机对象
Servo servo;

// 定义舵机控制引脚
const int servoPin = 9;

// 定义变量用于存储传感器数据和计算的角度
float tiltAngle;  // 存储从 MPU6050 读取的倾斜角度
int servoTargetAngle;  // 存储映射后要控制舵机转动的角度

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
  tiltAngle = atan2(mpu.getAccY(), mpu.getAccZ()) * 180 / PI;

  // 定义倾斜角度的输入范围和舵机角度的输出范围
  int inputMin = -90;  // 倾斜角度的最小值
  int inputMax = 90;   // 倾斜角度的最大值
  int outputMin = 0;   // 舵机角度的最小值
  int outputMax = 180; // 舵机角度的最大值

  // 将倾斜角度按比例映射到舵机角度范围
  servoTargetAngle = map((int)tiltAngle, inputMin, inputMax, outputMin, outputMax);

  // 对舵机角度进行限制，确保其在 0° 到 180° 之间
  if (servoTargetAngle < 0) {
    servoTargetAngle = 0;
  } else if (servoTargetAngle > 180) {
    servoTargetAngle = 180;
  }

  // 控制舵机转动到指定的角度
  servo.write(servoTargetAngle);

  // 通过串口输出当前的倾斜角度和舵机角度，方便调试
  Serial.print("倾斜角度: ");
  Serial.print(tiltAngle);
  Serial.print("°  舵机角度: ");
  Serial.print(servoTargetAngle);
  Serial.println("°");

  // 延时 100 毫秒，避免过于频繁地读取数据和控制舵机
  delay(100);
}