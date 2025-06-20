
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
#include <QStack>
#include <QWidget>

GraphAlgorithm::GraphAlgorithm(QObject *parent)
    : Algorithm(parent)
    , graph(nullptr)
    , builderPropertiesWidget(nullptr)
{
    complexityList.push_back(qMakePair("O(1)",     [](int I, int V, int E) { return I; }));
    complexityList.push_back(qMakePair("O(V)",     [](int I, int V, int E) { return V; }));
    complexityList.push_back(qMakePair("O(E)",     [](int I, int V, int E) { return E; }));
    complexityList.push_back(qMakePair("O(V+E)",   [](int I, int V, int E) { return V + E; }));
    complexityList.push_back(qMakePair("O(2*V+E)", [](int I, int V, int E) { return 2*V + E; }));
    complexityList.push_back(qMakePair("O(V^2)",   [](int I, int V, int E) { return V * V; }));

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

    const QString startTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    emit started();

    int i = 0;
    for(; i < iterationsNumber; ++i)
    {
        if(requestedEnd)
        {
            break;
        }

        graphBuilder->buildIterations = i + 1;

        QScopedPointer<Graph> testGraph(dynamic_cast<Graph*>(graphBuilder->createDataStructure()));
        graph = testGraph.get();

        ULONG64 start;
        QueryThreadCycleTime(GetCurrentThread(), &start);

        execute();

        ULONG64 end;
        QueryThreadCycleTime(GetCurrentThread(), &end);

        const QPointF point(complexityFunc(i, testGraph->getVerticesNum(), testGraph->getEdgesNum()), end - start);
        result.append(point);
    }

    graph = nullptr;

    toolTipText.append("build iterations : " + QString::number(i));
    toolTipText.append("\n");

    toolTipText.append("start time : " + startTime);
    toolTipText.append("\n");

    toolTipText.append("end time : " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    toolTipText.append("\n");

    emit finished(result, toolTipText);
}

#ifdef QT_DEBUG
void GraphAlgorithm::debugRun()
{
    //GraphBuilder* graphBuilder = qobject_cast<GraphBuilder*>(getSelectedBuilder());
    //graphBuilder->buildIterations = 4;

    QScopedPointer<Graph> testGraph(dynamic_cast<Graph*>(new AdjacencyListGraph()));
    graph = testGraph.get();

    for(int i = 0; i < 5; ++i)
    {
        const_cast<Graph*>(graph)->addNode();
    }

    Graph* gg = const_cast<Graph*>(graph);
    gg->setIsDirected(true);
    gg->addEdge(0, 1);
    gg->addEdge(1, 3);
    gg->addEdge(2, 4);
    gg->addEdge(4, 3);

    testGraph->print();

    execute();
}
#endif

DataStructureBuilder *GraphAlgorithm::getSelectedBuilder() const
{
    return builderComboBox->currentData(Qt::UserRole).value<DataStructureBuilder*>();
}

BFSIterative::BFSIterative(QObject *parent)
    : GraphAlgorithm(parent)
{
    setObjectName("Breadth First Search (Iterative)");
}

void BFSIterative::execute()
{
    QQueue<int> nodeQueue;
    nodeQueue.reserve(graph->getVerticesNum());

    QList<bool> visited;
    visited.fill(false, graph->getVerticesNum());

    nodeQueue.enqueue(0);
    visited[0] = true;

    auto func = [&](int start, int neighbour, int weight)
    {
        if(!visited[neighbour])
        {
            visited[neighbour] = true;
            nodeQueue.enqueue(neighbour);
        }
        return true;
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

void BFSRecursive::execute()
{
    QQueue<int> nodeQueue;
    nodeQueue.reserve(graph->getVerticesNum());

    QList<bool> visited;
    visited.fill(false, graph->getVerticesNum());

    nodeQueue.enqueue(0);
    visited[0] = true;

    auto func = [&](int start, int neighbour, int weight)
    {
        if(!visited[neighbour])
        {
            visited[neighbour] = true;
            nodeQueue.enqueue(neighbour);
        }
        return true;
    };

    BFSRecursiveHelper(nodeQueue, func);
}

void BFSRecursive::BFSRecursiveHelper(QQueue<int>& nodeQueue, std::function<bool (int, int, int)> forEachNeighbourFunc)
{
    if(nodeQueue.isEmpty())
    {
        return;
    }

    const int first = nodeQueue.dequeue();
    graph->forEachNeighbor(first, forEachNeighbourFunc);
    BFSRecursiveHelper(nodeQueue, forEachNeighbourFunc);
}

DFSIterative::DFSIterative(QObject *parent)
    : GraphAlgorithm(parent)
{
    setObjectName("Depth First Search (Iterative)");
}

void DFSIterative::execute()
{
    QList<bool> visited;
    visited.fill(false, graph->getVerticesNum());
    visited[0] = true;

    QStack<int> nodesStack;
    nodesStack.reserve(graph->getVerticesNum());
    nodesStack.push(0);

    auto func = [&](int start, int neighbour, int weight)
    {
        if(!visited[neighbour])
        {
            visited[neighbour] = true;
            nodesStack.push(neighbour);
        }
        return true;
    };

    while(!nodesStack.isEmpty())
    {
        const int first = nodesStack.pop();
        graph->forEachNeighbor(first, func);
    }
}

DFSRecursive::DFSRecursive(QObject *parent)
    : GraphAlgorithm(parent)
{
    setObjectName("Depth First Search (Recursive)");
}

void DFSRecursive::execute()
{
    QList<bool> visited;
    visited.fill(false, graph->getVerticesNum());

    DFSHelper(0, visited);
}

void DFSRecursive::DFSHelper(int begin, QList<bool>& visited)
{
    if(!visited[begin])
    {
        visited[begin] = true;

        auto func = [&](int start, int neighbour, int weight)
        {
            if(!visited[neighbour])
            {
                DFSHelper(neighbour, visited);
            }
            return true;
        };

        graph->forEachNeighbor(begin, func);
    }
}

TreeCenters::TreeCenters(QObject *parent)
    : GraphAlgorithm(parent)
{
    setObjectName("Tree Centers");

    dataStructureBuilders.clear();
    dataStructureBuilders.push_back(new TreeGraphBuilder(this));
}

void TreeCenters::execute()
{
    const qsizetype verticesNum = graph->getVerticesNum();

    QList<qsizetype> nodeDegrees;
    nodeDegrees.reserve(verticesNum);

    QList<int> leafNodes;

    auto forEachNode = [&](int value)
    {
        nodeDegrees.push_back(graph->getNeighboursNum(value));

        if(nodeDegrees[value] == 0 || nodeDegrees[value] == 1)
        {
            leafNodes.push_back(value);
            nodeDegrees[value] = 0;
        }

        return true;
    };

    graph->forEachNode(forEachNode);

    qsizetype count = leafNodes.size();
    while(count < verticesNum)
    {
        QList<int> newLeafNodes;

        auto forEachNeighbour = [&](int value, int neighbour, int weight)
        {
            if(--nodeDegrees[neighbour] == 1)
            {
                newLeafNodes.push_back(neighbour);
            }
            return true;
        };

        for(int leafNode : leafNodes)
        {
            graph->forEachNeighbor(leafNode, forEachNeighbour);
        }

        count += newLeafNodes.size();

        leafNodes = newLeafNodes;
    }

    const QList<int> centers = leafNodes;
}

TopologicalSort::TopologicalSort(QObject *parent)
    : GraphAlgorithm(parent)
{
    setObjectName("Topological Sort (Recursive)");

    dataStructureBuilders.clear();
    dataStructureBuilders.push_back(new TreeGraphBuilder(this));
}

void TopologicalSort::execute()
{
    QList<bool> visited;
    visited.fill(false, graph->getVerticesNum());

    QStack<int> topologicalOrder;
    topologicalOrder.reserve(graph->getVerticesNum());

    auto forEachNode = [&](int value)
    {
        TopologicalSortHelper(value, visited, topologicalOrder);
        return true;
    };
    graph->forEachNode(forEachNode);

    qDebug() << "sdsad : ";
    qDebug() << "";
    while(!topologicalOrder.empty())
    {
        qDebug() << topologicalOrder.pop();
    }

}

void TopologicalSort::TopologicalSortHelper(int begin, QList<bool> &visited, QStack<int>& topologicalOrder)
{
    if(!visited[begin])
    {
        visited[begin] = true;

        auto forEachNeighbour = [&](int start, int neighbour, int weight)
        {
            if(!visited[neighbour])
            {
                TopologicalSortHelper(neighbour, visited, topologicalOrder);
            }
            return true;
        };

        graph->forEachNeighbor(begin, forEachNeighbour);
        topologicalOrder.push(begin);
    }
}
