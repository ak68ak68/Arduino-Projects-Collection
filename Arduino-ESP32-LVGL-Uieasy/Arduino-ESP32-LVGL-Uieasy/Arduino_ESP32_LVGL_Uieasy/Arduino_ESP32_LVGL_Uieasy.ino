#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// 创建 U8g2 对象，注意类型名称的大小写
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

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

        // 在屏幕上绘制文本
        u8g2.drawStr(10, 20, "Simple UI Example");

        // 绘制一条横线
        u8g2.drawHLine(10, 30, 100);

        // 绘制一个矩形
        u8g2.drawFrame(10, 40, 50, 20);

        // 在矩形内绘制文本
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(20, 53, "Box");

    } while (u8g2.nextPage());

    // 可以根据需要添加延时
    delay(2000);
}