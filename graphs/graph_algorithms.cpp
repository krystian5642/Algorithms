
#include "graph_algorithms.h"
#include "graph.h"

#include "../core/property_editor_factory.h"

#include "graph_builders.h"

#include <windows.h>

#include <QBoxLayout>
#include <QComboBox>
#include <QElapsedTimer>
#include <QFormLayout>
#include <QLabel>
#include <QRandomGenerator>
#include <QWidget>

GraphAlgorithm::GraphAlgorithm(QObject *parent)
    : Algorithm(parent)
    , graph(nullptr)
    , builderPropertiesWidget(nullptr)
{
    dataStructureBuilders.push_back(new GeneralGraphBuilder(this));
    dataStructureBuilders.push_back(new GridGraphBuilder(this));
}

GraphAlgorithm::~GraphAlgorithm()
{

}

QWidget *GraphAlgorithm::createPropertiesWidget(QWidget *parent)
{
    builderPropertiesWidget = nullptr;

    QWidget* propertiesWidget = Algorithm::createPropertiesWidget(parent);

    QVBoxLayout* verticalLayout = qobject_cast<QVBoxLayout*>(propertiesWidget->layout());
    QFormLayout* formLayout = propertiesWidget->findChild<QFormLayout*>();

    builderComboBox = new QComboBox(propertiesWidget);

    for(const auto* builder : dataStructureBuilders)
    {
        builderComboBox->addItem(builder->objectName(), QVariant::fromValue(builder));
    }

    formLayout->addRow("builder", builderComboBox);

    auto onBuilderSelected = [this, verticalLayout, parent](const QString& currentText)
    {
        if(builderPropertiesWidget)
        {
            verticalLayout->removeWidget(builderPropertiesWidget);
            builderPropertiesWidget->deleteLater();
            builderPropertiesWidget = nullptr;
        }

        PropertyEditorFactory& propertyEditorFactory = PropertyEditorFactory::get();

        builderPropertiesWidget = getSelectedBuilder()->createPropertiesWidget(parent);

        verticalLayout->addWidget(builderPropertiesWidget);
    };

    connect(builderComboBox, &QComboBox::currentTextChanged, this, onBuilderSelected);

    onBuilderSelected(builderComboBox->currentText());

    return propertiesWidget;
}

void GraphAlgorithm::appendPropertiesInfo(QString &infoText)
{
    Algorithm::appendPropertiesInfo(infoText);

    infoText.append("builder : " + builderComboBox->currentText());
    infoText.append("\n");
}

