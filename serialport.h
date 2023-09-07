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

signals:
    void parse_message(QByteArray);
    void plot_new_values_x(QVector<double>);
    void plot_new_values_y(QVector<double>);
    void plot_new_values_z(QVector<double>);

public slots:
    void threadStarted();
    void threadStopped();
    void slot_read_uart();
    void parse_message_from_serialport(QByteArray);
    void connectToSerialPort(QString, QString);


};

#endif // SERIALPORT_H
