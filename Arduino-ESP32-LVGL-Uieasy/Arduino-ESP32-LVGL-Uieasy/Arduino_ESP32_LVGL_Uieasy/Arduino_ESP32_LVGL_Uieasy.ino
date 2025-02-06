#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// 创建 U8g2 对象，适用于 SSD1306 128x64 I2C 显示屏
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

// 定义表盘中心位置
const int centerX = 64;
const int centerY = 32;
// 定义表盘半径
const int radius = 30;
// 定义时针和分针长度
const int hourHandLength = 20;
const int minuteHandLength = 25;

// 计算指针端点坐标的函数
void calculatePosition(int angle, int length, int& x, int& y) {
    float rad = radians(angle - 90);  // 将角度转换为弧度，并调整 0 度位置
    x = centerX + length * cos(rad);
    y = centerY + length * sin(rad);
}


void setup() {
    // 初始化 U8g2
    u8g2.begin();
}

void loop() {
    // 清除屏幕
    u8g2.firstPage();
    do {
        // 绘制表盘外圈
        u8g2.drawCircle(centerX, centerY, radius);

        // 绘制刻度
        for (int i = 0; i < 60; i++) {
            int angle = i * 6;
            int x1, y1, x2, y2;
            if (i % 5 == 0) {
                // 整点刻度长一些
                calculatePosition(angle, radius - 3, x1, y1);
                calculatePosition(angle, radius, x2, y2);
            } else {
                // 普通刻度短一些
                calculatePosition(angle, radius - 1, x1, y1);
                calculatePosition(angle, radius, x2, y2);
            }
            u8g2.drawLine(x1, y1, x2, y2);
        }

        // 绘制 1 - 12 数字
        u8g2.setFont(u8g2_font_6x10_tr);  // 设置为支持的字体
        for (int i = 1; i <= 12; i++) {
            int angle = i * 30;  // 每个数字间隔 30 度
            int x, y;
            calculatePosition(angle, radius - 8, x, y);  // 数字位置稍微靠内一点
            char numStr[3];
            sprintf(numStr, "%d", i);
            int strWidth = u8g2.getStrWidth(numStr);
            u8g2.drawStr(x - strWidth / 2, y + 3, numStr);  // 居中绘制数字
        }

        // 计算 12:30 时时针和分针的角度
        int hour = 12;
        int minute = 30;
        int hourAngle = (hour % 12) * 30 + minute * 0.5; // 每小时 30 度，分针影响时针微小偏移
        int minuteAngle = minute * 6; // 每分钟 6 度

        // 计算时针和分针端点坐标
        int hourX, hourY, minuteX, minuteY;
        calculatePosition(hourAngle, hourHandLength, hourX, hourY);
        calculatePosition(minuteAngle, minuteHandLength, minuteX, minuteY);

        // 绘制时针和分针
        u8g2.drawLine(centerX, centerY, hourX, hourY);
        u8g2.drawLine(centerX, centerY, minuteX, minuteY);

        // 绘制中心圆点
        u8g2.drawPixel(centerX, centerY);

    } while (u8g2.nextPage());

    // 可以根据需要添加延时
    delay(1000);
}
