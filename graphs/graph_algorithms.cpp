
#include "graph_algorithms.h"
#include "graph.h"

#include "../core/property_editor_factory.h"
#include "../core/utils.h"

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
    complexityList.push_back(qMakePair("O(1)",         [](int I, int V, int E) { return I; }));
    complexityList.push_back(qMakePair("O(V)",         [](int I, int V, int E) { return V; }));
    complexityList.push_back(qMakePair("O(E)",         [](int I, int V, int E) { return E; }));
    complexityList.push_back(qMakePair("O(V+E)",       [](int I, int V, int E) { return V + E; }));
    complexityList.push_back(qMakePair("O(2V+E)",      [](int I, int V, int E) { return 2*V + E; }));
    complexityList.push_back(qMakePair("O(V^2)",       [](int I, int V, int E) { return V * V; }));
    complexityList.push_back(qMakePair("O(V+E)logV",   [](int I, int V, int E) { return (V + E)*std::log(V); }));
    complexityList.push_back(qMakePair("O(VE)",        [](int I, int V, int E) { return (V * E); }));

    dataStructureBuilders.push_back(new GeneralGraphBuilder(this));
    dataStructureBuilders.push_back(new GridGraphBuilder(this));
    dataStructureBuilders.push_back(new TreeGraphBuilder(this));
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

        const QPointF point(complexityFunc(i, testGraph->getNodesNum(), testGraph->getEdgesNum()), end - start);
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
    nodeQueue.reserve(graph->getNodesNum());

    QList<bool> visited;
    visited.fill(false, graph->getNodesNum());

    nodeQueue.enqueue(0);
    visited[0] = true;

    auto forEachNeighbour = [&](int start, int neighbour, int weight)
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
        graph->forEachNeighbour(first, forEachNeighbour);
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
    nodeQueue.reserve(graph->getNodesNum());

    QList<bool> visited;
    visited.fill(false, graph->getNodesNum());

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
    graph->forEachNeighbour(first, forEachNeighbourFunc);
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
    visited.fill(false, graph->getNodesNum());
    visited[0] = true;

    QStack<int> nodesStack;
    nodesStack.reserve(graph->getNodesNum());
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
        graph->forEachNeighbour(first, func);
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
    visited.fill(false, graph->getNodesNum());

    DFSHelper(0, visited);
}

void DFSRecursive::DFSHelper(int begin, QList<bool>& visited)
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

    graph->forEachNeighbour(begin, func);
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
    const qsizetype nodesNum = graph->getNodesNum();

    QList<int> nodeDegrees = graph->getNodeDegrees();

    QList<int> leafNodes;

    auto forEachNode = [&](int value)
    {
        if(nodeDegrees[value] == 0 || nodeDegrees[value] == 1)
        {
            leafNodes.push_back(value);
            nodeDegrees[value] = 0;
        }

        return true;
    };

    graph->forEachNode(forEachNode);

    qsizetype count = leafNodes.size();
    while(count < nodesNum)
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
            graph->forEachNeighbour(leafNode, forEachNeighbour);
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
    visited.fill(false, graph->getNodesNum());

    QStack<int> topologicalOrder;
    topologicalOrder.reserve(graph->getNodesNum());

    auto forEachNode = [&](int value)
    {
        TopologicalSortHelper(value, visited, topologicalOrder);
        return true;
    };
    graph->forEachNode(forEachNode);
}

void TopologicalSort::TopologicalSortHelper(int begin, QList<bool> &visited, QStack<int>& topologicalOrder)
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

    graph->forEachNeighbour(begin, forEachNeighbour);
    topologicalOrder.push(begin);
}

KahnsAlgorithm::KahnsAlgorithm(QObject *parent)
    : GraphAlgorithm(parent)
{
    setObjectName("Kahn's Algorithm (Topological Sort)");
}

void KahnsAlgorithm::execute()
{
    QList<int> nodeDegrees = graph->getNodeDegrees();

    QList<int> topologicalOrder;
    topologicalOrder.reserve(graph->getNodesNum());

    QQueue<int> nodeQueue;

    auto forEachNode = [&](int value)
    {
        if(nodeDegrees[value] == 0)
        {
            nodeQueue.enqueue(value);
        }

        return true;
    };

    graph->forEachNode(forEachNode);

    auto forEachNeighbour = [&](int value, int neighbour, int weight)
    {
        if(--nodeDegrees[neighbour] == 0)
        {
            nodeQueue.enqueue(neighbour);
        }
        return true;
    };

    while(!nodeQueue.empty())
    {
        const int first = nodeQueue.dequeue();
        topologicalOrder.push_back(first);
        graph->forEachNeighbour(first, forEachNeighbour);
    }

    if(topologicalOrder.size() < graph->getNodesNum())
    {
        topologicalOrder.clear(); // cycle
    }

}

