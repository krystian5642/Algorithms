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
    , paintGraphTimer(nullptr)
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
    clearPaintGraphTimer();

    if(loadGraph())
    {
        loadGraphNodeLocations();
        graphWidget->update();
    }
}

void AlgorithmsMainWindow::on_actionClear_triggered()
{
    clearPaintGraphTimer();
    graphWidget->clearGraph();
}

void AlgorithmsMainWindow::on_actionRun_algorithm_triggered()
{
    clearPaintGraphTimer();

    bool valueFound = false;
    const auto& graph = graphWidget->getGraph();
    const int randomValue = graph.getRandomValue(&valueFound);
    if(valueFound)
    {
        std::unique_ptr<GraphAlgorithm> algorithm = getAlgorithmToExecute();
        VisitedEdges visitedEdges;

        algorithm->Execute(randomValue, graph, visitedEdges);

        processedGraphAlgorithResult.edgeIt = visitedEdges.constBegin();

        graphWidget->setNodeColor(randomValue, Qt::red);

        paintGraphTimer = new QTimer;
        paintGraphTimer->setInterval(1000);
        connect(paintGraphTimer, &QTimer::timeout, this, [this, visitedEdges]()
        {
            graphWidget->setNodeColor(processedGraphAlgorithResult.edgeIt->first, Qt::red);
            graphWidget->setNodeColor(processedGraphAlgorithResult.edgeIt->second, Qt::red);

            graphWidget->setEdgeColor(processedGraphAlgorithResult.edgeIt->first, processedGraphAlgorithResult.edgeIt->second, Qt::red);
            processedGraphAlgorithResult.edgeIt++;

            if(processedGraphAlgorithResult.edgeIt == visitedEdges.constEnd())
            {
                clearPaintGraphTimer();
            }
        });

        paintGraphTimer->start();
    }
}

void AlgorithmsMainWindow::on_actionGenerateRandomGraph_triggered()
{
    clearPaintGraphTimer();
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

    algorithmComboBox = new QComboBox;
    algorithmComboBox->addItem("BFS");
    algorithmComboBox->addItem("DFS");
    algorithmComboBox->setMaximumWidth(100);

    const QList<QAction*> actions = ui->toolBar->actions();
    ui->toolBar->insertWidget(actions[4], algorithmComboBox);

    graphWidget = new GraphWidget(this);
    setCentralWidget(graphWidget);
}

void AlgorithmsMainWindow::clearPaintGraphTimer()
{
    if(paintGraphTimer)
    {
        paintGraphTimer->stop();
        paintGraphTimer->deleteLater();
        paintGraphTimer = nullptr;
    }
}

std::unique_ptr<GraphAlgorithm> AlgorithmsMainWindow::getAlgorithmToExecute() const
{
    const auto currentText = algorithmComboBox->currentText();
    if(currentText == "BFS")
    {
        return std::make_unique<BFSGraphAlgorithm>();
    }
    else if(currentText == "DFS")
    {
        return std::make_unique<DFSGraphAlgorithm>();
    }
    return nullptr;
}
