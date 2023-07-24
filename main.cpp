#include "customplotproject.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    customPlotProject w;
    w.show();
    return a.exec();
}
