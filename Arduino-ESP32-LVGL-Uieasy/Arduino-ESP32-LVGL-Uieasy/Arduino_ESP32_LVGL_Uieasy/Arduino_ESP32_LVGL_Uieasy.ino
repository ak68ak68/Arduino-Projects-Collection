#include <U8g2lib.h>

// 修正类型名称的大小写
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 22, /* data=*/ 21, /* reset=*/ U8X8_PIN_NONE);

void setup() {
  // 初始化U8g2库
  u8g2.begin();
}

void loop() {
  // 清空屏幕
  u8g2.firstPage();
  do {
    // 设置绘图颜色为白色
    u8g2.setDrawColor(1);
    // 绘制圆形，参数依次为圆心x坐标、圆心y坐标、半径、绘制模式（U8G2_DRAW_ALL表示绘制整个圆形）
    u8g2.drawCircle(64, 32, 20, U8G2_DRAW_ALL);
  } while (u8g2.nextPage());

  // 可以根据需要添加延迟
  delay(1000);
}