#include "customplotproject.h"
#include "ui_customplotproject.h"
#include <QPixmap>

customPlotProject::customPlotProject(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::customPlotProject)
{
    ui->setupUi(this);
    this->setWindowTitle("testing name of window");
    QPixmap pix("C:/Documents/_vysoka/DP/Qt/customPlotProject/mglb_logo.png");
    ui->pictureLabel->setPixmap(pix);



    serialPortInstance = new serialport;
    serialPortInstance->moveToThread(&serialPortThread);
    connect(&serialPortThread, SIGNAL(started()), serialPortInstance, SLOT(threadStarted()));
    connect(&serialPortThread, SIGNAL(finished()), serialPortInstance, SLOT(threadStopped()));

    connect(ui->SerialPort1Connect, SIGNAL(clicked()),this, SLOT(buttonConnectPressed()));
    connect(this, SIGNAL(sig_connectToSerialPort(QString)),serialPortInstance, SLOT(connectToSerialPort(QString)));
    connect(serialPortInstance, SIGNAL(sig_plot_new_values_x(double)), this, SLOT(plot_new_values_x(double)));
    connect(serialPortInstance, SIGNAL(sig_plot_new_values_y(double)), this, SLOT(plot_new_values_y(double)));
    connect(serialPortInstance, SIGNAL(sig_plot_new_values_z(double)), this, SLOT(plot_new_values_z(double)));

    connect(ui->testButton, SIGNAL(clicked()), serialPortInstance, SLOT(testIt()));

//    connect(ui->button_multiplexerX0, SIGNAL(clicked()), this, SLOT(setMultiplexerTo0_X()));
//    connect(ui->button_multiplexerX1, SIGNAL(clicked()), this, SLOT(setMultiplexerTo1_X()));
    connect(ui->resolution_05V_X, SIGNAL(clicked()), this, SLOT(setResolutionX()));
    connect(ui->resolution_05V_Y, SIGNAL(clicked()), this, SLOT(setResolutionY()));
    connect(ui->resolution_05V_Z, SIGNAL(clicked()), this, SLOT(setResolutionZ()));
    connect(ui->resolution_10V_X, SIGNAL(clicked()), this, SLOT(setResolutionX()));
    connect(ui->resolution_10V_Y, SIGNAL(clicked()), this, SLOT(setResolutionY()));
    connect(ui->resolution_10V_Z, SIGNAL(clicked()), this, SLOT(setResolutionZ()));
    connect(ui->button_setSPS, SIGNAL(clicked()), this, SLOT(setSPSandRestartADCs()));

    connect(ui->SerialPort1Disconnect, SIGNAL(clicked()), serialPortInstance, SLOT(disconnectFromSerialPort()));
    connect(ui->SerialPort1Refresh, SIGNAL(clicked()), this, SLOT(refreshCOMPorts()));
    connect(ui->button_clearPlots, SIGNAL(clicked()),this, SLOT(clearPlots()));


    // Set UI view
    QList<QSerialPortInfo> ports = info.availablePorts();
    QList<QString> stringPorts;
    for(int i = 0 ; i < ports.size() ; i++){
        stringPorts.append(ports.at(i).portName());
    }
    std::reverse(stringPorts.begin(), stringPorts.end());
    ui->SerialPlot1Port->addItems(stringPorts);

    ui->customPlot1->addGraph();
    ui->customPlot1->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
    ui->customPlot1->graph()->setLineStyle(QCPGraph::lsLine);
    ui->customPlot1->yAxis->setLabel("U [V]");
    ui->customPlot1->xAxis->setLabel("Samples");
    ui->customPlot1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot2->addGraph();
    ui->customPlot2->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
    ui->customPlot2->graph()->setLineStyle(QCPGraph::lsLine);
    ui->customPlot2->yAxis->setLabel("U [V]");
    ui->customPlot2->xAxis->setLabel("Samples");
    ui->customPlot2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot3->addGraph();
    ui->customPlot3->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
    ui->customPlot3->graph()->setLineStyle(QCPGraph::lsLine);
    ui->customPlot3->yAxis->setLabel("U [V]");
    ui->customPlot3->xAxis->setLabel("Samples");
    ui->customPlot3->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->list_samplesPerSecond->addItem("5");
    ui->list_samplesPerSecond->addItem("20");
    ui->list_samplesPerSecond->addItem("100");
    ui->list_samplesPerSecond->addItem("500");
    ui->list_samplesPerSecond->addItem("1000");

    ui->lcdNumberX->setSmallDecimalPoint(true);
    ui->lcdNumberY->setSmallDecimalPoint(true);
    ui->lcdNumberZ->setSmallDecimalPoint(true);

    ui->resolution_10V_X->setChecked(true);
    ui->resolution_10V_Y->setChecked(true);
    ui->resolution_10V_Z->setChecked(true);


    counterX = 0;
    counterY = 0;
    counterZ = 0;
    resolution10V_X = true;
    resolution10V_Y = true;
    resolution10V_Z = true;
    rescaleAxesOn = true;



    serialPortThread.start();
    qDebug() << "started";
}



void customPlotProject::buttonConnectPressed(){
    portName = ui->SerialPlot1Port->currentText();
    qDebug() << portName << " " << BAUD_115200 ;
    emit sig_connectToSerialPort(portName);
}

void customPlotProject::plot_new_values_x(double arg_valueToPlot){
    double valueToPlot;
    if(resolution10V_X == true){
        valueToPlot = (20* arg_valueToPlot / (pow(2,32))) - 10 ;
    }else{
        valueToPlot = (arg_valueToPlot / (pow(2,32))) - 0.5 ;
    }

    ui->customPlot1->graph(0)->addData(counterX, valueToPlot);
    if(rescaleAxesOn == true){
        ui->customPlot1->rescaleAxes();
    }
    ui->customPlot1->replot();
    counterX = counterX +1.0;
    ui->lcdNumberX->setDigitCount(QString::number(valueToPlot).length());
    ui->lcdNumberX->display(valueToPlot);
}

