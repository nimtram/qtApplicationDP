#include "customplotproject.h"
#include "ui_customplotproject.h"

customPlotProject::customPlotProject(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::customPlotProject)
{
    ui->setupUi(this);

    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    ui->customPlot->graph()->setLineStyle(QCPGraph::lsLine);
    ui->customPlot->xAxis->setLabel("X");
    ui->customPlot->yAxis->setLabel("Y");

    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    COMPORT = new QSerialPort();
    COMPORT->setPortName("COM8");
    COMPORT->setBaudRate(QSerialPort::BaudRate::Baud115200);
    COMPORT->setParity(QSerialPort::Parity::NoParity);
    COMPORT->setDataBits(QSerialPort::DataBits::Data8);
    COMPORT->setStopBits(QSerialPort::StopBits::OneStop);
    COMPORT->setFlowControl(QSerialPort::FlowControl::NoFlowControl);
    COMPORT->open(QIODevice::ReadWrite);

    if(COMPORT->isOpen()){
        qDebug() << "Serial Port Connected";
    }else{
        qDebug() << "Serial Port Disconnected";
    }

    connect(COMPORT,SIGNAL(readyRead()),this,SLOT(receiveMessage()));

}

customPlotProject::~customPlotProject()
{
    delete ui;
}

void customPlotProject::receiveMessage()
{
    //qDebug() << "Received!!";
    QByteArray dataBA = COMPORT->readLine();
    QString data(dataBA);
    buffer.append(data);
    ui->customPlot->graph(0)->;
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();
    ui->customPlot->update();
    ui->uartValuesList->setTextColor(Qt::blue); // Receieved message's color is blue.
    ui->uartValuesList->append(data);

}

void customPlotProject::on_sendToUart_clicked()
{
    COMPORT->write("a",2);
}

