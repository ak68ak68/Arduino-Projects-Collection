#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE); 

void setup() {
  Serial.begin(9600);
  Serial.println("Starting setup...");
  if (!u8g2.begin()) {
    Serial.println("U8g2 initialization failed!");
  } else {
    Serial.println("U8g2 initialized successfully.");
    // 设置对比度，值范围为 0 - 255
    u8g2.setContrast(255); 
  }
}

void loop() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawCircle(64, 32, 20, U8G2_DRAW_ALL);
    u8g2.drawFrame(20, 10, 30, 30);
  } while ( u8g2.nextPage() );

  delay(1000);
}