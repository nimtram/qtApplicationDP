#ifndef CUSTOMPLOTPROJECT_H
#define CUSTOMPLOTPROJECT_H

#include <QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QString>
#include <QSerialPort>
#include <QObject>
#include <QSerialPortInfo>
#include <QQueue>
#include <serialport.h>

QT_BEGIN_NAMESPACE
namespace Ui { class customPlotProject; }
QT_END_NAMESPACE

class customPlotProject : public QMainWindow
{
    Q_OBJECT
    QThread worker_thread;

public:
    customPlotProject(QWidget *parent = nullptr);
    ~customPlotProject();

private slots:
    void readSerialPort();
    void buttonConnectPressed();

private:
    serialport *worker = new serialport;
    QString portName;
    QString baudRate;
    QSerialPortInfo info;
    Ui::customPlotProject *ui;
    QSerialPort* serialComPort;
    QSerialPort* COMPORT_2;
    QString buffer;

    QString code;
    double plotIndex;
    bool conversionSuccess;
    QVector <double> x_plotValues = {};
    QVector <double> y_plotValues = {};
    int codeSize;

public:
    QQueue<QByteArray> queue;

signals:
    void sig_connectToSerialPort(QString, QString);

};
#endif // CUSTOMPLOTPROJECT_H
