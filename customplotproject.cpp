#include "customplotproject.h"
#include "ui_customplotproject.h"
#include <QPixmap>

#define LCD_DIGITS_TO_SHOW 11


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
    connect(ui->button_logToFile, SIGNAL(clicked()), this, SLOT(openSaveToFileDialog()));

    connect(ui->button_stopLogToFile, SIGNAL(clicked()),this, SLOT(stopLoggingToFile()));


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
    ui->customPlot1->xAxis->setRange(0, 50);
    ui->customPlot1->yAxis->setLabel("U [V]");
    ui->customPlot1->xAxis->setLabel("Samples");
    ui->customPlot1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot2->addGraph();
    ui->customPlot2->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
    ui->customPlot2->graph()->setLineStyle(QCPGraph::lsLine);
    ui->customPlot2->xAxis->setRange(0, 50);
    ui->customPlot2->yAxis->setLabel("U [V]");
    ui->customPlot2->xAxis->setLabel("Samples");
    ui->customPlot2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot3->addGraph();
    ui->customPlot3->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
    ui->customPlot3->graph()->setLineStyle(QCPGraph::lsLine);
    ui->customPlot3->xAxis->setRange(0, 50);
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
    loggingToFileEnabled = false;
    ui->loggingToFileText->setText("Saving to file active");
    QFont  font;
    font.setPointSize(16);
    ui->loggingToFileText->setFont(font);
    ui->loggingToFileText->setStyleSheet("background-color: green; color: black; padding: 0px;");
    ui->loggingToFileText->hide();


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


    ui->customPlot1->xAxis->setRange(counterX-50, counterX);
    ui->customPlot1->replot();
    counterX = counterX +1.0;

    QString formattedNumber = QString::number(valueToPlot, 'f', LCD_DIGITS_TO_SHOW);
    ui->lcdNumberX->setDigitCount(LCD_DIGITS_TO_SHOW);
    ui->lcdNumberX->display(formattedNumber);

    if(loggingToFileEnabled == true){
        writeDataToFile(formattedNumber);
    }
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
    ui->customPlot2->xAxis->setRange(counterY-50, counterY);
    ui->customPlot2->replot();
    counterY = counterY +1.0;
    QString formattedNumber = QString::number(valueToPlot, 'f', LCD_DIGITS_TO_SHOW);
    ui->lcdNumberY->setDigitCount(LCD_DIGITS_TO_SHOW);
    ui->lcdNumberY->display(formattedNumber);
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
    ui->customPlot3->xAxis->setRange(counterZ-50, counterZ);
    ui->customPlot3->replot();
    counterZ = counterZ +1.0;
    QString formattedNumber = QString::number(valueToPlot, 'f', LCD_DIGITS_TO_SHOW);
    ui->lcdNumberZ->setDigitCount(LCD_DIGITS_TO_SHOW);
    ui->lcdNumberZ->display(formattedNumber);
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
    ui->customPlot1->replot();
    ui->customPlot2->replot();
    ui->customPlot3->replot();
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

void customPlotProject::openSaveToFileDialog()
{
    dialogWindow = new QDialog();
    dialogWindow->setFixedWidth(640);
    dialogWindow->setFixedHeight(240);
    dialogWindow->setWindowModality(Qt::WindowModality::ApplicationModal);
    dialogWindow->show();

    filePath = new QLineEdit();
    filePath->setGeometry(10,10,500,30);
    filePath->setParent(dialogWindow);
    filePath->show();


    QPushButton *threeDotButton = new QPushButton();
    threeDotButton->setGeometry(520, 20, 40, 20); // Set the button's position and size
    threeDotButton->setText("...");
    threeDotButton->setParent(dialogWindow);
    threeDotButton->show();

    QPushButton *button_startLogging = new QPushButton();
    button_startLogging->setGeometry(540, 190, 80, 40); // Set the button's position and size
    button_startLogging->setText("Start logging");
    button_startLogging->setParent(dialogWindow);
    button_startLogging->show();

//    savingLabel = new QLabel("Saving in progress!");
//    savingLabel->setGeometry(10, 50, 200, 50);
//    QFont  font;
//    font.setPointSize(16);
//    savingLabel->setFont(font);
//    savingLabel->setStyleSheet("background-color: green; color: black; padding: 5px;");
//    savingLabel->setParent(dialogWindow);

    connect(threeDotButton, SIGNAL(clicked()), this, SLOT(chooseFileFromSystem()));
    connect(this, SIGNAL(sig_setFilePathValue(QString)),this, SLOT(setFilePathValue(QString)));
    connect(button_startLogging, SIGNAL(clicked()),this, SLOT(setLoggingToFile()));
}

void customPlotProject::chooseFileFromSystem()
{
    selectedFileName = QFileDialog::getSaveFileName(this, "Save File", QDir::homePath(), "Text Files (*.txt);;All Files (*)");
    emit sig_setFilePathValue(selectedFileName);
}

void customPlotProject::setFilePathValue(QString path)
{
    filePath->setText(path);
}

void customPlotProject::setLoggingToFile()
{
//    file.setFileName(filePath);
//    if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
//        fileOut.setDevice(&file);

//    }
    file.setFileName(selectedFileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
        fileOut.setDevice(&file);
        loggingToFileEnabled = true;
        ui->loggingToFileText->show();

    }else{
        loggingToFileEnabled = false;
    }
    dialogWindow->close();
}

void customPlotProject::stopLoggingToFile()
{
    loggingToFileEnabled = false;
    file.close();
    ui->loggingToFileText->hide();
}

bool customPlotProject::writeDataToFile(const QString &data){
    if (file.isOpen() && fileOut.status() == QTextStream::Ok)
    {
        // Write data to the file
        fileOut << data << Qt::endl;
        return true;
    }
    else
    {
        return false;
    }
}

