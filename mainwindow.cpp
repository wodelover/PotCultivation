#include "mainwindow.h"
#include "ui_mainwindow.h"

// 引入解析json文件的头文件
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QList>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // [0] 调用初始化函数设置当前状态
    initFormUI();

    // [1] 获取当前可以的串口
    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    for(int i = 0; i < list.size(); i++){
        ui->ports->addItem(list.at(i).portName());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleSerialData()
{
    // [0] 下位机发送的数据包格式如下
    // temp:温度 humidity:湿度 L:光照状态 P:浇水状态
    // {"temp":11,"humidity":45,"L":1,"P":1}

    // [1] 定义数据缓冲区并读取数据
    static QByteArray cache = "";
    cache = m_serialPort.readAll();

    // [2] 判断数据是否接收正确
    if(cache.startsWith('{')&&cache.endsWith('}')){
        // (0) 将缓冲区数据转换为JSon对象获取里面的值
        QJsonDocument doc = QJsonDocument::fromJson(cache);
        QJsonObject obj = doc.object();

        // (1) 分别获取里面的值
        double temp = obj.value("temp").toDouble();
        double humidity = obj.value("humidity").toDouble();
        bool lightStatus = obj.value("L").toInt();
        bool pumpStatus = obj.value("P").toInt();

        // (2) 更新对应的值
        tempValueChanged(temp);
        humidityValueChanged(humidity);
        lightStatusChanged(lightStatus);
        pumpStatusChanged(pumpStatus);
    }
}

void MainWindow::initFormUI()
{
    // [0] 初始化默认状态
    lightStatusChanged(false);
    tempValueChanged(0);
    humidityValueChanged(0);
    pumpStatusChanged(false);

    // [1] 初始化信号连接
    connect(&m_serialPort,&QSerialPort::readyRead,this,&MainWindow::handleSerialData);
}

void MainWindow::lightStatusChanged(bool status)
{
    m_lightStatus = status;
    if(m_lightStatus){
        ui->light_img->setEnabled(true);
        ui->light_text->setText(tr("已打开"));
    }else{
        ui->light_img->setEnabled(false);
        ui->light_text->setText(tr("已关闭"));
    }
}

void MainWindow::tempValueChanged(double temp)
{
    ui->temp_text->setText(QString::number(temp) + "℃");
}

void MainWindow::humidityValueChanged(double humidity)
{
    ui->humidity_text->setText(QString::number(humidity) + "%");
}

void MainWindow::pumpStatusChanged(bool status)
{
    m_pumpStatus = status;
    if(m_pumpStatus){
        ui->pump_img->setEnabled(true);
        ui->pump_text->setText(tr("浇水中"));
    }else{
        ui->pump_img->setEnabled(false);
        ui->pump_text->setText(tr("未浇水"));
    }
}

void MainWindow::on_lightBtn_clicked()
{
    m_lightStatus = !m_lightStatus;
    if(m_lightStatus){
        m_serialPort.write("{'L':1}");
    }else{
        m_serialPort.write("{'L':0}");
    }
}

void MainWindow::on_pumpBtn_clicked()
{
    // (0) 打开浇水
    m_serialPort.write("{'P':1}");

    // (1) 定时间隔完成后,关闭浇水
    QTimer::singleShot(ui->pump_time->text().toInt()*1000,this,[=](){
     m_serialPort.write("{'P':0}");
    });
}

void MainWindow::on_serialBtn_clicked()
{
    if(m_serialPort.isOpen()){
        m_serialPort.close();
    }
    m_serialPort.open(QIODevice::ReadWrite);

    if(m_serialPort.isOpen()){
        ui->serialBtn->setText(tr("已打开"));
    }else{
        ui->serialBtn->setText(tr("打开串口"));
    }
}

void MainWindow::on_ports_activated(const QString &arg1)
{
    m_serialPort.setPortName(arg1);
    on_serialBtn_clicked();
}

void MainWindow::on_bauds_activated(const QString &arg1)
{
    m_serialPort.setBaudRate(arg1.toInt());
    on_serialBtn_clicked();
}
