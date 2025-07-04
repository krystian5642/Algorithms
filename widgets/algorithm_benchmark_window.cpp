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
#include <QJsonDocument>
#include <QToolTip>
#include <QLabel>
#include <QPointer>
#include <QLegendMarker>

AlgorithmBenchmarkWindow::AlgorithmBenchmarkWindow(QWidget *parent)
    : QMainWindow{parent}
{
    registerAlgorithms();
    setupUi();
}

AlgorithmBenchmarkWindow::~AlgorithmBenchmarkWindow()
{
    endBenchmarkRun(true);
}

void AlgorithmBenchmarkWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        clearSeriesSelection();
    }
    else
    {
        QMainWindow::mousePressEvent(event);
    }
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
    chart->removeAllSeries();
    seriesToLabel.clear();
    seriesToToolTipText.clear();
}

void AlgorithmBenchmarkWindow::onActionRunBenchmarkTriggered(bool isOn)
{
    if(isOn)
    {
        Algorithm* algorithm = getSelectedAlgorithm();
        if(algorithm)
        {
            QThreadPool::globalInstance()->start(algorithm);
        }
        else
        {
            actionRunBenchmark->setChecked(false);

            QMessageBox::information(this, "Info", "No algorithm selected");
        }
    }
    else
    {
        actionRunBenchmark->setEnabled(false);
        endBenchmarkRun(false);
    }
}

void AlgorithmBenchmarkWindow::onActionClearSpikesTriggered()
{
    constexpr qreal threshold = 10.0;

    auto seriesArray = chart->series();
    for(auto* series : seriesArray)
    {
        QLineSeries* lineSeries = qobject_cast<QLineSeries*>(series);

        QList<QPointF> oldPoints = lineSeries->points();

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

        lineSeries->replace(newPoints);

        chart->removeSeries(lineSeries);
        addSeries(lineSeries);
    }

    chart->createDefaultAxes();
}

#ifdef QT_DEBUG
void AlgorithmBenchmarkWindow::onActionDebugRunTriggered()
{
    Algorithm* algorithm = getSelectedAlgorithm();
    if(algorithm)
    {
        algorithm->debugRun();
    }
    else
    {
        QMessageBox::information(this, "Info", "No algorithm selected");
    }
}
#endif

void AlgorithmBenchmarkWindow::onAlgorithmTreeItemClicked(const QModelIndex &index)
{
    QLayoutItem* layoutItem = verticalLayout->takeAt(1);
    if(layoutItem)
    {
        QWidget* widget = layoutItem->widget();
        if(widget)
        {
            widget->deleteLater();
        }
        delete layoutItem;
    }

    Algorithm* algorithm = qvariant_cast<Algorithm*>(index.data(Qt::UserRole));
    if(algorithm)
    {
        QWidget* propertyWidget = algorithm->createPropertiesWidget(this);
        verticalLayout->addWidget(propertyWidget, 1);
    }
    else
    {
        verticalLayout->addStretch(1);
    }
}

void AlgorithmBenchmarkWindow::onAlgorithmStarted()
{
    actionRunBenchmark->setChecked(true);
}

void AlgorithmBenchmarkWindow::onAlgorithmFinished(const QList<QPointF>& result, const QString& toolTipText)
{
    actionRunBenchmark->setChecked(false);
    actionRunBenchmark->setEnabled(true);

    QLineSeries* newSeries = new QLineSeries(this);
    newSeries->append(result);
    newSeries->setName(sender()->objectName() + "_" + QString::number(chart->series().size()));
    connect(newSeries, &QLineSeries::pressed, this, &AlgorithmBenchmarkWindow::onLineSeriesPressed);

    seriesToToolTipText[newSeries] = toolTipText;

    addSeries(newSeries);

    chart->createDefaultAxes();
}

void AlgorithmBenchmarkWindow::onLineSeriesPressed(const QPointF &point)
{
    QLineSeries* lineSeries = qobject_cast<QLineSeries*>(sender());
    auto it = seriesToLabel.constFind(lineSeries);

    int penWidth = 2;

    if(it != seriesToLabel.constEnd())
    {
        QLabel* label = *it;
        label->deleteLater();

        seriesToLabel.remove(lineSeries);
    }
    else
    {
        QPointer<QLabel> label = new QLabel(seriesToToolTipText[lineSeries], chartView);
        label->setStyleSheet("border: 1px solid black; background-color: white;");
        label->adjustSize();

        auto onLineSeriesDestroyed = [this, label]()
        {
            if(!label.isNull())
            {
                label->deleteLater();
            }

            QLineSeries* lineSeries = qobject_cast<QLineSeries*>(sender());
            if(lineSeries)
            {
                seriesToLabel.remove(lineSeries);
                seriesToToolTipText.remove(lineSeries);
            }
        };

        connect(lineSeries, &QLineSeries::destroyed, this, onLineSeriesDestroyed);

        const QPoint relativePos = chartView->mapFromGlobal(QCursor::pos());
        label->move(relativePos);

        label->show();

        seriesToLabel[lineSeries] = label;

        penWidth = 6;
    }

    QPen pen = lineSeries->pen();
    pen.setWidth(penWidth);
    lineSeries->setPen(pen);
}