LazyDijkstraAlgorithm::LazyDijkstraAlgorithm(QObject *parent)
    : GraphAlgorithm(parent)
{
    setObjectName("Lazy Dijkstra's");
}

void LazyDijkstraAlgorithm::execute()
{
    const qsizetype nodesNum = graph->getNodesNum();

    QList<int> distances(nodesNum, INT_MAX);
    distances[0] = 0;

    QList<int> prev(nodesNum, -1);

    PriorityQueue nodeDistancePairs;
    nodeDistancePairs.insert(0, 0);

    auto forEachNeighbour = [&](int value, int neighbour, int weight)
    {
        const int newDist = distances[value] + weight;
        if(distances[neighbour] > newDist)
        {
            distances[neighbour] = newDist;
            prev[neighbour] = value;

            nodeDistancePairs.insert(neighbour, newDist);
        }

        return true;
    };

    QList<bool> visited(nodesNum, false);

    while(!nodeDistancePairs.empty())
    {
        const IntPair top = nodeDistancePairs.extract();

        if(visited[top.first])
        {
            continue;
        };

        visited[top.first] = true;

        if(top.first == nodesNum - 1)
        {
            break;
        }

        graph->forEachNeighbour(top.first, forEachNeighbour);
    }

    QList<int> resultPath;

    int prevValue = nodesNum - 1;
    while(prevValue != -1)
    {
        resultPath.push_back(prevValue);
        prevValue = prev[prevValue];
    }

    std::reverse(resultPath.begin(), resultPath.end());

    if(resultPath[0] != 0)
    {
        resultPath.clear();
    }
}

EagerDijkstraAlgorithm::EagerDijkstraAlgorithm(QObject *parent)
    : GraphAlgorithm(parent)
{
    setObjectName("Eager Dijkstra's");
}

void EagerDijkstraAlgorithm::execute()
{
    const qsizetype nodesNum = graph->getNodesNum();

    QList<int> distances(nodesNum, INT_MAX);
    distances[0] = 0;

    QList<int> prev(nodesNum, -1);

    IndexedPriorityQueue nodeDistancePairs;
    nodeDistancePairs.insert(0, 0);

    auto forEachNeighbour = [&](int value, int neighbour, int weight)
    {
        const int newDist = distances[value] + weight;
        if(distances[neighbour] > newDist)
        {
            distances[neighbour] = newDist;
            prev[neighbour] = value;

            nodeDistancePairs.setValue(neighbour, newDist);
        }

        return true;
    };

    QList<bool> visited(nodesNum, false);

    while(!nodeDistancePairs.empty())
    {
        const IntPair top = nodeDistancePairs.extract();

        if(visited[top.first])
        {
            continue;
        };

        visited[top.first] = true;

        if(top.first == nodesNum - 1)
        {
            break;
        }

        graph->forEachNeighbour(top.first, forEachNeighbour);
    }

    QList<int> resultPath;

    int prevValue = nodesNum - 1;
    while(prevValue != -1)
    {
        resultPath.push_back(prevValue);
        prevValue = prev[prevValue];
    }

    std::reverse(resultPath.begin(), resultPath.end());

    if(resultPath[0] != 0)
    {
        resultPath.clear();
    }
}

BellmanFordAlgorithm::BellmanFordAlgorithm(QObject *parent)
    : GraphAlgorithm(parent)
    , breakIfNoChange(true)
{
    setObjectName("Bellman–Ford");
}

void BellmanFordAlgorithm::execute()
{
    const qsizetype nodesNum = graph->getNodesNum();
    QList<int> distances(nodesNum, INT_MAX);
    distances[0] = 0;

    bool anyChange = false;
    auto forEachEdge = [&](int start, int end, int weight)
    {
        if(distances[start] + weight < distances[end])
        {
            distances[end] = distances[start] + weight;
            anyChange = true;
        }
        return true;
    };

    for(int i = 0; i < nodesNum - 1; ++i)
    {
        anyChange = false;
        graph->forEachEdge(forEachEdge);

        if(breakIfNoChange && !anyChange)
        {
            break;
        }
    }

    if(!breakIfNoChange || breakIfNoChange && anyChange)
    {
        auto dedectNegativeCycle = [&](int start, int end, int weight)
        {
            if(distances[start] + weight < distances[end])
            {
                distances[end] = INT_MIN;
                anyChange = true;
            }
            return true;
        };

        graph->forEachEdge(dedectNegativeCycle);
    }
}

bool BellmanFordAlgorithm::getBreakIfNoChange() const
{
    return breakIfNoChange;
}

void BellmanFordAlgorithm::setBreakIfNoChange(bool newBreakIfNoChange)
{
    if (breakIfNoChange == newBreakIfNoChange)
    {
        return;
    }

    breakIfNoChange = newBreakIfNoChange;
    emit breakIfNoChangeChanged();
}




