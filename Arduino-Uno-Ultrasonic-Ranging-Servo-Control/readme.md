# Arduino-Uno-Ultrasonic-Ranging-Servo-Control

## 项目背景
本项目旨在学习如何使用 Arduino Uno 开发板结合超声波传感器和舵机实现测距和角度控制功能。

## 功能介绍
- 使用超声波传感器测量目标物体的距离。
- 根据测量的距离控制舵机的转动角度。

## 硬件连接
- 超声波传感器：VCC 接 5V，GND 接 GND，Trig 接 Arduino 数字引脚 9，Echo 接 Arduino 数字引脚 10。
- 舵机：VCC 接 5V，GND 接 GND，信号引脚接 Arduino 数字引脚 11。

## 代码解释
- `main.ino`：主程序文件，包含超声波测距和舵机控制的主要代码。

## 使用方法
1. 将硬件按照上述连接图连接好。
2. 打开 Arduino IDE，将 `main.ino` 文件上传到 Arduino Uno 开发板。
3. 观察超声波传感器的测量结果和舵机的转动情况。