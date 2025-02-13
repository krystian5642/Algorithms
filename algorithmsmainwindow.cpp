#include "Algorithmsmainwindow.h"
#include "ui_Algorithmsmainwindow.h"
#include "graphwidget.h"
#include "graphAlgorithms.h"

#include <QFile>
#include <QJsonDocument>
#include <QComboBox>
#include <QTimer>

AlgorithmsMainWindow::AlgorithmsMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AlgorithmsMainWindow)
    , algorithm(nullptr)
{
    setupUi();
}

AlgorithmsMainWindow::~AlgorithmsMainWindow()
{
    delete ui;
}

void AlgorithmsMainWindow::paintEvent(QPaintEvent *event)
{
    ui->statusbar->showMessage(QString("Last paint time : %1 ms").arg(graphWidget->getLastPaintTime()));
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
    clearAlgorithm();
    graphWidget->clearGraph();

    if(loadGraph())
    {
        loadGraphNodeLocations();
        graphWidget->update();
    }
}

void AlgorithmsMainWindow::on_actionClear_triggered()
{
    clearAlgorithm();
    graphWidget->clearGraph();
}

void AlgorithmsMainWindow::on_actionRun_Algorithm_triggered()
{
    clearAlgorithm();
    graphWidget->SetNodesAndEdgesToBlack();

    algorithm = getAlgorithmToExecute();
    connect(algorithm, &GraphAlgorithm::onShowResultFinished, this, [this] ()
    {
        clearAlgorithm();
    });

    algorithm->execute();
    algorithm->showResult(graphWidget);
}

void AlgorithmsMainWindow::on_actionGenerateRandomGraph_triggered()
{
    clearAlgorithm();
    graphWidget->clearGraph();
    for(int i=0; i<=1000; i++)
    {
        const int randomValueX = QRandomGenerator::global()->bounded(600);
        const int randomValueY = QRandomGenerator::global()->bounded(600);

        const QPoint randomLocationX(QRandomGenerator::global()->bounded(10000), QRandomGenerator::global()->bounded(10000));
        const QPoint randomLocationY(QRandomGenerator::global()->bounded(10000), QRandomGenerator::global()->bounded(10000));

        graphWidget->addNode(randomValueX, randomLocationX);
        graphWidget->addNode(randomValueY, randomLocationY);

        graphWidget->addEdge(randomValueX, graphWidget->getGraph().getRandomValue());
        graphWidget->addEdge(randomValueY, graphWidget->getGraph().getRandomValue());
    }

    const int randomValueX = 1;
    const int randomValueY = 5;

    const QPoint randomLocationX(QRandomGenerator::global()->bounded(10000), QRandomGenerator::global()->bounded(10000));
    const QPoint randomLocationY(QRandomGenerator::global()->bounded(10000), QRandomGenerator::global()->bounded(10000));

    graphWidget->addNode(randomValueX, randomLocationX);
    graphWidget->addNode(randomValueY, randomLocationY);

    graphWidget->addEdge(randomValueX, graphWidget->getGraph().getRandomValue());
    graphWidget->addEdge(randomValueY, graphWidget->getGraph().getRandomValue());

    graphWidget->update();
}

void AlgorithmsMainWindow::on_actionGenerateRandomGridGraph_triggered()
{
    clearAlgorithm();
    graphWidget->clearGraph();

    // this info will be taken from UI later
    constexpr int columns = 90;
    constexpr int rows = 90;
    constexpr int nodeSpace = 50;   
    constexpr QPoint startLoc(50, 50);

    QList<int> prevRow(columns);
    for(int i = 0; i < rows; i++)
    {
        int prevValue = -1;
        for(int j = 0; j < columns; j++)
        {
            const int randomValue = QRandomGenerator::global()->bounded(10000);

            if(graphWidget->addNode(randomValue, QPoint(startLoc.x()  + nodeSpace * j, startLoc.y() + nodeSpace * i)))
            {
                if(j > 0 && prevValue != -1)
                {
                    graphWidget->addEdge(prevValue, randomValue);
                }

                if(i > 0 && prevRow[j] != -1)
                {
                    graphWidget->addEdge(prevRow[j], randomValue);
                }

                prevRow[j] = randomValue;

                prevValue = randomValue;
            }
            else
            {
                prevRow[j] = -1;
            }
        }
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

    AlgorithmComboBox = new QComboBox;
    AlgorithmComboBox->addItem("BFS");
    AlgorithmComboBox->addItem("DFS");
    AlgorithmComboBox->addItem("BFS Shortest Path");
    AlgorithmComboBox->setMaximumWidth(200);

    const QList<QAction*> actions = ui->toolBar->actions();
    ui->toolBar->insertWidget(actions[4], AlgorithmComboBox);

    graphWidget = new GraphWidget(this);
    setCentralWidget(graphWidget);
}

GraphAlgorithm* AlgorithmsMainWindow::getAlgorithmToExecute() const
{
    const auto currentText = AlgorithmComboBox->currentText();
    if(currentText == "BFS")
    {
        return new BFS(graphWidget->getGraph());
    }
    else if(currentText == "DFS")
    {
        return new DFS(graphWidget->getGraph());
    }
    else if(currentText == "BFS Shortest Path")
    {
        return new BFSShortestPath(graphWidget->getGraph());
    }
    return nullptr;
}

void AlgorithmsMainWindow::clearAlgorithm()
{
    if(algorithm)
    {
        algorithm->stop();
        algorithm->deleteLater();
        algorithm = nullptr;
    }
}
