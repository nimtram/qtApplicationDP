#ifndef CUSTOMPLOTPROJECT_H
#define CUSTOMPLOTPROJECT_H

#include <QMainWindow>
#include <QSerialPort>

QT_BEGIN_NAMESPACE
namespace Ui { class customPlotProject; }
QT_END_NAMESPACE

class customPlotProject : public QMainWindow
{
    Q_OBJECT

public:
    customPlotProject(QWidget *parent = nullptr);
    ~customPlotProject();

private:
    Ui::customPlotProject *ui;
};
#endif // CUSTOMPLOTPROJECT_H
