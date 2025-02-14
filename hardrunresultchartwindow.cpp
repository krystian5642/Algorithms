#include "hardrunresultchartwindow.h"
#include "ui_hardrunresultchartwindow.h"

#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QMessageBox>

HardRunResultChartWindow::HardRunResultChartWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HardRunResultChartWindow)
    , series(new QLineSeries)
    , chart(new QChart)
    , chartView(new QChartView(chart))
{
    setupUi();
}

HardRunResultChartWindow::~HardRunResultChartWindow()
{
    delete ui;
}

void HardRunResultChartWindow::setChartTitle(const QString& title)
{
    chart->setTitle(title);
}

void HardRunResultChartWindow::setResult(const QList<QPointF>& result)
{
    series->append(result);
    chart->addSeries(series);
    chart->createDefaultAxes();
}

void HardRunResultChartWindow::on_actionSave_result_to_file_triggered()
{
    const auto reply = QMessageBox::question(this, "Confirmation", "Do you want to save this result and override the existing one?"
                                             , QMessageBox::Yes | QMessageBox::No);

    if(reply == QMessageBox::Yes)
    {
        saveChartToFile();
    }
}

void HardRunResultChartWindow::on_actionLoad_result_from_file_triggered()
{
    loadChartFromFile();
}

void HardRunResultChartWindow::setupUi()
{
    ui->setupUi(this);

    series->setName("T(V+E)");

    QFont titleFont;
    titleFont.setBold(true);
    titleFont.setPointSize(30);

    chart->setTitleFont(titleFont);

    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setChart(chart);

    setCentralWidget(chartView);
    resize(1200, 800);
}

void HardRunResultChartWindow::saveChartToFile()
{

}

void HardRunResultChartWindow::loadChartFromFile()
{

}
