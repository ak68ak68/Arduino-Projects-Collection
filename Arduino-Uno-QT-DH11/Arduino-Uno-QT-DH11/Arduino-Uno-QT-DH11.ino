#include <DHT.h>

// 定义 DHT 传感器连接的引脚
#define DHTPIN 2
// 定义使用的 DHT 传感器类型为 DHT11
#define DHTTYPE DHT11

// 创建 DHT 对象，用于与传感器进行交互
DHT dht(DHTPIN, DHTTYPE);

// setup() 函数在程序开始时执行一次，用于初始化设置
void setup() {
  // 初始化串口通信，波特率设置为 9600bps
  Serial.begin(9600);
  // 初始化 DHT 传感器
  dht.begin();
}

// loop() 函数会不断循环执行，用于持续读取传感器数据
void loop() {
  // 延迟 2000 毫秒（即 2 秒），避免过于频繁地读取数据
  delay(2000);

  // 读取当前的湿度值
  float h = dht.readHumidity();
  // 读取当前的温度值
  float t = dht.readTemperature();

  // 检查读取的数据是否有效，如果为 NaN 表示读取失败
  if (isnan(h) || isnan(t)) {
    // 输出错误信息到串口
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // 输出温度信息到串口
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" C");

  // 输出湿度信息到串口
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %");
}