#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QSerialPortInfo>
#include <QStatusBar>
#include <QCheckBox>
#include <QMessageBox>
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(100, 150);
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QGridLayout *gridLayout = new QGridLayout(centralWidget);
    centralWidget->setLayout(gridLayout);
    QLabel *measLabel = new QLabel(tr("Measurement"), centralWidget);
    gridLayout->addWidget(measLabel, 1, 0);
    meas_ = new QLabel(tr("N/A"), centralWidget);
    gridLayout->addWidget(meas_, 1, 1, Qt::AlignHCenter);

    QLabel *serialPortLabel = new QLabel(tr("Serial Port"), centralWidget);
    gridLayout->addWidget(serialPortLabel, 0, 0);

    serialPortCombo_ = new QComboBox(centralWidget);
    initSerialPortCombo();
    connect(serialPortCombo_, SIGNAL(currentIndexChanged(int)), this,
            SLOT(onCurrentIndexChanged(int)));
    gridLayout->addWidget(serialPortCombo_, 0, 1);

    connectButton_ = new QPushButton(tr("Connect"), centralWidget);
    connectButton_->setCheckable(true);
    connectButton_->setChecked(false);
    connect(connectButton_, &QPushButton::clicked, this, &MainWindow::onConnectPressed);
    gridLayout->addWidget(connectButton_, 2, 0, 1, 2);

    serialPort_.setBaudRate(QSerialPort::Baud9600);
    connect(&serialPort_, &QSerialPort::readyRead, this, &MainWindow::handleReadyRead);
    connect(&serialPort_, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &MainWindow::handleError);

    statusBar_ = new QStatusBar(this);
    heartBeatCheck_ = new QCheckBox(tr("Heartbeat"), statusBar_);
    heartBeatCheck_->setEnabled(false);
    statusBar_->addWidget(heartBeatCheck_);
    setStatusBar(statusBar_);

    heartBeatTimer_.setInterval(1000);
    heartBeatTimer_.setSingleShot(true);
    connect(&heartBeatTimer_, &QTimer::timeout, this, &MainWindow::onHeartBeatTimer);
}

void MainWindow::initSerialPortCombo()
{
    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    if (!list.isEmpty()) {
        for (int n = 0; n < list.size(); ++n) {
            serialPortCombo_->addItem(list.at(n).portName());
        }
        serialPortCombo_->setCurrentIndex(0);
    } else {
        serialPortCombo_->setEnabled(false);
    }
}

MainWindow::~MainWindow()
{

}

void MainWindow::onConnectPressed(bool checked)
{
    if (checked) {
        // start serial port connection
        if (serialPort_.open(QIODevice::ReadOnly)) {
            connectButton_->setText(tr("Disconnect"));
            serialPortCombo_->setEnabled(false);
        } else {
            QMessageBox::critical(this, tr("Radiation Detector"),
                                  tr("Failed to open port: %1").arg(serialPort_.errorString()));
            connectButton_->setChecked(false);
        }
    } else {
        serialPort_.close();
        connectButton_->setText(tr("Connect"));
        serialPortCombo_->setEnabled(true);
    }
}

void MainWindow::onCurrentIndexChanged(int index)
{
    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    if ((index >= 0) && (index < list.size())) {
        serialPort_.setPortName(list.at(index).portName());
    } else {
        QMessageBox::warning(this, tr("Radiation Detector"), tr("Selected serial port is invalid"));
        serialPortCombo_->clear();
        initSerialPortCombo();
    }
}

void MainWindow::handleReadyRead()
{
    meas_->setText(serialPort_.readAll());
    heartBeatCheck_->setChecked(true);
    heartBeatTimer_.start();
}

void MainWindow::handleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::ReadError) {
        statusBar_->showMessage(serialPort_.errorString());
    }
}

void MainWindow::onHeartBeatTimer()
{
    heartBeatCheck_->setChecked(false);
}
