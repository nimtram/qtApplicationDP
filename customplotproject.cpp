#include "customplotproject.h"
#include "ui_customplotproject.h"

customPlotProject::customPlotProject(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::customPlotProject)
{
    ui->setupUi(this);
    this->setWindowTitle("testing name of window");
    serialComPort = new QSerialPort;

    worker = new serialport;
    worker->moveToThread(&worker_thread);
    connect(&worker_thread, SIGNAL(started()), worker, SLOT(threadStarted()));
    connect(&worker_thread, SIGNAL(finished()), worker, SLOT(threadStopped()));

    connect(ui->SerialPort1Connect, SIGNAL(clicked()),this, SLOT(buttonConnectPressed()));
    connect(this, SIGNAL(sig_connectToSerialPort(QString,QString)),worker, SLOT(connectToSerialPort(QString,QString)));

//    //connections
//    connect(ui->SerialPort1Connect,SIGNAL(clicked()),this,SLOT(connectSerialPort()));
//    connect(this, SIGNAL(comPortOpened()), this,SLOT(readSerialPort()));

    // Set UI view
    QList<QSerialPortInfo> ports = info.availablePorts();
    QList<QString> stringPorts;
    for(int i = 0 ; i < ports.size() ; i++){
        stringPorts.append(ports.at(i).portName());
    }
    ui->SerialPlot1Port->addItems(stringPorts);

    QList<qint32> baudRates = info.standardBaudRates(); // What baudrates does my computer support ?
    QList<QString> stringBaudRates;
    for(int i = 0 ; i < baudRates.size() ; i++){
        stringBaudRates.append(QString::number(baudRates.at(i)));
    }
    ui->SerialPlot1Baud->addItems(stringBaudRates);



// -----------------------------------------------------------------------------------------------------------------------


//    ui->customPlot1->addGraph();
//    ui->customPlot1->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
//    ui->customPlot1->graph()->setLineStyle(QCPGraph::lsLine);
//    ui->customPlot1->xAxis->setLabel("X");
//    ui->customPlot1->yAxis->setLabel("Y");
//    ui->customPlot1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);



    plotIndex = 0;
//    connect(COMPORT_1,SIGNAL(readyRead()),this,SLOT(receiveMessage()));

}



void customPlotProject::buttonConnectPressed(){
    portName = ui->SerialPlot1Port->currentText();
    baudRate = ui->SerialPlot1Baud->currentText();
    qDebug() << portName << " " << baudRate ;
    emit sig_connectToSerialPort(portName, baudRate);
}


customPlotProject::~customPlotProject()
{
    delete ui;
}

