#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 初始化串口
    serial = new QSerialPort(this);
    // 设置串口名称，根据实际情况修改
    serial->setPortName("COM4");
    // 设置波特率，要和Arduino代码中的波特率一致
    serial->setBaudRate(QSerialPort::Baud9600);
    // 设置数据位
    serial->setDataBits(QSerialPort::Data8);
    // 设置停止位
    serial->setStopBits(QSerialPort::OneStop);
    // 设置校验位
    serial->setParity(QSerialPort::NoParity);
    // 设置流控制
    serial->setFlowControl(QSerialPort::NoFlowControl);

    // 打开串口
    if (serial->open(QIODevice::ReadOnly)) {
        qDebug() << "Serial port opened successfully!";
        // 连接信号和槽，当有数据可读时调用readData函数
        connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    } else {
        qDebug() << "Failed to open serial port!";
    }
}

MainWindow::~MainWindow()
{
    // 关闭串口
    if (serial->isOpen()) {
        serial->close();
    }
    delete ui;
}

void MainWindow::readData()
{
    QByteArray data = serial->readAll();
    buffer.append(QString::fromUtf8(data));

    // 检查缓存的数据是否包含完整的温度和湿度信息
    QRegularExpression tempRegex("Temperature: ([-+]?[0-9]*\\.?[0-9]+) °C");
    QRegularExpression humiRegex("Humidity: ([-+]?[0-9]*\\.?[0-9]+) %");

    QRegularExpressionMatch tempMatch = tempRegex.match(buffer);
    QRegularExpressionMatch humiMatch = humiRegex.match(buffer);

    if (tempMatch.hasMatch() && humiMatch.hasMatch()) {
        QString tempValue = tempMatch.captured(1);
        QString humiValue = humiMatch.captured(1);

        ui->label_temp->setText("温度：" + tempValue + " °C");
        ui->label_humi->setText("湿度：" + humiValue + " %");

        // 清除已经处理过的数据
        buffer.clear();
    } else {
        qDebug() << "Failed to match temperature or humidity data. Buffer: " << buffer;
    }
}
