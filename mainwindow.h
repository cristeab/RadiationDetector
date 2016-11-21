#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTimer>

class QLabel;
class QComboBox;
class QPushButton;
class QCheckBox;
class QStatusBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void onConnectPressed(bool checked);
    void onCurrentIndexChanged(int index);
    void onHeartBeatTimer();
    void handleReadyRead();
    void handleError(QSerialPort::SerialPortError serialPortError);
private:
    void initSerialPortCombo();
    QLabel *meas_;
    QComboBox *serialPortCombo_;
    QPushButton *connectButton_;
    QSerialPort serialPort_;
    QStatusBar *statusBar_;
    QCheckBox *heartBeatCheck_;
    QTimer heartBeatTimer_;
};

#endif // MAINWINDOW_H
