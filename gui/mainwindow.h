#pragma once

#include <QMainWindow>
#include <QSerialPort>
#include <QTimer>
#include <QVector>

class QCustomPlot;

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
    QTimer *timer;
    QVector<double> xData, yData;
    QByteArray buffer;

    void setupUI();
    void processBuffer();
};