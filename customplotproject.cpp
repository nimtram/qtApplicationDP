#include "customplotproject.h"
#include "ui_customplotproject.h"
#include <QPixmap>

#define LCD_DIGITS_TO_SHOW 11


customPlotProject::customPlotProject(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::customPlotProject)
{
    ui->setupUi(this);
    this->setWindowTitle("Three channel ADC board");
//    QPixmap pix("C:/Documents/_vysoka/DP/Qt/customPlotProject/mglb_logo.png");
//    ui->pictureLabel->setPixmap(pix);

    QPixmap img ("C:/Documents/_vysoka/DP/Qt/customPlotProject/mglb_logo.png");
    QPixmap scaledImg = img.scaled(400, 800);
    ui->label_maglab->setPixmap(scaledImg);
//    QVBoxLayout *layout = new QVBoxLayout();
//    layout->addWidget(label_maglab);
//    setLayout(layout);




    serialPortInstance = new serialport;
    serialPortInstance->moveToThread(&serialPortThread);
    connect(&serialPortThread, SIGNAL(started()), serialPortInstance, SLOT(threadStarted()));
    connect(&serialPortThread, SIGNAL(finished()), serialPortInstance, SLOT(threadStopped()));
    connect(serialPortInstance, SIGNAL(sig_serialPortConnected()),this, SLOT(serialPortConnected()));
    connect(serialPortInstance, SIGNAL(sig_serialPortDisconnected()),this, SLOT(serialPortDisconnected()));

    connect(ui->SerialPort1Connect, SIGNAL(clicked()),this, SLOT(buttonConnectPressed()));
    connect(this, SIGNAL(sig_connectToSerialPort(QString)),serialPortInstance, SLOT(connectToSerialPort(QString)));
    connect(serialPortInstance, SIGNAL(sig_plot_new_values_x(double)), this, SLOT(plot_new_values_x(double)));
    connect(serialPortInstance, SIGNAL(sig_plot_new_values_y(double)), this, SLOT(plot_new_values_y(double)));
    connect(serialPortInstance, SIGNAL(sig_plot_new_values_z(double)), this, SLOT(plot_new_values_z(double)));

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
    connect(ui->button_openFile, SIGNAL(clicked()), this, SLOT(openSaveToFileDialog()));

    connect(ui->button_closeFile, SIGNAL(clicked()),this, SLOT(closeFileForLogging()));

    connect(ui->button_logToFile, SIGNAL(clicked()),this, SLOT(startLoggingToFile()));
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

    valuesStoredToFile = 0;
    updateValues = true;
    spsCounter = 0;
    counterX = 0;
    counterY = 0;
    counterZ = 0;
    resolution10V_X = true;
    resolution10V_Y = true;
    resolution10V_Z = true;
    rescaleAxesOn = true;
    loggingToFileEnabled = false;
    ui->loggingToFileText->setText("Saving to file active: ");
    QFont  font;
    font.setPointSize(16);
    ui->loggingToFileText->setFont(font);
    ui->loggingToFileText->setStyleSheet("background-color: green; color: black; padding: 0px;");
    ui->loggingToFileText->hide();
    samplesPerSeconds = ui->list_samplesPerSecond->currentText();

    samplesDivider = 1;
    samplesPerSeconds = "5";
    counterHighSPS = 0;
    skipDueToDelay = false;

    ui->SerialPort1Disconnect->setEnabled(false);
    ui->button_closeFile->setEnabled(false);
    ui->button_logToFile->setEnabled(false);
    ui->button_stopLogToFile->setEnabled(false);

    serialPortThread.start();

}

customPlotProject::~customPlotProject()
{
    delete ui;
    serialPortThread.quit();
    serialPortThread.wait();
}


void customPlotProject::buttonConnectPressed(){
    portName = ui->SerialPlot1Port->currentText();
    emit sig_connectToSerialPort(portName);
}

void customPlotProject::plot_new_values_x(double arg_valueToPlot){
    if (skipDueToDelay == true) return;
    double valueToPlot;
    if(resolution10V_X == true){
        valueToPlot = (20* arg_valueToPlot / (pow(2,32))) - 10 ;
    }else{
        valueToPlot = (arg_valueToPlot / (pow(2,32))) - 0.5 ;
    }
    QString formattedNumber = QString::number(valueToPlot, 'f', LCD_DIGITS_TO_SHOW);
    if (samplesPerSeconds != "5" ){
        if (spsCounter % samplesDivider ==0){
            updateValues = true;
        }
        spsCounter++;
    }

    if(samplesPerSeconds == "5"){
        ui->customPlot1->graph(0)->addData(counterX, valueToPlot);
        if(rescaleAxesOn == true){
            ui->customPlot1->rescaleAxes();
        }
        ui->customPlot1->xAxis->setRange(counterX-50, counterX);
        ui->customPlot1->replot();
        counterX = counterX +1.0;
        ui->lcdNumberX->setDigitCount(LCD_DIGITS_TO_SHOW);
        ui->lcdNumberX->display(formattedNumber);

    }else if(updateValues == true && samplesPerSeconds != "5"){
        ui->customPlot1->graph(0)->addData(counterHighSPS, valueToPlot);
        if(rescaleAxesOn == true){
            ui->customPlot1->rescaleAxes();
        }
        ui->customPlot1->xAxis->setRange(counterHighSPS-50, counterHighSPS);
        ui->customPlot1->replot();
        ui->lcdNumberX->setDigitCount(LCD_DIGITS_TO_SHOW);
        ui->lcdNumberX->display(formattedNumber);
    }

    if(loggingToFileEnabled == true){
        bool resWriteData = writeDataToFile((formattedNumber + " "));
        if(resWriteData == false){
            QMessageBox::critical(nullptr, "Error", "An error has occurred while writing to a file!");
            closeFileForLogging();
        }
    }
}

