#include "serialport.h"


serialport::serialport(QObject* parent) : QSerialPort(parent)
{
}


void serialport::threadStarted()
{
    qDebug()<< "thread started";

    connect(this, SIGNAL(readyRead()), this, SLOT(slot_read_uart()));
    connect(this, SIGNAL(parse_message(QByteArray)),this,SLOT(parse_message_from_serialport(QByteArray)));

}

void serialport::threadStopped()
{
    close();
}


void serialport::slot_read_uart()
{
    QByteArray msg = readAll();
    emit parse_message(msg);
}

void serialport::parse_message_from_serialport(QByteArray){
    qDebug()<< "Parse it!!";
    QVector<double> values_to_plot;
    emit plot_new_values_x(values_to_plot);
}


void serialport::connectToSerialPort(QString portName, QString baudRate){

    setPortName(portName);
    setBaudRate(baudRate.toUInt());
    setDataBits(QSerialPort::Data8);
    setParity(QSerialPort::NoParity);
    setStopBits(QSerialPort::OneStop);
    setFlowControl(QSerialPort::NoFlowControl);
    //setReadBufferSize(1000);

    bool status=open(QSerialPort::ReadWrite);

    if(status==false)
    {
        qDebug()<<"Unable to open com port";
    }
    else
    {
        qDebug() << "Connected!";
    }
    readAll();
}

