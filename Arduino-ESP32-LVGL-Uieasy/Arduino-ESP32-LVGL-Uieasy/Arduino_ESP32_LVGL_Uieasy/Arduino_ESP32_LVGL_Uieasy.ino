#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// 使用 I2C 接口初始化 U8g2 对象，适用于 128x64 的 SSD1306 OLED 屏幕
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

// 绘制心形函数
void drawHeart(u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t size) {
  for (float t = 0; t <= 2 * PI; t += 0.01) {
    // 心形的参数方程
    float xCoord = 16 * pow(sin(t), 3);
    float yCoord = 13 * cos(t) - 5 * cos(2 * t) - 2 * cos(3 * t) - cos(4 * t);

    // 调整坐标和比例
    int drawX = x + static_cast<int>(xCoord * size / 20);
    int drawY = y - static_cast<int>(yCoord * size / 20);

    // 绘制点
    u8g2.drawPixel(drawX, drawY);
  }
}

void setup() {
  // 初始化 U8g2 对象
  u8g2.begin();
  // 设置对比度，值范围为 0 - 255
  u8g2.setContrast(255);
}

void loop() {
  // 清空屏幕
  u8g2.firstPage();
  do {
    // 设置字体
    u8g2.setFont(u8g2_font_ncenB08_tr);

    // 绘制一个心形，位置 (32, 10)，大小为 20
    drawHeart(32, 10, 20);

  } while (u8g2.nextPage());

  // 延迟一段时间，可根据需求调整
  delay(1000);
}