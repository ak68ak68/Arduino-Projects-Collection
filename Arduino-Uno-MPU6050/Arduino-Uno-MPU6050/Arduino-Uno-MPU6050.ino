#include <Wire.h>  // 引入Wire库，用于I2C通信，MPU6050通过I2C协议与Arduino通信
#include <math.h>  // 引入数学库，提供三角函数等数学运算功能，用于姿态角度计算

const int MPU_addr = 0x68; // MPU6050的I2C地址，用于在I2C总线上识别该传感器

// 定义变量存储加速度计和陀螺仪的原始数据
int16_t AcX, AcY, AcZ, GyX, GyY, GyZ; 

// 定义零偏补偿变量，用于存储陀螺仪的零偏值
float gyroXoffset, gyroYoffset, gyroZoffset; 

// 互补滤波参数
float gyroXangle, gyroYangle, gyroZangle; // 陀螺仪计算的角度
float AccXangle, AccYangle;   // 加速度计计算的角度
float CompAngleX, CompAngleY; // 互补滤波后的角度
float gyroXrate, gyroYrate, gyroZrate;   // 陀螺仪的角速度
unsigned long preInterval;    // 上一次处理的时间

void setup() {
  Wire.begin(); // 初始化I2C总线，让Arduino能够与I2C设备进行通信
  Wire.beginTransmission(MPU_addr); // 开始与MPU6050进行通信
  Wire.write(0x6B); // 选择PWR_MGMT_1寄存器，该寄存器用于控制MPU6050的电源模式
  Wire.write(0);    // 将PWR_MGMT_1寄存器的值设为0，唤醒MPU6050，使其从睡眠模式进入工作模式
  Wire.endTransmission(true); // 结束与MPU6050的通信，并释放I2C总线

  Serial.begin(9600); // 初始化串口通信，设置波特率为9600，用于将数据传输到计算机的串口监视器

  // 陀螺仪零偏校准
  int calibrationSamples = 1000; // 采样次数，采样次数越多，零偏计算越准确
  float gyroXsum = 0, gyroYsum = 0, gyroZsum = 0;
  for (int i = 0; i < calibrationSamples; i++) {
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x43); // 选择陀螺仪数据起始寄存器
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr, 6, true); // 请求6个字节的陀螺仪数据
    int16_t tempGyX = Wire.read() << 8 | Wire.read(); // 读取X轴陀螺仪数据
    int16_t tempGyY = Wire.read() << 8 | Wire.read(); // 读取Y轴陀螺仪数据
    int16_t tempGyZ = Wire.read() << 8 | Wire.read(); // 读取Z轴陀螺仪数据
    gyroXsum += tempGyX; // 累加X轴陀螺仪数据
    gyroYsum += tempGyY; // 累加Y轴陀螺仪数据
    gyroZsum += tempGyZ; // 累加Z轴陀螺仪数据
    delay(1); // 延时1ms，避免数据读取过于频繁
  }
  gyroXoffset = gyroXsum / calibrationSamples; // 计算X轴陀螺仪零偏
  gyroYoffset = gyroYsum / calibrationSamples; // 计算Y轴陀螺仪零偏
  gyroZoffset = gyroZsum / calibrationSamples; // 计算Z轴陀螺仪零偏

  preInterval = millis(); // 记录初始化时间，用于后续计算时间间隔
}

void loop() {
  unsigned long now = millis(); // 获取当前时间（毫秒）
  float dt = (now - preInterval) / 1000.0; // 计算从上次处理到现在的时间间隔（秒）
  preInterval = now; // 更新上一次处理的时间为当前时间

  Wire.beginTransmission(MPU_addr); // 开始与MPU6050进行通信
  Wire.write(0x3B); // 选择加速度计数据起始寄存器
  Wire.endTransmission(false); // 结束本次通信，但不释放I2C总线，以便继续请求数据
  Wire.requestFrom(MPU_addr, 14, true); // 请求14个字节的数据，包含加速度计、陀螺仪和温度数据

  // 读取加速度计数据
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();

  // 读取陀螺仪数据
  int16_t rawGyX = Wire.read() << 8 | Wire.read();
  int16_t rawGyY = Wire.read() << 8 | Wire.read();
  int16_t rawGyZ = Wire.read() << 8 | Wire.read();

  // 减去零偏补偿值
  GyX = rawGyX - static_cast<int16_t>(gyroXoffset);
  GyY = rawGyY - static_cast<int16_t>(gyroYoffset);
  GyZ = rawGyZ - static_cast<int16_t>(gyroZoffset);

  // 计算加速度计的角度
  AccXangle = (atan2(AcY, AcZ) + M_PI) * 180 / M_PI; // 计算俯仰角（Pitch）
  AccYangle = (atan2(AcZ, AcX) + M_PI) * 180 / M_PI; // 计算横滚角（Roll）

  // 计算陀螺仪的角速度
  gyroXrate = GyX / 131.0; // 将原始数据转换为实际角速度（度/秒）
  gyroYrate = GyY / 131.0;
  gyroZrate = GyZ / 131.0;

  // 计算陀螺仪的角度
  gyroXangle += gyroXrate * dt; // 通过积分计算X轴角度变化
  gyroYangle += gyroYrate * dt; // 通过积分计算Y轴角度变化
  gyroZangle += gyroZrate * dt; // 通过积分计算Z轴角度变化

  // 互补滤波
  CompAngleX = 0.98 * (CompAngleX + gyroXrate * dt) + 0.02 * AccXangle; // 融合加速度计和陀螺仪数据得到更准确的Pitch角
  CompAngleY = 0.98 * (CompAngleY + gyroYrate * dt) + 0.02 * AccYangle; // 融合加速度计和陀螺仪数据得到更准确的Roll角

  // 通过串口输出ROLL、PITCH、YAW
  Serial.print("ROLL: ");
  Serial.print(CompAngleY);
  Serial.print("  PITCH: ");
  Serial.print(CompAngleX);
  Serial.print("  YAW: ");
  Serial.println(gyroZangle);

  delay(10); // 延时10ms，避免数据处理过于频繁
}