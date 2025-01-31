#include <Wire.h>  // 引入Wire库，用于I2C通信
#include <math.h>  // 引入数学库，用于三角函数计算

const int MPU_addr = 0x68; // MPU6050的I2C地址
int16_t AcX, AcY, AcZ, GyX, GyY, GyZ; // 定义变量存储加速度计和陀螺仪的原始数据

// 互补滤波参数
float gyroXangle, gyroYangle, gyroZangle; // 陀螺仪计算的角度
float AccXangle, AccYangle;   // 加速度计计算的角度
float CompAngleX, CompAngleY; // 互补滤波后的角度
float gyroXrate, gyroYrate, gyroZrate;   // 陀螺仪的角速度
unsigned long preInterval;    // 上一次处理的时间

void setup() {
  Wire.begin(); // 初始化I2C总线
  Wire.beginTransmission(MPU_addr); // 开始与MPU6050通信
  Wire.write(0x6B); // 选择PWR_MGMT_1寄存器
  Wire.write(0);    // 唤醒MPU6050
  Wire.endTransmission(true); // 结束通信

  Serial.begin(9600); // 初始化串口通信，波特率为9600

  preInterval = millis(); // 记录初始化时间
}

void loop() {
  unsigned long now = millis(); // 获取当前时间
  float dt = (now - preInterval) / 1000.0; // 计算时间间隔（秒）
  preInterval = now; // 更新上一次处理的时间

  Wire.beginTransmission(MPU_addr); // 开始与MPU6050通信
  Wire.write(0x3B); // 选择加速度计数据寄存器
  Wire.endTransmission(false); // 不释放总线
  Wire.requestFrom(MPU_addr, 14, true); // 请求14个字节的数据（加速度计、陀螺仪和温度数据）

  // 读取加速度计数据
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  // 读取陀螺仪数据
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();

  // 计算加速度计的角度
  AccXangle = (atan2(AcY, AcZ) + M_PI) * 180 / M_PI;
  AccYangle = (atan2(AcZ, AcX) + M_PI) * 180 / M_PI;

  // 计算陀螺仪的角速度
  gyroXrate = GyX / 131.0;
  gyroYrate = GyY / 131.0;
  gyroZrate = GyZ / 131.0;

  // 计算陀螺仪的角度
  gyroXangle += gyroXrate * dt;
  gyroYangle += gyroYrate * dt;
  gyroZangle += gyroZrate * dt;

  // 互补滤波
  CompAngleX = 0.98 * (CompAngleX + gyroXrate * dt) + 0.02 * AccXangle;
  CompAngleY = 0.98 * (CompAngleY + gyroYrate * dt) + 0.02 * AccYangle;

  // 通过串口输出ROLL、PITCH、YAW
  Serial.print("ROLL: ");
  Serial.print(CompAngleY);
  Serial.print("  PITCH: ");
  Serial.print(CompAngleX);
  Serial.print("  YAW: ");
  Serial.println(gyroZangle);

  delay(10); // 延时10ms
}