void customPlotProject::plot_new_values_y(double arg_valueToPlot){
    if (skipDueToDelay == true) return;
    double valueToPlot;
    if(resolution10V_Y == true){
        valueToPlot = (20* arg_valueToPlot / (pow(2,32))) - 10 ;
    }else{
        valueToPlot = (arg_valueToPlot / (pow(2,32))) - 0.5 ;
    }
    QString formattedNumber = QString::number(valueToPlot, 'f', LCD_DIGITS_TO_SHOW);

    if(samplesPerSeconds == "5"){
        ui->customPlot2->graph(0)->addData(counterY, valueToPlot);
        if(rescaleAxesOn == true){
            ui->customPlot2->rescaleAxes();
        }
        ui->customPlot2->xAxis->setRange(counterY-50, counterY);
        ui->customPlot2->replot();
        counterY = counterY +1.0;
        ui->lcdNumberY->setDigitCount(LCD_DIGITS_TO_SHOW);
        ui->lcdNumberY->display(formattedNumber);

    }else if(updateValues == true && samplesPerSeconds != "5"){
        ui->customPlot2->graph(0)->addData(counterHighSPS, valueToPlot);
        if(rescaleAxesOn == true){
            ui->customPlot2->rescaleAxes();
        }
        ui->customPlot2->xAxis->setRange(counterHighSPS-50, counterHighSPS);
        ui->customPlot2->replot();
        ui->lcdNumberY->setDigitCount(LCD_DIGITS_TO_SHOW);
        ui->lcdNumberY->display(formattedNumber);
    }

    if(loggingToFileEnabled == true){
        bool resWriteData = writeDataToFile((formattedNumber + " "));
        if(resWriteData == false){
            QMessageBox::critical(nullptr, "Error", "An error has occurred while writing to a file!");
            closeFileForLogging();
        }
    }
}

void customPlotProject::plot_new_values_z(double arg_valueToPlot){
    if (skipDueToDelay == true) return;
    double valueToPlot;
    if(resolution10V_Z == true){
        valueToPlot = (20* arg_valueToPlot / (pow(2,32))) - 10 ;
    }else{
        valueToPlot = (arg_valueToPlot / (pow(2,32))) - 0.5 ;
    }
    QString formattedNumber = QString::number(valueToPlot, 'f', LCD_DIGITS_TO_SHOW);

    if(samplesPerSeconds == "5"){
        ui->customPlot3->graph(0)->addData(counterZ, valueToPlot);
        if(rescaleAxesOn == true){
            ui->customPlot3->rescaleAxes();
        }
        ui->customPlot3->xAxis->setRange(counterZ-50, counterZ);
        ui->customPlot3->replot();
        counterZ = counterZ +1.0;
        ui->lcdNumberZ->setDigitCount(LCD_DIGITS_TO_SHOW);
        ui->lcdNumberZ->display(formattedNumber);

    }else if(updateValues == true && samplesPerSeconds != "5"){
        ui->customPlot3->graph(0)->addData(counterHighSPS, valueToPlot);
        if(rescaleAxesOn == true){
            ui->customPlot3->rescaleAxes();
        }
        ui->customPlot3->xAxis->setRange(counterHighSPS-50, counterHighSPS);
        ui->customPlot3->replot();
        ui->lcdNumberZ->setDigitCount(LCD_DIGITS_TO_SHOW);
        ui->lcdNumberZ->display(formattedNumber);
        updateValues = false;
        counterHighSPS++;
    }

    if(loggingToFileEnabled == true){
        bool resWriteData = writeDataToFile((formattedNumber + "\n"));
        if(resWriteData == false){
            QMessageBox::critical(nullptr, "Error", "An error has occurred while writing to a file!");
            closeFileForLogging();
        }
        valuesStoredToFile++;
        uint8_t moduloDivider;
        if(samplesPerSeconds == "5"){
            moduloDivider = 10;
        }else{
            moduloDivider = 100;
        }
        if ((valuesStoredToFile % moduloDivider) ==0){
            ui->loggingToFileText->setText("Saving to file active: " + QString::number(valuesStoredToFile));
        }
    }
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
    counterHighSPS = 0;
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
        samplesDivider = 1;

    }else if (samplesPerSeconds == "20"){
        serialPortInstance->write("1");
        samplesDivider = 4;

    }else if(samplesPerSeconds == "100"){
        serialPortInstance->write("2");
        samplesDivider = 20;

    }else if(samplesPerSeconds == "500"){
        serialPortInstance->write("3");
        samplesDivider = 100;

    }else if(samplesPerSeconds == "1000"){
        serialPortInstance->write("4");
        samplesDivider = 200;
    }
    skipDueToDelay = true;
    clearPlots();
    delay();
    skipDueToDelay = false;
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

    QPushButton *button_dialogOpenFile = new QPushButton();
    button_dialogOpenFile->setGeometry(540, 190, 80, 40); // Set the button's position and size
    button_dialogOpenFile->setText("Open file");
    button_dialogOpenFile->setParent(dialogWindow);
    button_dialogOpenFile->show();

    connect(threeDotButton, SIGNAL(clicked()), this, SLOT(chooseFileFromSystem()));
    connect(button_dialogOpenFile, SIGNAL(clicked()),this, SLOT(openFileForLogging()));
}

