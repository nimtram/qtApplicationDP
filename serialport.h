#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QSerialPort>
#include <QObject>
#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QVector>
#include <QtMath>
#include <QFileDialog>
#include <QTimer>

class serialport : public QSerialPort
{
    Q_OBJECT
public:
    serialport(QObject *parent = nullptr);
private:
    QByteArray uartBuffer;
signals:
    void parse_message(QByteArray);
    void sig_plot_new_values_x(double);
    void sig_plot_new_values_y(double);
    void sig_plot_new_values_z(double);

public slots:
    void threadStarted();
    void threadStopped();
    void slot_read_uart();
    void parse_message_from_serialport(QByteArray);
    void connectToSerialPort(QString);
    void disconnectFromSerialPort();
    void testIt();

};

#endif // SERIALPORT_H
