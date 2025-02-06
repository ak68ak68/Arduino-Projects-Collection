#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// 创建 U8g2 对象，适用于 SSD1306 128x64 I2C 显示屏
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

// 定义一些全局变量用于动态更新
int counter = 0;

void setup() {
  // 初始化 U8g2
  u8g2.begin();
}

void loop() {
  // 清除屏幕
  u8g2.firstPage();
  do {
    // 设置字体
    u8g2.setFont(u8g2_font_ncenB08_tr);

    // 绘制标题
    u8g2.drawStr(10, 15, "Complex UI Example");

    // 绘制一条分隔线
    u8g2.drawHLine(10, 20, 108);

    // 绘制第一个区域：文本和矩形
    u8g2.drawStr(10, 35, "Section 1");
    u8g2.drawFrame(10, 40, 50, 20);
    // 使用支持的字体
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(20, 53, "Info");

    // 绘制第二个区域：圆形和动态文本
    u8g2.drawCircle(90, 50, 10);
    u8g2.setCursor(80, 35);
    u8g2.print("Counter: ");
    u8g2.print(counter);

    // 绘制底部文本
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(10, 62, "Bottom Text");

  } while (u8g2.nextPage());

  // 动态更新计数器
  counter++;
  if (counter > 999) {
    counter = 0;
  }

  // 控制更新频率
  delay(1000);
}