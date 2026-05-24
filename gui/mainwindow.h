#pragma once
#include <QMainWindow>
#include <QSerialPort>
#include <QLineSeries>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void onDataReceived();
    void onConnectClicked();
private:
    QSerialPort *serial;
    QLineSeries *series;
    QByteArray buffer;
    void setupUI();
    void processBuffer();
};