void GraphAlgorithm::run()
{
    requestedEnd = false;
    emit started();

    QList<QPointF> result;
    result.reserve(iterationsNumber);

    QString toolTipText;
    appendPropertiesInfo(toolTipText);

    GraphBuilder* graphBuilder = qobject_cast<GraphBuilder*>(getSelectedBuilder());
    graphBuilder->appendPropertiesInfo(toolTipText);

    auto complexityFunc = std::find_if(complexityList.begin(), complexityList.end(), [&](const ComplexityNameToFunction& pair)
    {
        return pair.first == selectedComplexity;
    })->second;

    int i = 0;
    for(; i < iterationsNumber; ++i)
    {
        if(requestedEnd)
        {
            clear();
            break;
        }

        graphBuilder->buildIterations = i + 1;

        QScopedPointer<Graph> testGraph(dynamic_cast<Graph*>(graphBuilder->createDataStructure()));
        setGraph(testGraph.get());
        init();

        ULONG64 start;
        QueryThreadCycleTime(GetCurrentThread(), &start);

        execute();

        ULONG64 end;
        QueryThreadCycleTime(GetCurrentThread(), &end);

        const QPointF point(complexityFunc(i, testGraph->getVerticesNum(), testGraph->getEdgesNum()), end - start);
        result.append(point);

        clear();
    }

    toolTipText.append("build iterations : " + QString::number(i));
    toolTipText.append("\n");

    toolTipText.append("start time : " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    toolTipText.append("\n");

    emit finished(result, toolTipText);
}

#ifdef QT_DEBUG
void GraphAlgorithm::debugRun()
{
    GraphBuilder* graphBuilder = qobject_cast<GraphBuilder*>(getSelectedBuilder());
    graphBuilder->buildIterations = 4;

    QScopedPointer<Graph> testGraph(dynamic_cast<Graph*>(graphBuilder->createDataStructure()));
    setGraph(testGraph.get());
    init();

    qDebug().noquote() << testGraph->print();

    execute();
}
#endif

DataStructureBuilder *GraphAlgorithm::getSelectedBuilder() const
{
    return builderComboBox->currentData(Qt::UserRole).value<DataStructureBuilder*>();
}

void GraphAlgorithm::setGraph(Graph *newGraph)
{
    graph = newGraph;
}

BFSIterative::BFSIterative(QObject *parent)
    : GraphAlgorithm(parent)
{
    setObjectName("Breadth First Search (Iterative)");
}

void BFSIterative::init()
{
    GraphAlgorithm::init();

    nodeQueue.reserve(graph->getVerticesNum());
    visited.fill(false, graph->getVerticesNum());
}

void BFSIterative::clear()
{
    GraphAlgorithm::clear();

    nodeQueue.clear();
    visited.clear();
}

void BFSIterative::execute()
{
    nodeQueue.enqueue(0);
    visited[0] = true;

    auto func = [&](int start, int neighbour, int weight)
    {
        if(!visited[neighbour])
        {
            visited[neighbour] = true;
            nodeQueue.enqueue(neighbour);
        }
    };

    while(!nodeQueue.empty())
    {
        const int first = nodeQueue.dequeue();
        graph->forEachNeighbor(first, func);
    }
}

BFSRecursive::BFSRecursive(QObject *parent)
    : GraphAlgorithm(parent)
{
    setObjectName("Breadth First Search (Recursive)");
}

void BFSRecursive::init()
{
    GraphAlgorithm::init();

    nodeQueue.reserve(graph->getVerticesNum());
    visited.fill(false, graph->getVerticesNum());
}

void BFSRecursive::clear()
{
    GraphAlgorithm::clear();

    nodeQueue.clear();
    visited.clear();
}

void BFSRecursive::execute()
{
    nodeQueue.enqueue(0);
    visited[0] = true;

    auto func = [&](int start, int neighbour, int weight)
    {
        if(!visited[neighbour])
        {
            visited[neighbour] = true;
            nodeQueue.enqueue(neighbour);
        }
    };

    BFSRecursiveHelper(func);
}

void BFSRecursive::BFSRecursiveHelper(std::function<void (int, int, int)> forEachNeighbourFunc)
{
    if(nodeQueue.isEmpty())
    {
        return;
    }

    const int first = nodeQueue.dequeue();
    graph->forEachNeighbor(first, forEachNeighbourFunc);
    BFSRecursiveHelper(forEachNeighbourFunc);
}

DFSRecursive::DFSRecursive(QObject *parent)
    : GraphAlgorithm(parent)
{
    setObjectName("Depth First Search (Recursive)");
}

void DFSRecursive::execute()
{
    // const int start = graph->getFirstValue();

    // QSet<int> visited;
    // visited.reserve(graph->getNodesNum());
    // visited.insert(start);

    // DFSHelper(start, visited);
}

void DFSRecursive::DFSHelper(int begin, QSet<int>& visited)
{
    // const QList<GraphEdge>& neighbourEdges = graph->getNeighbourEdges(begin);
    // for(const auto& neighbour : neighbourEdges)
    // {
    //     const int value = neighbour.getEndValue();
    //     if(!visited.contains(value))
    //     {
    //         visited.insert(value);
    //         DFSHelper(value, visited);
    //     }
    // }
}