void customPlotProject::chooseFileFromSystem()
{
    selectedFileName = QFileDialog::getSaveFileName(this, "Save File", QDir::homePath(), "Text Files (*.txt);;All Files (*)");
    filePath->setText(selectedFileName);
}

void customPlotProject::openFileForLogging()
{
    selectedFileName = filePath->text();
    file.setFileName(selectedFileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
        fileOut.setDevice(&file);
        fileOpened = true;
//        loggingToFileEnabled = true;
        //ui->loggingToFileText->show();

    }else{
        //loggingToFileEnabled = false;
        fileOpened = false;
    }
//    QString fileHeader = createHeaderForFile();
//    writeDataToFile(fileHeader);
//    valuesStoredToFile = 0;
    ui->button_openFile->setEnabled(false);
    ui->button_closeFile->setEnabled(true);
    ui->button_logToFile->setEnabled(true);
    ui->button_stopLogToFile->setEnabled(true);
    dialogWindow->close();
}

void customPlotProject::closeFileForLogging()
{
    if (loggingToFileEnabled==false){
        fileOpened = false;
        file.close();
        ui->button_openFile->setEnabled(true);
        ui->button_closeFile->setEnabled(false);
        ui->button_logToFile->setEnabled(false);
        ui->button_stopLogToFile->setEnabled(false);
    }
}

void customPlotProject::serialPortConnected()
{
    ui->SerialPort1Connect->setEnabled(false);
    ui->SerialPort1Disconnect->setEnabled(true);
}

void customPlotProject::serialPortDisconnected()
{
    ui->SerialPort1Connect->setEnabled(true);
    ui->SerialPort1Disconnect->setEnabled(false);
}

void customPlotProject::startLoggingToFile()
{
    QString fileHeader = createHeaderForFile();
    writeDataToFile(fileHeader);
    ui->loggingToFileText->show();
    loggingToFileEnabled = true;
    ui->button_closeFile->setEnabled(false);
}

void customPlotProject::stopLoggingToFile()
{
    loggingToFileEnabled = false;
    ui->loggingToFileText->hide();
    valuesStoredToFile = 0;
    ui->button_closeFile->setEnabled(true);
}

bool customPlotProject::writeDataToFile(const QString &data){
    if (file.isOpen() && fileOut.status() == QTextStream::Ok)
    {
        // Write data to the file
        fileOut << data;
        return true;
    }
    else
    {
        return false;
    }
}

QString customPlotProject::createHeaderForFile(){
    const QString infoString = "Setting of x,y,z data: ";
    QString spsString ="SPS " + samplesPerSeconds + ", ";
    QString resolutionString_x;
    QString resolutionString_y;
    QString resolutionString_z;
    if (resolution10V_X == true){
        resolutionString_x ="resolution x: 10V, ";
    }else{
        resolutionString_x ="resolution x: 0.5V, ";
    }
    if (resolution10V_Y == true){
        resolutionString_y ="resolution y: 10V, ";
    }else{
        resolutionString_y ="resolution y: 0.5V, ";
    }
    if (resolution10V_Z == true){
        resolutionString_z ="resolution z: 10V";
    }else{
        resolutionString_z ="resolution z: 0.5V";
    }
    QString endString ="\n";

    QString outputString = infoString + spsString + resolutionString_x + resolutionString_y + resolutionString_z + endString;
    return outputString;
}

void customPlotProject::delay()
{
    QTime dieTime= QTime::currentTime().addSecs(1);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
