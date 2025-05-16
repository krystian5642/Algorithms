#include "graph_widget.h"

#include "../graph_algorithm_visualizers.h"
#include "random_graph_properties_dialog.h"

#include <QComboBox>
#include <QFile>
#include <QJsonDocument>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QRandomGenerator>
#include <QThreadPool>
#include <QVBoxLayout>

GraphWidget::GraphWidget(QWidget *parent)
    : DataStructureWidget(parent)
{
    category = "Graph";

    graph = std::make_unique<AdjacencyListGraph>();

    QAction* actionGenerateRandomEdges = new QAction(this);
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/icons/randomedges.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    actionGenerateRandomEdges->setIcon(icon);
    connect(actionGenerateRandomEdges, &QAction::triggered, this, &GraphWidget::onActionGenerateRandomEdgesTriggered);

#if QT_CONFIG(tooltip)
    actionGenerateRandomEdges->setToolTip("Generate random edges");
#endif

    additionalActions.push_back(actionGenerateRandomEdges);
}

GraphWidget::~GraphWidget()
{
}

void GraphWidget::clear()
{   
    clearVisualization();
    graph->clear();
    graphNodeVisualData.clear();
    graphEdgeVisualData.clear();  
    update();
}

QJsonObject GraphWidget::toJsonObject() const
{
    QJsonObject locationsAsJsonObject;
    for(auto it = graphNodeVisualData.constBegin(); it != graphNodeVisualData.constEnd(); it++)
    {
        QJsonObject locationAsJsonObject;
        locationAsJsonObject["x"] = it->location.x();
        locationAsJsonObject["y"] = it->location.y();
    }
    return locationsAsJsonObject;
}

void GraphWidget::fromJsonObject(const QJsonObject &jsonObj)
{
    graphNodeVisualData.clear();
    for (auto it = jsonObj.constBegin(); it != jsonObj.constEnd(); ++it)
    {
        const QJsonObject jsonObject = it.value().toObject();
        const QPoint graphNodeLocation(jsonObject["x"].toInt(), jsonObject["y"].toInt());

        graphNodeVisualData[it.key().toInt()].location = graphNodeLocation;
    }
    update();
}

void GraphWidget::setNodeColor(int value, const QColor& color, bool callUpdate)
{
    if(graphNodeVisualData.size() > value)
    {
        graphNodeVisualData[value].color = color;

        if(callUpdate)
        {
            update();
        }
    }
}

void GraphWidget::setEdgeColor(int start, int end, const QColor &color, bool callUpdate)
{
    const QPair<int, int> edge(qMin(start, end), qMax(start, end));
    auto it = graphEdgeVisualData.find(edge);
    if(it != graphEdgeVisualData.end() && it->color != color)
    {
        it->color = color;

        if(callUpdate)
        {
            update();
        }
    }
}

void GraphWidget::setNodesAndEdgesToBlack()
{
    for(auto it = graphNodeVisualData.begin(); it != graphNodeVisualData.end(); it++)
    {
        if(it->color != Qt::black)
        {
            it->color = Qt::black;
        }
    }

    for(auto it = graphEdgeVisualData.begin(); it != graphEdgeVisualData.end(); it++)
    {
        if(it->color != Qt::black)
        {
            it->color = Qt::black;
        }
    }

    update();
}

bool GraphWidget::addNode(const QPoint& location)
{
    graph->addNode();

    graphNodeVisualData.resize(graphNodeVisualData.size() + 1);

    graphNodeVisualData[graphNodeVisualData.size() - 1].location = location;
    return true;
}

void GraphWidget::addEdge(int start, int end, int weight)
{
    graph->addEdge(start, end, weight);
}

void GraphWidget::saveAction()
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

void GraphWidget::loadAction()
{
    clear();

    if(loadGraph())
    {
        loadGraphNodeLocations();
        update();
    }
}

void GraphWidget::clearAction()
{
    clear();
}

void GraphWidget::generateRandomDataStructureAction()
{
    RandomGraphPropertiesDialog randomGraphProperitiesDialog;
    randomGraphProperitiesDialog.exec();

    if(randomGraphProperitiesDialog.result() == QDialog::DialogCode::Accepted)
    {
        clear();

        const QString selectedGraphTypeName = randomGraphProperitiesDialog.getSelectedGraphTypeName();

        if(selectedGraphTypeName == GraphTypeNames::AllRandom)
        {
            generateRandomGraph();
        }
        else if(selectedGraphTypeName == GraphTypeNames::Grid)
        {
            generateRandomGridGraph();
        }

        update();
    }
}

void GraphWidget::visualizeAlgorithmAction(AlgorithmVisualizer* algorithmVisualizer, bool pause)
{
    if(!pause && algorithmVisualizer)
    {
        if(currentAlgorithmVisualizer)
        {
            currentAlgorithmVisualizer->setPause(false);
        }
        else
        {
            setNodesAndEdgesToBlack();

            GraphAlgorithmVisualizer* graphAlgorithmVisualizer = qobject_cast<GraphAlgorithmVisualizer*>(algorithmVisualizer);
            graphAlgorithmVisualizer->clear();
            graphAlgorithmVisualizer->setGraph(graph.get());

            currentAlgorithmVisualizer = graphAlgorithmVisualizer;
            connect(currentAlgorithmVisualizer, &AlgorithmVisualizer::finished, this, &GraphWidget::onAlgorithmVisualizerFinished);

            currentAlgorithmVisualizer->run(this);
        }
    }
    else if(currentAlgorithmVisualizer)
    {
        currentAlgorithmVisualizer->setPause(true);
    }
}

void GraphWidget::registerAlgorithmVisualizers()
{
    algorithmVisualizers.append(new BFSVisualizer(this));
    algorithmVisualizers.append(new DFSVisualizer(this));
}

