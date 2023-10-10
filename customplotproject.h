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
#include <QDialog>
#include <QLineEdit>
#include <QFile>


QT_BEGIN_NAMESPACE
namespace Ui { class customPlotProject; }
QT_END_NAMESPACE

class customPlotProject : public QMainWindow
{
    Q_OBJECT
    QThread serialPortThread;

public:
    customPlotProject(QWidget *parent = nullptr);
    ~customPlotProject();


private slots:
    void buttonConnectPressed();
    void plot_new_values_x(double);
    void plot_new_values_y(double);
    void plot_new_values_z(double);
    void setMultiplexerTo0_X();
    void setMultiplexerTo1_X();
    void setMultiplexerTo0_Y();
    void setMultiplexerTo1_Y();
    void setMultiplexerTo0_Z();
    void setMultiplexerTo1_Z();
    void enableSavingToSDCard();
    void disableSavingToSDCard();
    void refreshCOMPorts();
    void clearPlots();
    void setResolutionX();
    void setResolutionY();
    void setResolutionZ();
    void setSPSandRestartADCs();
    void openSaveToFileDialog();
    void chooseFileFromSystem();
    void setFilePathValue(QString);
    void setLoggingToFile();
    void stopLoggingToFile();

private:
    QVector<double> createVectorForPlot_x(int);
    QVector<double> createVectorForPlot_y(int);
    QVector<double> createVectorForPlot_z(int);
    serialport *serialPortInstance = new serialport;
    QString portName;
    QString baudRate;

    QSerialPortInfo info;
    Ui::customPlotProject *ui;
    bool updateValues;
    int spsCounter;
    int counterX;
    int counterY;
    int counterZ;
    bool rescaleAxesOn;

    QVector<double> allMeasuredData_X;
    QVector<double> allMeasuredData_Y;
    QVector<double> allMeasuredData_Z;

    QSerialPort* serialComPort;
    QSerialPort* COMPORT_2;
    QString buffer;

    QString code;
    double plotIndex;
    bool conversionSuccess;
    QVector <double> x_plotValues = {};
    QVector <double> y_plotValues = {};
    int codeSize;
    bool resolution10V_X;
    bool resolution10V_Y;
    bool resolution10V_Z;
    QString samplesPerSeconds;
    QString selectedFileName;
    QLineEdit *filePath;
    bool loggingToFileEnabled;
    QTextStream fileOut;
    QFile file;
    QDialog *dialogWindow;
    quint32 valuesStoredToFile;

public:
    QQueue<QByteArray> queue;

private:
    bool writeDataToFile(const QString &data);
    QString createHeaderForFile(void);

signals:
    void sig_connectToSerialPort(QString);
    void sig_setFilePathValue(QString);


};
#endif // CUSTOMPLOTPROJECT_H