void AlgorithmBenchmarkWindow::onPlotAreaChanged(const QRectF &plotArea)
{
    clearSeriesSelection();
}

void AlgorithmBenchmarkWindow::onLegendMarkerClicked()
{
    QLegendMarker* marker = qobject_cast<QLegendMarker*>(sender());
    chart->removeSeries(marker->series());
    chart->createDefaultAxes();
}

void AlgorithmBenchmarkWindow::onLegendMarkerHovered(bool state)
{
#if QT_CONFIG(tooltip)
    if(state)
    {
        QToolTip::showText(QCursor::pos(), "Click to remove series", chartView);
    }
    else
    {
        QToolTip::hideText();
    }
#endif
}

void AlgorithmBenchmarkWindow::setupUi()
{
    setupActionsAndToolBar();

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QFont titleFont;
    titleFont.setBold(true);
    titleFont.setPointSize(30);

    chart = new QChart;
    chart->setTitleFont(titleFont);
    connect(chart, &QChart::plotAreaChanged, this, &AlgorithmBenchmarkWindow::onPlotAreaChanged);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    algorithmsTreeView = new QTreeView(centralWidget);
    algorithmsTreeView->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    algorithmsTreeView->setFixedWidth(270);
    connect(algorithmsTreeView, &QTreeView::clicked, this, &AlgorithmBenchmarkWindow::onAlgorithmTreeItemClicked);

    QWidget* layoutHolder = new QWidget(this);
    layoutHolder->setFixedWidth(285);

    verticalLayout = new QVBoxLayout(layoutHolder);
    verticalLayout->addWidget(algorithmsTreeView, 1);

    QHBoxLayout* horizontalLayout = new QHBoxLayout(centralWidget);
    horizontalLayout->addWidget(layoutHolder);
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
    setIconSize(QSize(40, 40));

    actionSave = new QAction(this);
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/icons/save.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    actionSave->setIcon(icon);
    connect(actionSave, &QAction::triggered, this, &AlgorithmBenchmarkWindow::onActionSaveTriggered);

    actionLoad = new QAction(this);
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/icons/load.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    actionLoad->setIcon(icon1);
    connect(actionLoad, &QAction::triggered, this, &AlgorithmBenchmarkWindow::onActionLoadTriggered);

    actionClear = new QAction(this);
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/icons/clear.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    actionClear->setIcon(icon2);
    connect(actionClear, &QAction::triggered, this, &AlgorithmBenchmarkWindow::onActionClearTriggered);

    actionRunBenchmark = new QAction(this);
    actionRunBenchmark->setCheckable(true);
    actionRunBenchmark->setChecked(false);
    QIcon icon4;
    icon4.addFile(QString::fromUtf8(":/icons/play.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    icon4.addFile(QString::fromUtf8(":/icons/pause.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
    actionRunBenchmark->setIcon(icon4);
    connect(actionRunBenchmark, &QAction::triggered, this, &AlgorithmBenchmarkWindow::onActionRunBenchmarkTriggered);

    actionClearSpikes = new QAction(this);
    QIcon icon5;
    icon5.addFile(QString::fromUtf8(":/icons/clear_spikes.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    actionClearSpikes->setIcon(icon5);
    connect(actionClearSpikes, &QAction::triggered, this, &AlgorithmBenchmarkWindow::onActionClearSpikesTriggered);

#ifdef QT_DEBUG
    debugRun = new QAction(this);
    QIcon icon6;
    icon6.addFile(QString::fromUtf8(":/icons/debug.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    debugRun->setIcon(icon6);
    connect(debugRun, &QAction::triggered, this, &AlgorithmBenchmarkWindow::onActionDebugRunTriggered);
#endif

#if QT_CONFIG(tooltip)
    actionSave->setToolTip("Save series to file");
    actionLoad->setToolTip("Load series from file");
    actionClear->setToolTip("Clear all series");
    actionRunBenchmark->setToolTip("Pause/Run benchmark");
    actionClearSpikes->setToolTip("Clear spikes");

#ifdef QT_DEBUG
    debugRun->setToolTip("Debug run: select algorithm and view result in console");
#endif

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

#ifdef QT_DEBUG
    toolBar->addAction(debugRun);
#endif

    QTimer::singleShot(0, this, [toolBar]()
    {
        toolBar->setFixedHeight(70);
    });
}

void AlgorithmBenchmarkWindow::registerAlgorithms()
{
    auto it = algorithms.insert("Graph", QList<Algorithm*>{});
    QList<Algorithm*>& algorithmsList = it.value();

    algorithmsList.append(new BFSIterative);
    algorithmsList.append(new BFSRecursive);

    algorithmsList.append(new DFSIterative);
    algorithmsList.append(new DFSRecursive);

    algorithmsList.append(new TreeCenters);

    algorithmsList.append(new TopologicalSort);
    algorithmsList.append(new KahnsAlgorithm);

    algorithmsList.append(new LazyDijkstraAlgorithm);

    for(auto* algorithm : algorithmsList)
    {
        connect(algorithm, &Algorithm::started, this, &AlgorithmBenchmarkWindow::onAlgorithmStarted);
        connect(algorithm, &Algorithm::finished, this, &AlgorithmBenchmarkWindow::onAlgorithmFinished);
    }
}

void AlgorithmBenchmarkWindow::endBenchmarkRun(bool deleteAllAlgorithms)
{
    for(auto it = algorithms.constBegin(); it != algorithms.constEnd(); ++it)
    {
        const QList<Algorithm*>& algorithmsList = it.value();
        for(auto* algorithm : algorithmsList)
        {
            algorithm->requestEnd();
        }
    }

    QThreadPool::globalInstance()->waitForDone();

    if(deleteAllAlgorithms)
    {
        for(auto it = algorithms.constBegin(); it != algorithms.constEnd(); ++it)
        {
            const QList<Algorithm*>& algorithmsList = it.value();
            qDeleteAll(algorithmsList);
        }
    }
}

Algorithm *AlgorithmBenchmarkWindow::getSelectedAlgorithm() const
{
    const QModelIndex modelIndex = algorithmsTreeView->selectionModel()->currentIndex();
    return modelIndex.isValid() ? qvariant_cast<Algorithm*>(modelIndex.data(Qt::UserRole)) : nullptr;
}

void AlgorithmBenchmarkWindow::saveSeriesToFile()
{
    auto series = chart->series();

    if(series.empty())
    {
        return;
    }

    QFile saveFile("benchmark series.txt");
    if(!saveFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QLineSeries* lastSeries = qobject_cast<QLineSeries*>(series[series.size() - 1]);

    QJsonObject infoAsJsonObject;
    infoAsJsonObject["info"] = seriesToToolTipText[lastSeries];

    QJsonArray resultArray;
    resultArray.append(infoAsJsonObject);

    const QList<QPointF> result = lastSeries->points();
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

    QLineSeries* newSeries = new QLineSeries;
    connect(newSeries, &QLineSeries::pressed, this, &AlgorithmBenchmarkWindow::onLineSeriesPressed);

    const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    const QJsonArray resultJsonArray = jsonDoc.array();

    const QString info = resultJsonArray[0].toObject()["info"].toString();
    seriesToToolTipText[newSeries] = info;

    newSeries->setName(info.left(info.indexOf('\n')) + "_" + QString::number(chart->series().size() + 1));

    for(int i = 1; i < resultJsonArray.size(); ++i)
    {
        const QJsonObject jsonObj = resultJsonArray[i].toObject();
        newSeries->append(QPointF(jsonObj["x"].toDouble(), jsonObj["y"].toDouble()));
    }

    addSeries(newSeries);
    chart->createDefaultAxes();
}

void AlgorithmBenchmarkWindow::clearSeriesSelection()
{
    for(auto it = seriesToLabel.constBegin(); it != seriesToLabel.constEnd(); ++it)
    {
        it.value()->deleteLater();

        QLineSeries* lineSeries = it.key();

        QPen pen = lineSeries->pen();
        pen.setWidth(2);
        lineSeries->setPen(pen);
    }

    seriesToLabel.clear();
}

void AlgorithmBenchmarkWindow::addSeries(QLineSeries* series)
{
    chart->addSeries(series);
    const auto markers = chart->legend()->markers(series);

    QLegendMarker* marker = markers[0];

    connect(marker, &QLegendMarker::clicked, this, &AlgorithmBenchmarkWindow::onLegendMarkerClicked);
    connect(marker, &QLegendMarker::hovered, this, &AlgorithmBenchmarkWindow::onLegendMarkerHovered);
}
