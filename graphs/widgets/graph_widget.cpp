#include "graph_widget.h"

#include "../../core/property_editor_factory.h"

#include "../graph_algorithm_visualizers.h"
#include "../graph_visual_builders.h"
#include "../graph_visualization_settings.h"

#include "add_edge_dialog.h"
#include "remove_edge_dialog.h"
#include "random_graph_properties_dialog.h"

#include <QComboBox>
#include <QElapsedTimer>
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
    , nodeRadius(15.0)
    , arrowSize(10.0)
    , arrowAngle(M_PI / 6.0)
{
    category = "Graph";

    graph = std::make_unique<AdjacencyListGraph>(true);

    graphVisualizationSettings = new GraphVisualizationSettings(this);

    connect(graphVisualizationSettings, &GraphVisualizationSettings::showWeightsChanged, this, [this](bool showWeights)
    {
        update();
    });

    connect(graphVisualizationSettings, &GraphVisualizationSettings::graphDirectedChanged, this, [this](bool graphDirected)
    {
        graph->setIsDirected(graphDirected);
        update();
    });

    QAction* actionGenerateRandomEdges = new QAction(this);
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/icons/random_edges.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    actionGenerateRandomEdges->setIcon(icon);
    connect(actionGenerateRandomEdges, &QAction::triggered, this, &GraphWidget::onActionGenerateRandomEdgesTriggered);

    QAction* actionAddEdge = new QAction(this);
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/icons/add_edge.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    actionAddEdge->setIcon(icon1);
    connect(actionAddEdge, &QAction::triggered, this, &GraphWidget::onActionAddEdgeTriggered);

    QAction* actionRemoveEdge = new QAction(this);
    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/icons/remove_edge.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    actionRemoveEdge->setIcon(icon3);
    connect(actionRemoveEdge, &QAction::triggered, this, &GraphWidget::onActionRemoveEdgeTriggered);

#if QT_CONFIG(tooltip)
    actionGenerateRandomEdges->setToolTip("Generate random edges");
    actionAddEdge->setToolTip("Add edge");
#endif

    additionalActions.push_back(actionGenerateRandomEdges);
    additionalActions.push_back(actionAddEdge);
    additionalActions.push_back(actionRemoveEdge);

    dataStructureVisualBuilders.push_back(new GeneralGraphVisualBuilder(this));
    dataStructureVisualBuilders.push_back(new GridGraphVisualBuilder(this));

    QWidget* propertiesWidget = PropertyEditorFactory::get().createPropertiesWidget(graphVisualizationSettings, this);
    propertiesWidget->setFixedWidth(170);

    additionalToolBarWidgets.push_back(propertiesWidget);
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
    for(int i = 0; i < graphNodeVisualData.size(); ++i)
    {
        QJsonObject locationAsJsonObject;
        locationAsJsonObject["x"] = graphNodeVisualData[i].location.x();
        locationAsJsonObject["y"] = graphNodeVisualData[i].location.y();
        locationsAsJsonObject[QString::number(i)] = locationAsJsonObject;
    }
    return locationsAsJsonObject;
}

