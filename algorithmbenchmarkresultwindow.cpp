#include "AlgorithmBenchmarkResultWindow.h"
#include "ui_AlgorithmBenchmarkResultWindow.h"

#include <QChart>
#include <QChartView>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLineSeries>
#include <QMessageBox>

AlgorithmBenchmarkResultWindow::AlgorithmBenchmarkResultWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AlgorithmBenchmarkResultWindow)
    , defaultSeries(new QLineSeries)
    , chart(new QChart)
    , chartView(new QChartView(chart))
{
    setupUi();
}

AlgorithmBenchmarkResultWindow::~AlgorithmBenchmarkResultWindow()
{
    delete ui;
}

void AlgorithmBenchmarkResultWindow::setChartTitle(const QString& title)
{
    chart->setTitle(title);
}

void AlgorithmBenchmarkResultWindow::setResult(const QList<QPointF>& result)
{
    defaultSeries->append(result);
    chart->addSeries(defaultSeries);
    chart->createDefaultAxes();
}

void AlgorithmBenchmarkResultWindow::on_actionSave_result_to_file_triggered()
{
    const auto reply = QMessageBox::question(this, "Confirmation", "Do you want to save this result and override the existing one?"
                                             , QMessageBox::Yes | QMessageBox::No);

    if(reply == QMessageBox::Yes)
    {
        saveResultToFile();
    }
}

void AlgorithmBenchmarkResultWindow::on_actionLoad_result_from_file_triggered()
{
    loadResultFromFile();
}

void AlgorithmBenchmarkResultWindow::setupUi()
{
    ui->setupUi(this);

    QFont titleFont;
    titleFont.setBold(true);
    titleFont.setPointSize(30);

    chart->setTitleFont(titleFont);

    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setChart(chart);

    setCentralWidget(chartView);
    resize(1200, 800);
}

void AlgorithmBenchmarkResultWindow::saveResultToFile()
{
    QFile saveFile("algorithm benchmark result.txt");
    if(!saveFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QJsonArray resultArray;
    const auto result = defaultSeries->points();
    for(const auto& point : result)
    {
        QJsonObject pointAsJsonObject;
        pointAsJsonObject["x"] = point.x();
        pointAsJsonObject["y"] = point.y();
        resultArray.append(pointAsJsonObject);
    }

    const QJsonDocument jsonDoc(resultArray);

    saveFile.write(jsonDoc.toJson());
    saveFile.close();
}

void AlgorithmBenchmarkResultWindow::loadResultFromFile()
{
    QFile loadFile("algorithm benchmark result.txt");
    if(!loadFile.open(QIODevice::ReadOnly))
    {
        return;
    }

    const QByteArray jsonData = loadFile.readAll();
    loadFile.close();

    const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);

    const QJsonArray resultJsonArray = jsonDoc.array();

    QLineSeries *newSeries = new QLineSeries;

    for(const auto& value : resultJsonArray)
    {
        const auto jsonObj = value.toObject();
        newSeries->append(QPointF(jsonObj["x"].toDouble(), jsonObj["y"].toDouble()));
    }

    chart->addSeries(newSeries);
    chart->createDefaultAxes();
}
