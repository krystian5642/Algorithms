#include "Algorithmsmainwindow.h"
#include <QApplication>

#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AlgorithmsMainWindow w;
    w.show();
    return a.exec();
}