void customPlotProject::plot_new_values_y(double arg_valueToPlot){
    double valueToPlot;
    if(resolution10V_Y == true){
        valueToPlot = (20* arg_valueToPlot / (pow(2,32))) - 10 ;
    }else{
        valueToPlot = (arg_valueToPlot / (pow(2,32))) - 0.5 ;
    }

    ui->customPlot2->graph(0)->addData(counterY, valueToPlot);
    if(rescaleAxesOn == true){
        ui->customPlot2->rescaleAxes();
    }
    ui->customPlot2->replot();
    counterY = counterY +1.0;
    ui->lcdNumberY->setDigitCount(QString::number(valueToPlot).length());
    ui->lcdNumberY->display(valueToPlot);
}

void customPlotProject::plot_new_values_z(double arg_valueToPlot){
    double valueToPlot;
    if(resolution10V_Z == true){
        valueToPlot = (20* arg_valueToPlot / (pow(2,32))) - 10 ;
    }else{
        valueToPlot = (arg_valueToPlot / (pow(2,32))) - 0.5 ;
    }
    ui->customPlot3->graph(0)->addData(counterZ, valueToPlot);
    if(rescaleAxesOn == true){
        ui->customPlot3->rescaleAxes();
    }
    ui->customPlot3->replot();
    counterZ = counterZ +1.0;
    ui->lcdNumberZ->setDigitCount(QString::number(valueToPlot).length());
    ui->lcdNumberZ->display(valueToPlot);
}

customPlotProject::~customPlotProject()
{
    delete ui;
}

QVector<double> customPlotProject::createVectorForPlot_x(int numberOfValues){
    QVector<double> yData;
    yData.reserve(numberOfValues);
    int i;
    for(i=counterX; i<(counterX+numberOfValues); i=i+1){
        yData.append(static_cast<double>(i));

    }
    //allMeasuredData_X.append();
    counterX = i;
    return yData;
}

QVector<double> customPlotProject::createVectorForPlot_y(int numberOfValues){
    QVector<double> yData;
    yData.reserve(numberOfValues);
    int i;
    for(i=counterY; i<(counterY+numberOfValues); i=i+1){
        yData.append(static_cast<double>(i));

    }
    counterY = i;
    return yData;
}

QVector<double> customPlotProject::createVectorForPlot_z(int numberOfValues){
    QVector<double> yData;
    yData.reserve(numberOfValues);
    int i;
    for(i=counterZ; i<(counterZ+numberOfValues); i=i+1){
        yData.append(static_cast<double>(i));

    }

    counterZ = i;
    return yData;
}

void customPlotProject::setMultiplexerTo0_X(){
    serialPortInstance->write("a");
}

void customPlotProject::setMultiplexerTo1_X(){
    serialPortInstance->write("b");
}

void customPlotProject::setMultiplexerTo0_Y(){
    serialPortInstance->write("c");
}

void customPlotProject::setMultiplexerTo1_Y(){
    serialPortInstance->write("d");
}

void customPlotProject::setMultiplexerTo0_Z(){
    serialPortInstance->write("e");
}

void customPlotProject::setMultiplexerTo1_Z(){
    serialPortInstance->write("f");
}


void customPlotProject::enableSavingToSDCard(){
    serialPortInstance->write("p");
}

void customPlotProject::disableSavingToSDCard(){
    serialPortInstance->write("q");
}

void customPlotProject::refreshCOMPorts(){
    QList<QSerialPortInfo> ports = info.availablePorts();
    QList<QString> stringPorts;
    for(int i = 0 ; i < ports.size() ; i++){
        stringPorts.append(ports.at(i).portName());
    }
    std::reverse(stringPorts.begin(), stringPorts.end());
    ui->SerialPlot1Port->clear();
    ui->SerialPlot1Port->addItems(stringPorts);
}

void customPlotProject::clearPlots(){
    ui->customPlot1->graph(0)->data()->clear();
    ui->customPlot2->graph(0)->data()->clear();
    ui->customPlot3->graph(0)->data()->clear();
    counterX = 0.0;
    counterY = 0.0;
    counterZ = 0.0;
}

void customPlotProject::setResolutionX()
{
    resolution10V_X = ui->resolution_10V_X->isChecked();
    if(resolution10V_X == true){
        serialPortInstance->write("a");
    }else{
        serialPortInstance->write("b");
    }
}

void customPlotProject::setResolutionY()
{
    resolution10V_Y = ui->resolution_10V_Y->isChecked();
    if(resolution10V_Y == true){
        serialPortInstance->write("c");
    }else{
        serialPortInstance->write("d");
    }
}

void customPlotProject::setResolutionZ()
{
    resolution10V_Z = ui->resolution_10V_Z->isChecked();
    if(resolution10V_Z == true){
        serialPortInstance->write("e");
    }else{
        serialPortInstance->write("f");
    }
}

void customPlotProject::setSPSandRestartADCs()
{
    samplesPerSeconds = ui->list_samplesPerSecond->currentText();
    if(samplesPerSeconds == "5"){
        serialPortInstance->write("0");

    }else if (samplesPerSeconds == "20"){
        serialPortInstance->write("1");

    }else if(samplesPerSeconds == "100"){
        serialPortInstance->write("2");

    }else if(samplesPerSeconds == "500"){
        serialPortInstance->write("3");

    }else if(samplesPerSeconds == "1000"){
        serialPortInstance->write("4");
    }
}

