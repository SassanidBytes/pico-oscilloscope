#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include <QLineSeries>
#include <QChart>
#include <QChartView>
#include <QValueAxis>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    serial = new QSerialPort(this);
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::onDataReceived);
    setupUI();
}

MainWindow::~MainWindow() {
    if (serial->isOpen()) serial->close();
}

void MainWindow::setupUI() {
    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);

    QHBoxLayout *topBar = new QHBoxLayout();
    QPushButton *connectBtn = new QPushButton("Connect COM8");
    topBar->addWidget(connectBtn);
    topBar->addStretch();

    series = new QLineSeries();

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Pico W Oscilloscope");
    chart->legend()->hide();

    QValueAxis *axisX = new QValueAxis();
    axisX->setRange(0, 1024);
    axisX->setTitleText("Sample");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 3.3);
    axisY->setTitleText("Voltage (V)");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    layout->addLayout(topBar);
    layout->addWidget(chartView);
    setCentralWidget(central);
    setWindowTitle("Pico Oscilloscope");
    resize(900, 600);

    connect(connectBtn, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
}

void MainWindow::onConnectClicked() {
    if (!serial->isOpen()) {
        serial->setPortName("COM8");
        serial->setBaudRate(QSerialPort::Baud115200);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->open(QIODevice::ReadOnly);
    }
}

void MainWindow::onDataReceived() {
    buffer.append(serial->readAll());
    processBuffer();
}

void MainWindow::processBuffer() {
    const uint16_t START = 0xABCD;
    const uint16_t END   = 0xDCBA;
    const int FRAME_SIZE = (1 + 1024 + 1) * 2;

    while (buffer.size() >= FRAME_SIZE) {
        uint16_t *data = reinterpret_cast<uint16_t*>(buffer.data());
        if (data[0] != START) { buffer.remove(0, 1); continue; }
        if (data[1025] != END) { buffer.remove(0, 1); continue; }

        QVector<QPointF> points;
        points.reserve(1024);
        for (int i = 0; i < 1024; i++) {
            points.append(QPointF(i, data[i+1] * 3.3 / 4095.0));
        }
        series->replace(points);
        buffer.remove(0, FRAME_SIZE);
    }
}