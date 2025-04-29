#include "algorithm_benchmark_window.h"
#include "../core/algorithm.h"
#include "../graphs/graph_algorithms.h"

#include <QStandardItemModel>
#include <QToolBar>
#include <QTreeView>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QThreadPool>
#include <QCheckBox>
#include <QMessageBox>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument.h>

AlgorithmBenchmarkWindow::AlgorithmBenchmarkWindow(QWidget *parent)
    : QMainWindow{parent}
{
    registerAlgorithms();
    setupUi();
}

AlgorithmBenchmarkWindow::~AlgorithmBenchmarkWindow()
{
}

void AlgorithmBenchmarkWindow::onActionSaveTriggered()
{
    const auto reply = QMessageBox::question(this, "Confirmation", "Do you want to save this result and override the existing one?"
                                             , QMessageBox::Yes | QMessageBox::No);

    if(reply == QMessageBox::Yes)
    {
        saveSeriesToFile();
    }
}

void AlgorithmBenchmarkWindow::onActionLoadTriggered()
{
    loadSeriesFromFile();
}

void AlgorithmBenchmarkWindow::onActionClearTriggered()
{
    defaultSeries->clear();
    //chart->removeAllSeries();
}

void AlgorithmBenchmarkWindow::onActionRunBenchmarkTriggered(bool isOn)
{
    Algorithm* algorithm = getSelectedAlgorithm();
    if(algorithm)
    {
        connect(algorithm, &Algorithm::finished, this, &AlgorithmBenchmarkWindow::onAlgorithmFinished);

        QThreadPool::globalInstance()->start(algorithm);
    }
}

void AlgorithmBenchmarkWindow::onActionClearSpikesTriggered()
{
    constexpr qreal threshold = 10.0;

    QList<QPointF> oldPoints = defaultSeries->points();

    QList<QPointF> newPoints;

    for(int i = 1; i < oldPoints.size() - 1; ++i)
    {
        const qreal a = oldPoints[i - 1].y();
        const qreal b = oldPoints[i].y();
        const qreal c = oldPoints[i + 1].y();

        const qreal median = std::max(std::min(a, b), std::min(std::max(a, b), c));

        if(std::abs(b - threshold) < median)
        {
            newPoints.append(oldPoints[i]);
        }
    }

    defaultSeries->replace(newPoints);

    chart->removeSeries(defaultSeries);
    chart->addSeries(defaultSeries);

    chart->createDefaultAxes();
}

void AlgorithmBenchmarkWindow::onAlgorithmFinished(const QList<QPointF>& result)
{
    defaultSeries->replace(result);

    //QLineSeries* newSeries = new QLineSeries(this);
    //newSeries->append(result);

    chart->removeSeries(defaultSeries);
    chart->addSeries(defaultSeries);

    chart->createDefaultAxes();
}

void AlgorithmBenchmarkWindow::setupUi()
{
    setupActionsAndToolBar();

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    defaultSeries = new QLineSeries(this);

    QFont titleFont;
    titleFont.setBold(true);
    titleFont.setPointSize(30);

    chart = new QChart;
    chart->setTitleFont(titleFont);
    chart->addSeries(defaultSeries);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    algorithmsTreeView = new QTreeView(centralWidget);
    algorithmsTreeView->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    algorithmsTreeView->setFixedWidth(240);

    QVBoxLayout* verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(algorithmsTreeView, 1);

    QHBoxLayout* horizontalLayout = new QHBoxLayout(centralWidget);
    horizontalLayout->addLayout(verticalLayout);
    horizontalLayout->addWidget(chartView);

    QStandardItemModel* model = new QStandardItemModel;
    model->setHorizontalHeaderLabels({"All algorithms"});

    for (auto it = algorithms.constBegin(); it != algorithms.constEnd(); ++it)
    {
        QStandardItem* categoryItem = new QStandardItem(it.key());

        const QList<Algorithm*>& algorithmsList = it.value();
        for(auto* algorithm : algorithmsList)
        {
            QStandardItem* algorithmItem = new QStandardItem(algorithm->objectName());
            algorithmItem->setData(QVariant::fromValue(algorithm), Qt::UserRole);
            categoryItem->appendRow(algorithmItem);
        }

        model->appendRow(categoryItem);
    }

    algorithmsTreeView->setModel(model);
    algorithmsTreeView->expandAll();
}

