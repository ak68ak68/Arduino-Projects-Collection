// 引入DHT传感器库，该库用于与DHT系列温湿度传感器进行交互
#include <DHT.h>

// 定义DHT传感器连接到Arduino的引脚编号，这里连接到数字引脚2
#define DHTPIN 2
// 定义使用的DHT传感器类型，这里使用的是DHT11传感器
#define DHTTYPE DHT11

// 创建一个DHT对象，名为dht，用于控制和读取DHT传感器的数据
// 构造函数的参数分别为传感器连接的引脚和传感器类型
DHT dht(DHTPIN, DHTTYPE);

// setup()函数是Arduino程序的初始化函数，在程序开始运行时只执行一次
void setup() {
  // 初始化串口通信，设置波特率为9600bps
  // 波特率决定了数据传输的速度，这里的9600bps是常用的通信速率
  Serial.begin(9600);
  // 初始化DHT传感器，为后续读取数据做准备
  dht.begin();
}

// loop()函数是Arduino程序的主循环函数，在setup()函数执行完毕后会不断循环执行
void loop() {
  // 延迟2000毫秒（即2秒），避免过于频繁地读取传感器数据
  // 因为DHT传感器的读取间隔需要一定时间，过短的间隔可能导致数据不准确
  delay(2000);

  // 调用DHT对象的readHumidity()方法读取当前的湿度值，并将结果存储在浮点型变量h中
  float h = dht.readHumidity();
  // 调用DHT对象的readTemperature()方法读取当前的温度值，并将结果存储在浮点型变量t中
  float t = dht.readTemperature();

  // isnan()函数用于判断一个数是否为非数字（NaN）
  // 如果读取的湿度值或温度值为NaN，说明读取传感器数据失败
  if (isnan(h) || isnan(t)) {
    // 通过串口输出错误信息，提示用户读取传感器数据失败
    Serial.println("Failed to read from DHT sensor!");
    // 直接返回，跳过后续的温度输出操作，等待下一次循环重新读取数据
    return;
  }

  // 通过串口输出提示信息“Temperature: ”，用于标识接下来输出的是温度值
  Serial.print("Temperature: ");
  // 通过串口输出实际的温度值
  Serial.print(t);
  // 通过串口输出温度单位“ °C”，并换行，方便在串口监视器中查看数据
  Serial.println(" °C");
}