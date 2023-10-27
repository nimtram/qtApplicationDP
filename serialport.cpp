#include "serialport.h"
#include <QRegularExpression>

serialport::serialport(QObject* parent) : QSerialPort(parent)
{
}


void serialport::threadStarted()
{
    //qDebug()<< "thread started";
    connect(this, SIGNAL(readyRead()), this, SLOT(slot_read_uart()));
    connect(this, SIGNAL(parse_message(QByteArray)),this,SLOT(parse_message_from_serialport(QByteArray)));
}

void serialport::threadStopped()
{
    close();
}


void serialport::slot_read_uart()
{
    while (canReadLine()) {
        QByteArray msg = readLine();
        qDebug() << msg;
        emit parse_message(msg);
    }


}

void serialport::parse_message_from_serialport(QByteArray byteArray){
    qDebug()<< "Parse it!!";
    QString receivedData(byteArray);
    // Define a regular expression pattern to match numeric values
    QRegularExpression pattern("\\d+");

    // Use QRegularExpressionMatchIterator to extract all numeric values from the message
    QRegularExpressionMatchIterator matches = pattern.globalMatch(receivedData);

    double values[3];
    int i = 0;

    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        if (i < 3) {
            values[i] = match.captured(0).toDouble();
            i++;
        } else {
            break;
        }
    }

    if (i >= 3) {
        emit sig_plot_new_values_x(values[0]);
        emit sig_plot_new_values_y(values[1]);
        emit sig_plot_new_values_z(values[2]);
    } else {
        qDebug() << "Not enough values found in the message.";
    }




//    if(typeOfData == 'x'){
//        emit sig_plot_new_values_x(xValue);
//    }
//    else if(typeOfData == 'y'){

//        emit sig_plot_new_values_y(valuesToPlot);
//    }
//    else if(typeOfData == 'z'){
//        emit sig_plot_new_values_z(valuesToPlot);
//    }
//    else{
//        qDebug() << "something went wrong, no letter was sent first";
//    }



}


void serialport::connectToSerialPort(QString portName){

    setPortName(portName);
    setBaudRate(3000000);
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
        emit sig_serialPortConnected();

    }
    readAll();
}

void serialport::disconnectFromSerialPort(){
    emit sig_serialPortDisconnected();
    close();

}
