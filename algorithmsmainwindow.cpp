#include "Algorithmsmainwindow.h"
#include "ui_Algorithmsmainwindow.h"
#include "graphwidget.h"
#include "graphAlgorithms.h"

#include <QMessageBox>
#include <QFile>
#include <QJsonDocument>
#include <QComboBox>
#include <QTimer>
#include <QElapsedTimer>
#include <QLineSeries>
#include <QChartView>

AlgorithmsMainWindow::AlgorithmsMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AlgorithmsMainWindow)
    , algorithm(nullptr)
    , algorithmExecutionTime(0)
    , hardRunWindow(nullptr)
{
    setupUi();
}

AlgorithmsMainWindow::~AlgorithmsMainWindow()
{
    delete ui;
}

void AlgorithmsMainWindow::paintEvent(QPaintEvent *event)
{
    ui->statusbar->showMessage(QString("Last paint time : %1 ms. Last algorithm execution time : %2 ms")
                                   .arg(graphWidget->getLastPaintTime())
                                   .arg(algorithmExecutionTime));
}

void AlgorithmsMainWindow::on_actionSave_triggered()
{
    const auto reply = QMessageBox::question(this, "Confirmation", "Do you want to save this graph and override the saved graph?"
                                             , QMessageBox::Yes | QMessageBox::No);

    if(reply == QMessageBox::Yes)
    {
        if(saveGraph())
        {
            saveGraphNodeLocations();
        }
    }
}

void AlgorithmsMainWindow::on_actionLoad_triggered()
{
    clearAlgorithm();
    graphWidget->clearGraph();

    if(loadGraph())
    {
        graphWidget->updateGraphProperites();
        loadGraphNodeLocations();
        graphWidget->update();
    }
}

void AlgorithmsMainWindow::on_actionClear_triggered()
{
    clearAlgorithm();
    graphWidget->clearGraph();
    graphWidget->updateGraphProperites();
}

void AlgorithmsMainWindow::on_actionRun_Algorithm_triggered(bool isOn)
{
    if(isOn)
    {
        if(algorithm)
        {
            algorithm->setPause(false);
        }
        else
        {
            clearAlgorithm();
            graphWidget->SetNodesAndEdgesToBlack();

            algorithm = getAlgorithmToExecute(graphWidget->getGraph());
            connect(algorithm, &GraphAlgorithm::onShowResultFinished, this, [this] ()
            {
                clearAlgorithm();
            });

            QElapsedTimer executionTime;
            executionTime.start();

            algorithm->execute();

            algorithmExecutionTime = executionTime.elapsed();

            algorithm->showResult(graphWidget);
        }
    }
    else if(algorithm)
    {
        algorithm->setPause(true);
    }
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
    graphWidget->updateGraphProperites();
}

void AlgorithmsMainWindow::on_actionGenerateRandomGridGraph_triggered()
{
    clearAlgorithm();
    graphWidget->clearGraph();

    // this info will be taken from UI later
    constexpr int columns = 20;
    constexpr int rows = 20;
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
    graphWidget->updateGraphProperites();
}

void AlgorithmsMainWindow::on_actionAlgorithm_hard_run_triggered()
{
    QLineSeries *series = new QLineSeries();
    series->setName("T(V+E)");

    Graph<int> testGraph;
    for(int i = 1; i < 1000; i++)
    {
        for(int j = 0; j < i; j++)
        {
            const int randomValueX = QRandomGenerator::global()->bounded(60000);
            const int randomValueY = QRandomGenerator::global()->bounded(60000);

            testGraph.addEdge(randomValueX, testGraph.getRandomValue());
            testGraph.addEdge(randomValueY, testGraph.getRandomValue());
        }

        GraphAlgorithm* testAlgorithm = getAlgorithmToExecute(testGraph);

        QElapsedTimer executionTime;
        executionTime.start();

        testAlgorithm->execute();

        series->append(testGraph.getEdgesNum() + testGraph.getNodesNum(), executionTime.nsecsElapsed());

        testGraph.clear();
        testAlgorithm->deleteLater();
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(algorithmComboBox->currentText());

    QFont titleFont;
    titleFont.setBold(true);
    titleFont.setPointSize(30);

    chart->setTitleFont(titleFont);
    chart->createDefaultAxes();

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    hardRunWindow = new QMainWindow(this);
    hardRunWindow->setCentralWidget(chartView);
    hardRunWindow->resize(1200, 800);
    hardRunWindow->show();
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

    algorithmComboBox->addItem("Breadth First Search");
    algorithmComboBox->addItem("Depth First Search");
    algorithmComboBox->addItem("Breadth First Search : Shortest Path");
    algorithmComboBox->addItem("Tree Centers");

    const QList<QAction*> actions = ui->toolBar->actions();
    ui->toolBar->insertWidget(actions[4], algorithmComboBox);

    graphWidget = new GraphWidget(this);
    setCentralWidget(graphWidget);
}

GraphAlgorithm* AlgorithmsMainWindow::getAlgorithmToExecute(const Graph<int>& graph) const
{
    const auto currentText = algorithmComboBox->currentText();
    if(currentText == "Breadth First Search")
    {
        return new BFS(graph);
    }
    else if(currentText == "Depth First Search")
    {
        return new DFS(graph);
    }
    else if(currentText == "Breadth First Search : Shortest Path")
    {
        return new BFSShortestPath(graph);
    }
    else if(currentText == "Tree Centers")
    {
        return new TreeCenters(graph);
    }
    return nullptr;
}

void AlgorithmsMainWindow::clearAlgorithm()
{
    if(algorithm)
    {
        algorithm->close();
        algorithm->deleteLater();
        algorithm = nullptr;
    }
}