void GraphWidget::onActionGenerateRandomEdgesTriggered()
{
    clearVisualization();
    graph->generateRandomEdges(0.2);
    update();
}

void GraphWidget::onAlgorithmVisualizerFinished()
{
    disconnect(currentAlgorithmVisualizer, &AlgorithmVisualizer::finished, this, &GraphWidget::onAlgorithmVisualizerFinished);
    currentAlgorithmVisualizer = nullptr;
}

void GraphWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        addNode(event->pos());
        update();
    }
    else
    {
        DataStructureWidget::mousePressEvent(event);
    }
}

void GraphWidget::paintDataStructure(QPainter &painter)
{
    painter.save();

    paintEdges(painter);
    paintNodes(painter);
    paintNodeValues(painter);

    painter.restore();
}

void GraphWidget::paintEdges(QPainter &painter)
{
    QSet<QPair<int, int>> drawnEdges;
    drawnEdges.reserve(qMax(0, graph->getEdgesNum()));

    auto func = [&](int start, int end, int weight)
    {
        const QPair<int, int> edge(qMin(start, end), qMax(start, end));
        if(!drawnEdges.contains(edge))
        {
            QPen edgePen;
            edgePen.setBrush(graphEdgeVisualData[edge].color);
            painter.setPen(edgePen);

            painter.drawLine(graphNodeVisualData[start].location, graphNodeVisualData[end].location);
            drawnEdges.insert(edge);
        }
    };
    graph->forEachEdge(func);
}

void GraphWidget::paintNodes(QPainter &painter)
{
    auto func = [&](int value)
    {
        QPen nodePen;
        nodePen.setBrush(graphNodeVisualData[value].color);
        painter.setPen(nodePen);
        painter.setBrush(graphNodeVisualData[value].color);

        painter.drawEllipse(graphNodeVisualData[value].location, 15, 15);
    };
    graph->forEachNode(func);
}

void GraphWidget::paintNodeValues(QPainter &painter)
{
    QPen textPen;
    textPen.setBrush(Qt::white);
    painter.setPen(textPen);

    auto func = [&](int value)
    {
        painter.drawText(graphNodeVisualData[value].location + QPointF(-3.5, 3), QString("%1").arg(value));
    };
    graph->forEachNode(func);
}

void GraphWidget::clearVisualization()
{
    if(currentAlgorithmVisualizer)
    {
        currentAlgorithmVisualizer->clear();
    }
    setNodesAndEdgesToBlack();
}

bool GraphWidget::saveGraph()
{
    // QFile saveFile("graph.txt");
    // if(!saveFile.open(QIODevice::WriteOnly))
    // {
    //     return false;
    // }

    // const QJsonObject graphAsJsonObject = graph.toJsonObject();
    // const QJsonDocument jsonDoc(graphAsJsonObject);

    // saveFile.write(jsonDoc.toJson());
    // saveFile.close();

    return true;
}

bool GraphWidget::loadGraph()
{
    // QFile loadFile("graph.txt");
    // if(!loadFile.open(QIODevice::ReadOnly))
    // {
    //     return false;
    // }

    // const QByteArray jsonData = loadFile.readAll();
    // loadFile.close();

    // const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    // graph.fromJsonObject(jsonDoc.object());

    return true;
}

bool GraphWidget::saveGraphNodeLocations()
{
    QFile saveFile("graph visuals.txt");
    if(!saveFile.open(QIODevice::WriteOnly))
    {
        return false;
    }

    const QJsonDocument jsonDoc(toJsonObject());

    saveFile.write(jsonDoc.toJson());
    saveFile.close();

    return true;
}

bool GraphWidget::loadGraphNodeLocations()
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

    fromJsonObject(locationsAsJsonObject);

    return true;
}

void GraphWidget::generateRandomGraph()
{
    // for(int i=0; i<=1000; i++)
    // {
    //     const int randomValueX = QRandomGenerator::global()->bounded(600);
    //     const int randomValueY = QRandomGenerator::global()->bounded(600);

    //     const QPoint randomLocationX(QRandomGenerator::global()->bounded(10000), QRandomGenerator::global()->bounded(10000));
    //     const QPoint randomLocationY(QRandomGenerator::global()->bounded(10000), QRandomGenerator::global()->bounded(10000));

    //     addNode(randomValueX, randomLocationX);
    //     addNode(randomValueY, randomLocationY);

    //     addEdge(randomValueX, graph.getRandomValue());
    //     addEdge(randomValueY, graph.getRandomValue());
    // }
}

void GraphWidget::generateRandomGridGraph()
{
    // // this info will be taken from UI later
    // constexpr int columns = 20;
    // constexpr int rows = 20;
    // constexpr int nodeSpace = 50;
    // constexpr QPoint startLoc(50, 50);

    // QList<int> prevRow(columns);
    // for(int i = 0; i < rows; i++)
    // {
    //     int prevValue = -1;
    //     for(int j = 0; j < columns; j++)
    //     {
    //         const int randomValue = QRandomGenerator::global()->bounded(10000);

    //         if(addNode(randomValue, QPoint(startLoc.x()  + nodeSpace * j, startLoc.y() + nodeSpace * i)))
    //         {
    //             if(j > 0 && prevValue != -1)
    //             {
    //                 addEdge(prevValue, randomValue);
    //             }

    //             if(i > 0 && prevRow[j] != -1)
    //             {
    //                 addEdge(prevRow[j], randomValue);
    //             }

    //             prevRow[j] = randomValue;

    //             prevValue = randomValue;
    //         }
    //         else
    //         {
    //             prevRow[j] = -1;
    //         }
    //     }
    // }
}
