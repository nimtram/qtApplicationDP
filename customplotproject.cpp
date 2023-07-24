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

}

customPlotProject::~customPlotProject()
{
    delete ui;
}

