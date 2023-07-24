#ifndef CUSTOMPLOTPROJECT_H
#define CUSTOMPLOTPROJECT_H

#include <QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QString>
#include <QSerialPort>
#include <QObject>

QT_BEGIN_NAMESPACE
namespace Ui { class customPlotProject; }
QT_END_NAMESPACE

class customPlotProject : public QMainWindow
{
    Q_OBJECT

public:
    customPlotProject(QWidget *parent = nullptr);
    ~customPlotProject();

private slots:
    void receiveMessage();

    void on_sendToUart_clicked();

private:
    Ui::customPlotProject *ui;
    QSerialPort* COMPORT;
    QString buffer;
    QString code;
    int codeSize;
};
#endif // CUSTOMPLOTPROJECT_H