void GraphWidget::fromJsonObject(const QJsonObject &jsonObj)
{
    graphNodeVisualData.clear();
    graphNodeVisualData.resize(jsonObj.size());

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

void GraphWidget::generateRandomEdges(const double addEdgePropability)
{
    graph->generateRandomEdges(addEdgePropability);
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
    randomGraphProperitiesDialog.setGraphBuilders(dataStructureVisualBuilders);

    const int result = randomGraphProperitiesDialog.exec();

    if(result == QDialog::DialogCode::Accepted)
    {
        DataStructureVisualBuilder* graphBuilder = randomGraphProperitiesDialog.getSelectedBuidler();
        graphBuilder->buildDataStructureVisualization(this);

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
    generateRandomEdges(0.2);
}

void GraphWidget::onActionAddEdgeTriggered()
{
    AddEdgeDialog addEdgeDialog(graph.get());
    const int result = addEdgeDialog.exec();

    if(result == QDialog::DialogCode::Accepted)
    {
        graph->addEdge(addEdgeDialog.getStart(), addEdgeDialog.getEnd(), addEdgeDialog.getWeight());

        update();
    }
}

void GraphWidget::onActionRemoveEdgeTriggered()
{
    RemoveEdgeDialog removeEdgeDialog(graph.get());
    const int result = removeEdgeDialog.exec();

    if(result == QDialog::DialogCode::Accepted)
    {
        graph->removeEdge(removeEdgeDialog.getStart(), removeEdgeDialog.getEnd());

        update();
    }
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

    {
        QElapsedTimer paintTimer;
        paintTimer.start();

        paintEdges(painter);
        paintNodes(painter);
        paintNodeValues(painter);

        if(graphVisualizationSettings->getShowWeights())
        {
            paintWeights(painter);
        }

        lastPaintTime = paintTimer.elapsed();
    }

    painter.restore();
}

void GraphWidget::paintEdges(QPainter &painter)
{
    QSet<QPair<int, int>> drawnEdges;
    drawnEdges.reserve(graph->getEdgesNum());

    auto func = [&](int start, int end, int weight)
    {
        const QPair<int, int> edge(qMin(start, end), qMax(start, end));
        if(!drawnEdges.contains(edge))
        {
            const QColor color = graphEdgeVisualData[edge].color;
            painter.setPen(QPen(color));
            painter.setBrush(color);

            const GraphNodeVisualData& startNodeVisualData = graphNodeVisualData[start];
            const GraphNodeVisualData& endNodeVisualData = graphNodeVisualData[end];

            QVector2D toNodeCenter(endNodeVisualData.location - startNodeVisualData.location);
            const float lenght = toNodeCenter.length();
            const QPointF end = startNodeVisualData.location + toNodeCenter.normalized().toPointF() * (lenght - nodeRadius);

            painter.drawLine(startNodeVisualData.location, end);
            drawnEdges.insert(edge);

            if(graph->getIsDirected())
            {
                const QLineF line(end, startNodeVisualData.location);
                const double angle = std::atan2(line.dy(), line.dx());

                const QPointF arrowP1 = end + QPointF(arrowSize * std::cos(angle + arrowAngle), arrowSize * std::sin(angle + arrowAngle));
                const QPointF arrowP2 = end + QPointF(arrowSize * std::cos(angle - arrowAngle), arrowSize * std::sin(angle - arrowAngle));

                painter.drawPolygon(QPolygonF({end, arrowP1, arrowP2}));
            }
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

        painter.drawEllipse(graphNodeVisualData[value].location, nodeRadius, nodeRadius);
    };
    graph->forEachNode(func);
}

void GraphWidget::paintNodeValues(QPainter &painter)
{
    QPen textPen;
    textPen.setBrush(Qt::white);
    painter.setPen(textPen);
    QFont font("Arial", 12);
    painter.setFont(font);

    auto func = [&](int value)
    {
        const QString valueText(QString::number(value));

        QFontMetrics fontMetrics(font);
        const int textWidth = fontMetrics.horizontalAdvance(valueText);
        const int textHeight = fontMetrics.height();

        const QPointF& center = graphNodeVisualData[value].location;
        const QRectF backgroundRect(center.x() - (textWidth / 2)
                                    , center.y() - (textHeight / 2)
                                    , textWidth
                                    , textHeight);

        painter.drawText(backgroundRect, Qt::AlignCenter, valueText);
    };
    graph->forEachNode(func);
}

void GraphWidget::paintWeights(QPainter &painter)
{
    QPen textPen;
    textPen.setBrush(Qt::blue);
    QFont font("Arial", 12);
    painter.setFont(font);
    painter.setBrush(QColor(255, 165, 0)); // orange

    constexpr int padding = 5;

    auto func = [&](int start, int end, int weight)
    {
        const QString weightText(QString::number(weight));

        QFontMetrics fontMetrics(font);
        const int textWidth = fontMetrics.horizontalAdvance(weightText);
        const int textHeight = fontMetrics.height();

        const QPointF center((graphNodeVisualData[start].location + graphNodeVisualData[end].location) / 2.0);
        const QRectF backgroundRect(center.x() - (textWidth / 2) - padding
                                    , center.y() - (textHeight / 2) - padding
                                    , textWidth + 2 * padding
                                    , textHeight + 2 * padding);

        painter.setPen(Qt::NoPen);
        painter.drawRect(backgroundRect);

        painter.setPen(textPen);
        painter.drawText(backgroundRect, Qt::AlignCenter, weightText);
    };
    graph->forEachEdge(func);
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
    QFile saveFile("graph.txt");
    if(!saveFile.open(QIODevice::WriteOnly))
    {
        return false;
    }

    const QJsonObject graphAsJsonObject = graph->toJsonObject();
    const QJsonDocument jsonDoc(graphAsJsonObject);

    saveFile.write(jsonDoc.toJson());
    saveFile.close();

    return true;
}

bool GraphWidget::loadGraph()
{
    QFile loadFile("graph.txt");
    if(!loadFile.open(QIODevice::ReadOnly))
    {
        return false;
    }

    const QByteArray jsonData = loadFile.readAll();
    loadFile.close();

    const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    graph->fromJsonObject(jsonDoc.object());

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