void AlgorithmBenchmarkWindow::setupActionsAndToolBar()
{
    setIconSize(QSize(35, 35));

    QAction* actionSave = new QAction(this);
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/icons/save.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    actionSave->setIcon(icon);
    connect(actionSave, &QAction::triggered, this, &AlgorithmBenchmarkWindow::onActionSaveTriggered);

    QAction* actionLoad = new QAction(this);
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/icons/load.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    actionLoad->setIcon(icon1);
    connect(actionLoad, &QAction::triggered, this, &AlgorithmBenchmarkWindow::onActionLoadTriggered);

    QAction* actionClear = new QAction(this);
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/icons/clear.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    actionClear->setIcon(icon2);
    connect(actionClear, &QAction::triggered, this, &AlgorithmBenchmarkWindow::onActionClearTriggered);

    QAction* actionRunBenchmark = new QAction(this);
    actionRunBenchmark->setCheckable(true);
    QIcon icon4;
    icon4.addFile(QString::fromUtf8(":/icons/play.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    icon4.addFile(QString::fromUtf8(":/icons/pause.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
    actionRunBenchmark->setIcon(icon4);
    connect(actionRunBenchmark, &QAction::triggered, this, &AlgorithmBenchmarkWindow::onActionRunBenchmarkTriggered);

    QAction* actionClearSpikes = new QAction(this);
    QIcon icon5;
    icon5.addFile(QString::fromUtf8(":/icons/clear_spikes.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    actionClearSpikes->setIcon(icon5);
    connect(actionClearSpikes, &QAction::triggered, this, &AlgorithmBenchmarkWindow::onActionClearSpikesTriggered);

#if QT_CONFIG(tooltip)
    actionSave->setToolTip("Save series to file");
    actionLoad->setToolTip("Load series from file");
    actionClear->setToolTip("Clear all series");
    actionRunBenchmark->setToolTip("Pause/Run benchmark");
    actionClearSpikes->setToolTip("Clear spikes");
#endif

    // tool bar
    QToolBar* toolBar = new QToolBar(this);
    toolBar->setMovable(false);
    toolBar->setFloatable(false);
    addToolBar(Qt::ToolBarArea::TopToolBarArea, toolBar);

    toolBar->addAction(actionSave);
    toolBar->addAction(actionLoad);
    toolBar->addAction(actionClear);
    toolBar->addSeparator();
    toolBar->addAction(actionRunBenchmark);
    toolBar->addAction(actionClearSpikes);
}

void AlgorithmBenchmarkWindow::registerAlgorithms()
{
    auto it = algorithms.insert("Graph algorthms", QList<Algorithm*>{});
    QList<Algorithm*>& algorithmsList = it.value();

    algorithmsList.append(new BFSIterative);
    algorithmsList.append(new DFSRecursive);
}

Algorithm *AlgorithmBenchmarkWindow::getSelectedAlgorithm() const
{
    const QModelIndex modelIndex = algorithmsTreeView->selectionModel()->currentIndex();
    return modelIndex.isValid() ? qvariant_cast<Algorithm*>(modelIndex.data(Qt::UserRole)) : nullptr;
}

void AlgorithmBenchmarkWindow::saveSeriesToFile()
{
    QFile saveFile("benchmark series.txt");
    if(!saveFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QJsonArray resultArray;
    const QList<QPointF> result = defaultSeries->points();
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

void AlgorithmBenchmarkWindow::loadSeriesFromFile()
{
    QFile loadFile("benchmark series.txt");
    if(!loadFile.open(QIODevice::ReadOnly))
    {
        return;
    }

    const QByteArray jsonData = loadFile.readAll();
    loadFile.close();

    const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);

    const QJsonArray resultJsonArray = jsonDoc.array();

    QLineSeries* newSeries = new QLineSeries;

    for(const auto& value : resultJsonArray)
    {
        const QJsonObject jsonObj = value.toObject();
        newSeries->append(QPointF(jsonObj["x"].toDouble(), jsonObj["y"].toDouble()));
    }

    chart->addSeries(newSeries);
    chart->createDefaultAxes();
}
