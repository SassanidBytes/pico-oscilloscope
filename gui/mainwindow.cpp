#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QWidget>
#include <QtCharts>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    serial = new QSerialPort(this);
    setupUI();
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::onDataReceived);
}

MainWindow::~MainWindow() {
    if (serial->isOpen()) serial->close();
}

void MainWindow::setupUI() {
    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);

    // Top bar
    QHBoxLayout *topBar = new QHBoxLayout();
    QLabel *portLabel = new QLabel("Port: COM8");
    QPushButton *connectBtn = new QPushButton("Connect");
    topBar->addWidget(portLabel);
    topBar->addWidget(connectBtn);
    topBar->addStretch();

    // Chart
    QChart *chart = new QChart();
    chart->setTitle("Pico W Oscilloscope");
    chart->legend()->hide();

    QLineSeries *series = new QLineSeries();
    chart->addSeries(series);

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

    // Store series for later
    this->setProperty("series", QVariant::fromValue(series));
}

void MainWindow::onConnectClicked() {
    if (!serial->isOpen()) {
        serial->setPortName("COM8");
        serial->setBaudRate(QSerialPort::Baud115200);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        if (serial->open(QIODevice::ReadOnly)) {
            qDebug() << "Connected to COM8";
        }
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
        if (data[0] != START) {
            buffer.remove(0, 1);
            continue;
        }
        if (data[1025] != END) {
            buffer.remove(0, 1);
            continue;
        }

        // Valid frame
        QLineSeries *series = this->property("series").value<QLineSeries*>();
        series->clear();
        for (int i = 0; i < 1024; i++) {
            double voltage = data[i+1] * 3.3 / 4095.0;
            series->append(i, voltage);
        }

        buffer.remove(0, FRAME_SIZE);
    }
}