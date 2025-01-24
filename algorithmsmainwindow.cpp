#include "algorithmsmainwindow.h"
#include "ui_algorithmsmainwindow.h"
#include "graphwidget.h"
#include "graphalgorithms.h"

#include <QFile>
#include <QJsonDocument>
#include <QComboBox>
#include <QTimer>

#include <memory>

AlgorithmsMainWindow::AlgorithmsMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AlgorithmsMainWindow)
{
    setupUi();
}

AlgorithmsMainWindow::~AlgorithmsMainWindow()
{
    delete ui;
}

void AlgorithmsMainWindow::on_actionSave_triggered()
{
    if(saveGraph())
    {
        saveGraphNodeLocations();
    }
}

void AlgorithmsMainWindow::on_actionLoad_triggered()
{
    if(loadGraph())
    {
        loadGraphNodeLocations();
        graphWidget->update();
    }
}

void AlgorithmsMainWindow::on_actionClear_triggered()
{
    graphWidget->clearGraph();
}

void AlgorithmsMainWindow::on_actionRun_algorithm_triggered()
{
    bool valueFound = false;
    const auto& graph = graphWidget->getGraph();
    const int randomValue = graph.getRandomValue(&valueFound);
    if(valueFound)
    {
        std::unique_ptr<GraphAlgorithm> bfs = std::make_unique<BFSGraphAlgorithm>();

        QList<int> visitedNodes;
        QSet<QPair<int, int>> visitedEdges;

        bfs->Execute(randomValue, graph, visitedNodes, visitedEdges);

        processedGraphAlgorithResult.nodeIt = visitedNodes.constBegin();
        processedGraphAlgorithResult.edgeIt = visitedEdges.constBegin();

        constexpr float paintInterval = 1.f;

        QTimer* paintNodeTimer = new QTimer;
        paintNodeTimer->setInterval(paintInterval);
        connect(paintNodeTimer, &QTimer::timeout, this, [this, visitedNodes, paintNodeTimer]()
        {
            graphWidget->setNodeColor(*processedGraphAlgorithResult.nodeIt, Qt::red);
            processedGraphAlgorithResult.nodeIt++;

            if(processedGraphAlgorithResult.nodeIt == visitedNodes.constEnd())
            {
                paintNodeTimer->stop();
                paintNodeTimer->deleteLater();
            }
        });

        QTimer* paintEdgeTimer = new QTimer;
        paintEdgeTimer->setInterval(paintInterval + 100);
        connect(paintEdgeTimer, &QTimer::timeout, this, [this, visitedEdges, paintEdgeTimer]()
        {
            graphWidget->setEdgeColor(processedGraphAlgorithResult.edgeIt->first, processedGraphAlgorithResult.edgeIt->second, Qt::red);
            processedGraphAlgorithResult.edgeIt++;

            if(processedGraphAlgorithResult.edgeIt == visitedEdges.constEnd())
            {
                paintEdgeTimer->stop();
                paintEdgeTimer->deleteLater();
            }
        });

        paintNodeTimer->start();
        paintEdgeTimer->start();
    }
}

void AlgorithmsMainWindow::on_actionGenerateRandomGraph_triggered()
{
    graphWidget->clearGraph();
    for(int i=0; i<=1000; i++)
    {
        const int randomValueX = QRandomGenerator::global()->bounded(600);
        const int randomValueY = QRandomGenerator::global()->bounded(600);

        const QPoint randomLocationX(QRandomGenerator::global()->bounded(1000), QRandomGenerator::global()->bounded(1000));
        const QPoint randomLocationY(QRandomGenerator::global()->bounded(1000), QRandomGenerator::global()->bounded(1000));

        graphWidget->addNode(randomValueX, randomLocationX);
        graphWidget->addNode(randomValueY, randomLocationY);

        graphWidget->addEdge(randomValueX, randomValueY);
    }
    graphWidget->update();
}

bool AlgorithmsMainWindow::saveGraph()
{
    QFile saveFile("graph.txt");
    if(!saveFile.open(QIODevice::WriteOnly))
    {
        return false;
    }

    const QJsonObject graphAsJsonObject = graphWidget->getGraph().toJsonObject();
    const QJsonDocument jsonDoc(graphAsJsonObject);

    saveFile.write(jsonDoc.toJson());
    saveFile.close();

    return true;
}

bool AlgorithmsMainWindow::loadGraph()
{
    QFile loadFile("graph.txt");
    if(!loadFile.open(QIODevice::ReadOnly))
    {
        return false;
    }

    const QByteArray jsonData = loadFile.readAll();
    loadFile.close();

    const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    graphWidget->getGraphMutable().fromJsonObject(jsonDoc.object());

    return true;
}

bool AlgorithmsMainWindow::saveGraphNodeLocations()
{
    QFile saveFile("graph visuals.txt");
    if(!saveFile.open(QIODevice::WriteOnly))
    {
        return false;
    }

    const QJsonDocument jsonDoc(graphWidget->toJsonObject());

    saveFile.write(jsonDoc.toJson());
    saveFile.close();

    return true;
}

bool AlgorithmsMainWindow::loadGraphNodeLocations()
{
    QFile loadFile("graph visuals.txt");
    if(!loadFile.open(QIODevice::ReadOnly))
    {
        return false;
    }

    const QByteArray jsonData = loadFile.readAll();
    loadFile.close();

    const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    const QJsonObject locationsAsJsonObject = jsonDoc.object();

    graphWidget->fromJsonObject(locationsAsJsonObject);

    return true;
}

void AlgorithmsMainWindow::setupUi()
{
    ui->setupUi(this);

    QComboBox* algorithmComboBox = new QComboBox;
    algorithmComboBox->setMaximumWidth(100);

    const QList<QAction*> actions = ui->toolBar->actions();
    ui->toolBar->insertWidget(actions[4], algorithmComboBox);

    graphWidget = new GraphWidget(this);
    setCentralWidget(graphWidget);
}
