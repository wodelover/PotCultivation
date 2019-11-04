#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
// 引入串口通信头文件
#include <QSerialPort>
#include <QSerialPortInfo>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleSerialData();// 处理串口数据
    void on_lightBtn_clicked(); // 改变光照状态
    void on_pumpBtn_clicked(); // 改变浇水状态
    void on_serialBtn_clicked(); // 打开串口按钮
    void on_ports_activated(const QString &arg1); // 串口号改变
    void on_bauds_activated(const QString &arg1); // 串口波特率改变
    void on_autowaterBtn_clicked(); // 自动浇水按钮

private:
    Ui::MainWindow *ui;

    bool m_lightStatus = false; // 是否打开光照变量
    bool m_pumpStatus = false; // 是否在浇水
    bool m_autoWater = false; // 是否自动浇水变量

    // [0] 定义用于进行串口通信的实例
    QSerialPort m_serialPort;

    // [1] 定义初始化界面函数
    void initFormUI();

    // [2] 定义光照状态改变
    void lightStatusChanged(bool status);
    // [3] 定义温度值改变
    void tempValueChanged(double temp);
    // [4] 定义突然湿度值改变
    void humidityValueChanged(double humidity);
    // [5] 定义浇水状态改变
    void pumpStatusChanged(bool status);
    // [6] 定义更新是否自动浇水
    void updateAutoWater();
};
#endif // MAINWINDOW_H
