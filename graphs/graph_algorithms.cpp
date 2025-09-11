
#include "graph_algorithms.h"
#include "graph.h"

#include "../core/property_editor_factory.h"
#include "../core/utils.h"
#include "../core/benchmark_timers.h"

#include "edge_list.h"
#include "graph_builders.h"
#include "graph_texts.h"

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
    complexityList.push_back(qMakePair("O(1)",                [](int I, int V, int E) { return I; }));
    complexityList.push_back(qMakePair("O(V)",                [](int I, int V, int E) { return V; }));
    complexityList.push_back(qMakePair("O(E)",                [](int I, int V, int E) { return E; }));
    complexityList.push_back(qMakePair("O(V+E)",              [](int I, int V, int E) { return V+E; }));
    complexityList.push_back(qMakePair("O(2*V+E)",            [](int I, int V, int E) { return 2*V+E; }));
    complexityList.push_back(qMakePair("O(V^2)",              [](int I, int V, int E) { return V*V; }));
    complexityList.push_back(qMakePair("O(V+E)*logV",         [](int I, int V, int E) { return (V+E)*std::log(V); }));
    complexityList.push_back(qMakePair("O(V*E)",              [](int I, int V, int E) { return (V*E); }));
    complexityList.push_back(qMakePair("O(V^3)",              [](int I, int V, int E) { return (V*V*V); }));
    complexityList.push_back(qMakePair("O(V*(E+1))",          [](int I, int V, int E) { return V*(E+1); }));
    complexityList.push_back(qMakePair("O(V^2*2^V)",          [](int I, int V, int E) { return V*V*(1<<V); }));
    complexityList.push_back(qMakePair("O(E*log(E))",         [](int I, int V, int E) { return E * std::log(E); }));
    complexityList.push_back(qMakePair("O(E*log(V))",         [](int I, int V, int E) { return E * std::log(V); }));
    complexityList.push_back(qMakePair("O(log(E))",           [](int I, int V, int E) { return std::log(E); }));
    complexityList.push_back(qMakePair("O(log(V))",           [](int I, int V, int E) { return std::log(V); }));
    complexityList.push_back(qMakePair("O((V+E)*log(V+E))",    [](int I, int V, int E) { return (V+E)*std::log(V+E); }));

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

    AlgorithmBenchmarkTimerManager &timerManager = AlgorithmBenchmarkTimerManager::getTimerManager();
    timerManager.clear(this);

    AlgorithmBenchmarkResult& resultData = timerManager.algorithmToResultData[this];
    resultData.mainSeries.reserve(iterationsNumber);

    QString& toolTipText = resultData.toolTipInfo;
    appendPropertiesInfo(toolTipText);

    GraphBuilder* graphBuilder = qobject_cast<GraphBuilder*>(getSelectedBuilder());
    graphBuilder->appendPropertiesInfo(toolTipText);

    currentComplexityFunction = std::find_if(complexityList.begin(), complexityList.end(), [&](const ComplexityNameToFunction& pair)
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
        currentIteration = i;

        QScopedPointer<Graph> testGraph(dynamic_cast<Graph*>(graphBuilder->createDataStructure()));
        graph = testGraph.get();

        ULONG64 start;
        QueryThreadCycleTime(GetCurrentThread(), &start);

        execute();

        ULONG64 end;
        QueryThreadCycleTime(GetCurrentThread(), &end);

        const QPointF point(calculateXForCurrentIteration(), (end - start) - timerManager.takeTimersWorkCycles());
        resultData.mainSeries.append(point);
    }

    graph = nullptr;
    currentIteration = 0;

    toolTipText.append("build iterations : " + QString::number(i));
    toolTipText.append("\n");

    toolTipText.append("start time : " + startTime);
    toolTipText.append("\n");

    toolTipText.append("end time : " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    toolTipText.append("\n");

    emit finished(resultData);
}

qreal GraphAlgorithm::calculateXForCurrentIteration() const
{
    return currentComplexityFunction(currentIteration, graph->getNodesNum(), graph->getEdgesNum());
}

#ifdef QT_DEBUG
void GraphAlgorithm::debugRun()
{
    QScopedPointer<Graph> testGraph(dynamic_cast<Graph*>(new AdjacencyListGraph()));
    graph = testGraph.get();

    for(int i = 0; i < 2; ++i)
    {
        testGraph->addNode();
    }

    testGraph->setIsDirected(false);

    testGraph->addEdge(0, 1, 1);
    testGraph->addEdge(0, 1, 1);
    testGraph->addEdge(0, 1, 1);
    testGraph->addEdge(0, 1, 1);
    testGraph->addEdge(0, 1, 1);
    testGraph->addEdge(0, 1, 1);
    testGraph->addEdge(0, 1, 1);
    testGraph->addEdge(0, 1, 1);
    testGraph->addEdge(0, 1, 1);
    testGraph->addEdge(0, 1, 1);
    testGraph->addEdge(0, 1, 1);
    testGraph->addEdge(0, 1, 1);
    testGraph->addEdge(0, 1, 1);
    testGraph->addEdge(0, 1, 1);
    testGraph->addEdge(0, 1, 1);

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

    {
        BENCHMARK_SCOPED_TIMER("while loop + get edges num", Qt::green, AggregationMode::Sum)

        while(!nodeQueue.empty())
        {
            const int first = nodeQueue.dequeue();
            graph->forEachNeighbour(first, forEachNeighbour);
        }
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

    auto forEachNeighbour = [&](int start, int neighbour, int weight)
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
        graph->forEachNeighbour(first, forEachNeighbour);
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

    QList<int> inDegrees;
    graph->getNodeInDegrees(inDegrees);

    QList<int> leafNodes;

    for(int i = 0; i < nodesNum; ++i)
    {
        if(inDegrees[i] == 0 || inDegrees[i] == 1)
        {
            leafNodes.push_back(i);
            inDegrees[i] = 0;
        }
    };

    qsizetype count = leafNodes.size();
    while(count < nodesNum)
    {
        QList<int> newLeafNodes;

        auto forEachNeighbour = [&](int value, int neighbour, int weight)
        {
            if(--inDegrees[neighbour] == 1)
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

bool TopologicalSort::canRunAlgorithm(QString &outInfo) const
{
    const GraphBuilder* graphBuilder = qobject_cast<GraphBuilder*>(getSelectedBuilder());
    if(graphBuilder->getIsGraphDirected())
    {
        return true;
    }

    outInfo = GraphTexts::UndirectedGraphIsNotSupported;
    return false;
}

void TopologicalSort::execute()
{
    const qsizetype nodesNum = graph->getNodesNum();

    QList<bool> visited(nodesNum, false);

    QStack<int> topologicalOrder;
    topologicalOrder.reserve(graph->getNodesNum());

    for(int i = 0; i < nodesNum; ++i)
    {
        if(!visited[i])
        {
            TopologicalSortHelper(i, visited, topologicalOrder);
        }
    };
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

bool KahnsAlgorithm::canRunAlgorithm(QString &outInfo) const
{
    const GraphBuilder* graphBuilder = qobject_cast<GraphBuilder*>(getSelectedBuilder());
    if(graphBuilder->getIsGraphDirected())
    {
        return true;
    }

    outInfo = GraphTexts::UndirectedGraphIsNotSupported;
    return false;
}

void KahnsAlgorithm::execute()
{
    const qsizetype nodesNum = graph->getNodesNum();

    QList<int> inDegrees;
    graph->getNodeInDegrees(inDegrees);

    QList<int> topologicalOrder;
    topologicalOrder.reserve(graph->getNodesNum());

    QQueue<int> nodeQueue;

    for(int i = 0; i < nodesNum; ++i)
    {
        if(inDegrees[i] == 0)
        {
            nodeQueue.enqueue(i);
        }
    };

    auto forEachNeighbour = [&](int value, int neighbour, int weight)
    {
        if(--inDegrees[neighbour] == 0)
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

bool LazyDijkstraAlgorithm::canRunAlgorithm(QString &outInfo) const
{
    const GraphBuilder* graphBuilder = qobject_cast<GraphBuilder*>(getSelectedBuilder());
    if(graphBuilder->getIsGraphDirected())
    {
        return true;
    }

    outInfo = GraphTexts::UndirectedGraphIsNotSupported;
    return false;
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

bool EagerDijkstraAlgorithm::canRunAlgorithm(QString &outInfo) const
{
    const GraphBuilder* graphBuilder = qobject_cast<GraphBuilder*>(getSelectedBuilder());
    if(graphBuilder->getIsGraphDirected())
    {
        return true;
    }

    outInfo = GraphTexts::UndirectedGraphIsNotSupported;
    return false;
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
        const int first = nodeDistancePairs.extract().first;

        if(visited[first])
        {
            continue;
        };

        visited[first] = true;

        if(first == nodesNum - 1)
        {
            break;
        }

        graph->forEachNeighbour(first, forEachNeighbour);
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

FloydWarshallAlgorithm::FloydWarshallAlgorithm(QObject *parent)
    : GraphAlgorithm(parent)
{
    setObjectName("Floyd-Warshall");
}

void FloydWarshallAlgorithm::execute()
{
    const qsizetype nodesNum = graph->getNodesNum();

    QList<QList<int>> dist(nodesNum, QList<int>(nodesNum, INF));
    QList<QList<int>> next(nodesNum, QList<int>(nodesNum, -1));

    auto forEachNeighbour = [&](int value, int neighbour, int weight)
    {
        dist[value][neighbour] = weight;
        next[value][neighbour] = neighbour;
        return true;
    };

    for(int i = 0; i < nodesNum; i++)
    {
        dist[i][i] = 0;
        graph->forEachNeighbour(i, forEachNeighbour);
    }

    for(int k = 0; k < nodesNum; k++)
    {
        for(int i = 0; i < nodesNum; i++)
        {
            for(int j = 0; j < nodesNum; j++)
            {
                if(dist[i][k] == INF || dist[k][j] == INF)
                {
                    continue;
                }

                const int newDist = dist[i][k] + dist[k][j];
                if(dist[i][j] > newDist)
                {
                    dist[i][j] = newDist;
                    next[i][j] = next[i][k];
                }
            }
        }
    }

    for(int k = 0; k < nodesNum; k++)
    {
        for(int i = 0; i < nodesNum; i++)
        {
            for(int j = 0; j < nodesNum; j++)
            {
                if(dist[i][j] == -INF || dist[i][k] == INF || dist[k][j] == INF)
                {
                    continue;
                };

                const int newDist = dist[i][k] + dist[k][j];
                if(dist[i][j] > newDist)
                {
                    dist[i][j] = -INF;
                    next[i][j] = -1;
                }
            }
        }
    }

    QList<int> resultPath;

    const int start = 0;
    const int end = nodesNum - 1;

    int at = start;
    while(at != nodesNum - 1 && at != -1)
    {
        resultPath.append(at);
        at = next[at][end];
    }

    if(at == end)
    {
        resultPath.append(end);
    }
    else
    {
        resultPath.clear();
    }
}

static const int UNVISITED = -1;

SCCsAlgorithm::SCCsAlgorithm(QObject *parent)
    : GraphAlgorithm(parent)
    , time(0)
    , sccCount(0)
{
    setObjectName("Tarjan's Strongly Connected Component");
}

void SCCsAlgorithm::execute()
{
    const qsizetype nodesNum = graph->getNodesNum();

    QList<int> low(nodesNum, 0);
    QList<int> visitTime(nodesNum, UNVISITED);
    QStack<int> stack;

    for(int i = 0; i < nodesNum; i++)
    {
        if(visitTime[i] == UNVISITED)
        {
            SCCsHelper(i, visitTime, stack, low);
        }
    }
}

void SCCsAlgorithm::SCCsHelper(int begin, QList<int> &visitTime, QStack<int> &stack, QList<int> &low)
{
    visitTime[begin] = low[begin] = time++;
    stack.push(begin);

    auto forEachNeighbour = [&](int value, int neighbour, int weight)
    {
        if(visitTime[neighbour] == UNVISITED)
        {
            SCCsHelper(neighbour, visitTime, stack, low);
        }

        if(stack.contains(neighbour))
        {
            low[value] = std::min(low[value], low[neighbour]);
        }

        return true;
    };

    graph->forEachNeighbour(begin, forEachNeighbour);

    if(low[begin] == visitTime[begin])
    {
        while(!stack.empty())
        {
            const int popValue = stack.pop();
            low[popValue] = visitTime[begin];

            if(popValue == begin)
            {
                break;
            }
        }

        sccCount++;
    }
}

TravelingSalesmanProblemAlgorithmHash::TravelingSalesmanProblemAlgorithmHash(QObject *parent)
    : GraphAlgorithm(parent)
{
    setObjectName("Traveling Salesman Problem (Held-Karp, Hash-Index method)");
}

void TravelingSalesmanProblemAlgorithmHash::execute()
{
    const qsizetype nodesNum = graph->getNodesNum();

    QList<QSet<int>> combinations;
    generateCombinations(combinations);

    QHash<Index, int> minDistances;
    QHash<Index, int> parents;

    for(const QSet<int>& combination : combinations)
    {
        int minDistance = INF;
        for(int i = 1; i < nodesNum; i++)
        {
            if(combination.contains(i))
            {
                continue;
            }

            const Index index(i, combination);
            if(combination.empty())
            {
                minDistances[index] = graph->getEdgeWeight(0, i);
                parents[index] = 0;
                continue;
            }

            minDistances[index] = INF;

            for(int end : combination)
            {
                QSet<int> subCombination = combination;
                subCombination.remove(end);

                const int newDistance = graph->getEdgeWeight(end, i) + getDistanceTo(end, index, minDistances);
                if(newDistance < minDistances[index])
                {
                    minDistances[index] = newDistance;
                    parents[index] = end;
                }
            }
        }
    }

    QSet<int> set;
    set.reserve(nodesNum - 1);

    for(int i = 1; i < nodesNum; i++)
    {
        set.insert(i);
    }

    const Index index(0, set);
    minDistances[index] = INF;

    for(int end : set)
    {
        const int newDistance = graph->getEdgeWeight(end, 0) + getDistanceTo(end, index, minDistances);
        if(newDistance < minDistances[index])
        {
            minDistances[index] = newDistance;
            parents[index] = end;
        }
    }
}

QList<int> TravelingSalesmanProblemAlgorithmHash::buildResultPath(const Index& index, const QHash<Index, int> &parents)
{
    const qsizetype nodesNum = graph->getNodesNum();

    QList<int> resultPath;
    resultPath.reserve(nodesNum + 1);

    Index pathIndex = index;
    int to = 0;

    auto fromIt = parents.find(pathIndex);
    while(fromIt != parents.end())
    {
        resultPath.append(to);
        to = *fromIt;

        if (to == 0)
        {
            break;
        }

        pathIndex.vertices.remove(to);
        pathIndex.vertex = to;
        fromIt = parents.find(pathIndex);
    }

    if(fromIt == parents.end())
    {
        resultPath.clear();
    }
    else
    {
        resultPath.append(0);
        std::reverse(resultPath.begin(), resultPath.end());
    }

    return resultPath;
}

int TravelingSalesmanProblemAlgorithmHash::getDistanceTo(int end, const Index &index, const QHash<Index, int> &minDistances) const
{
    const Index newIndex(end, index.vertices - QSet<int>{end});
    return minDistances[newIndex];
}

void TravelingSalesmanProblemAlgorithmHash::generateCombinations(QList<QSet<int>>& combinations) const
{
    const int nodesNum = graph->getNodesNum();
    const int combinationsNum = 1 << (nodesNum - 1);

    combinations.append(QSet<int>{});

    for(int i = 1; i < combinationsNum; i++)
    {
        QSet<int> combination;
        for(int j = 0; j < nodesNum - 1; j++)
        {
            if((1 << j) & i)
            {
                combination.insert(j + 1);
            }
        }

        combinations.append(combination);
    }
}

TravelingSalesmanProblemAlgorithmBitmask::TravelingSalesmanProblemAlgorithmBitmask(QObject *parent)
    : GraphAlgorithm(parent)
{
    setObjectName("Traveling Salesman Problem (Held-Karp, Bitmask method)");
}

void TravelingSalesmanProblemAlgorithmBitmask::execute()
{
    const qsizetype nodesNum = graph->getNodesNum();

    QList<QList<int>> memo(nodesNum, QList<int>( 1 << nodesNum, INF));
    for(int i = 1; i < nodesNum; i++)
    {
        memo[i][1 | 1 << i] = graph->getEdgeWeight(0, i);
    }

    for(int r = 3; r <= nodesNum; r++)
    {
        QList<int> combinations;
        generateCombinations(r, combinations);

        for(int combination : combinations)
        {
            if(isNotInCombination(0, combination))
            {
                continue;
            }

            for(int next = 1; next < nodesNum; next++)
            {
                if(isNotInCombination(next, combination))
                {
                    continue;
                }

                const int prevState = combination ^ (1 << next);

                for(int end = 1; end < nodesNum; end++)
                {
                    if(end == next || isNotInCombination(end, combination))
                    {
                        continue;
                    }

                    const int newDistance = memo[end][prevState] + graph->getEdgeWeight(end, next);
                    if(newDistance < memo[next][combination])
                    {
                        memo[next][combination] = newDistance;
                    }
                }
            }
        }
    }

    buildResultPath(memo);
}

bool TravelingSalesmanProblemAlgorithmBitmask::isNotInCombination(int i, int combination) const
{
    return ((1 << i) & combination) == 0;
}

QList<int> TravelingSalesmanProblemAlgorithmBitmask::buildResultPath(const QList<QList<int>> &memo)
{
    const qsizetype nodesNum = graph->getNodesNum();

    QList<int> resultPath;
    resultPath.reserve(nodesNum + 1);

    resultPath.append(0);

    int state = ( 1 << nodesNum ) - 1;
    int to = 0;

    for(int i = 0; i < nodesNum; i++)
    {
        int minDistance = INF;
        int last = -1;

        for(int end = 0; end < nodesNum; end++)
        {
            if(end == to || isNotInCombination(end, state))
            {
                continue;
            }

            if(last == -1)
            {
                last = end;
            }

            const int newDistance = memo[end][state] + graph->getEdgeWeight(end, to);
            if(newDistance < minDistance)
            {
                minDistance = newDistance;
                last = end;
            }
        }

        if(last != 0 && minDistance == INF)
        {
            resultPath.clear();
            break;
        }

        resultPath.append(last);
        state ^= (1 << last);

        to = last;
    }

    std::reverse(resultPath.begin(), resultPath.end());

    return resultPath;
}

void TravelingSalesmanProblemAlgorithmBitmask::generateCombinations(int r, QList<int> &combinations) const
{
    generateCombinations(0, 0, r, combinations);
}

void TravelingSalesmanProblemAlgorithmBitmask::generateCombinations(int subSet, int pos, int r, QList<int> &combinations) const
{
    if(r == 0)
    {
        combinations.append(subSet);
        return;
    }

    const int nodesNum = graph->getNodesNum();
    for(int i = pos; i < nodesNum; i++)
    {
        subSet ^= (1 << i);

        generateCombinations(subSet, i + 1, r - 1, combinations);

        subSet ^= (1 << i);
    }
}

EulerianPathAlgorithm::EulerianPathAlgorithm(QObject *parent)
    : GraphAlgorithm(parent)
{
    setObjectName("Eulerian Path / (Hierholzer's algorithm)");
}

void EulerianPathAlgorithm::execute()
{
    QList<int> inDegrees;
    QList<int> outDegrees;
    {
        BENCHMARK_SCOPED_TIMER("getNodeDegrees", Qt::green, AggregationMode::Sum)

        graph->getNodeDegrees(inDegrees, outDegrees);
    }

    bool pathExist;
    {
        //BENCHMARK_SCOPED_TIMER("doesEulerianPathExist", Qt::red, false)

        pathExist = doesEulerianPathExist(inDegrees, outDegrees);
    }

    if(pathExist)
    {
        QList<int> eulerianPath;
        {
            //BENCHMARK_SCOPED_TIMER("reserve(graph->getEdgesNum() + 1)", Qt::blue, false)

            eulerianPath.reserve(graph->getEdgesNum() + 1);
        }

        int start;
        {
            //BENCHMARK_SCOPED_TIMER("findStart", Qt::black, false)

            start = findStart(inDegrees, outDegrees);
        }

        {
            BENCHMARK_SCOPED_TIMER("DFSHelper", Qt::red, AggregationMode::Sum)

            EdgeList visitedUndirectedEdges; // It is only used with an undirected graph
            DFSHelper(start, outDegrees, eulerianPath, visitedUndirectedEdges);
        }
    }
}

void EulerianPathAlgorithm::DFSHelper(int begin, QList<int> &outDegrees, QList<int> &eulerianPath, EdgeList &visitedUndirectedEdges) const
{
    {
        while(outDegrees[begin] != 0)
        {
            const int neighbour = graph->getNeighbourAt(begin, --outDegrees[begin]);

            if(!graph->getIsDirected())
            {
                const Edge edge(begin, neighbour, false);
                if(!visitedUndirectedEdges.contains(edge))
                {
                    visitedUndirectedEdges.add(begin, neighbour, false);
                    DFSHelper(neighbour, outDegrees, eulerianPath, visitedUndirectedEdges);
                }
            }
            else
            {
                DFSHelper(neighbour, outDegrees, eulerianPath, visitedUndirectedEdges);
            }
        }
    }

    eulerianPath.push_front(begin);
}

bool EulerianPathAlgorithm::doesEulerianPathExist(const QList<int> &inDegrees, const QList<int> &outDegrees) const
{
    if(outDegrees.empty())
    {
        return false;
    }

    if(graph->getIsDirected())
    {
        int startNodes = 0, endNodes = 0;

        for(int i = 0; i < graph->getNodesNum(); i++)
        {
            const int outMinusIn = outDegrees[i] - inDegrees[i];
            const int inMinusOut = inDegrees[i] - outDegrees[i];

            if(outMinusIn > 1 || inMinusOut > 1)
            {
                return false;
            }
            else if(outMinusIn == 1)
            {
                startNodes++;
            }
            else if(inMinusOut == 1)
            {
                endNodes++;
            }
        }

        return (startNodes == 0 && endNodes == 0) || (startNodes == 1 && endNodes == 1);
    }
    else
    {
        bool allEven = true;
        int oddDegreesNum = 0;

        // inDegrees are equal to outDegrees
        for(int nodeDegree : inDegrees)
        {
            if(nodeDegree % 2 != 0)
            {
                allEven = false;
                oddDegreesNum++;
            }
        }

        return allEven || oddDegreesNum == 2;
    }
}

int EulerianPathAlgorithm::findStart(const QList<int> &inDegrees, const QList<int> &outDegrees) const
{
    int start = 0;

    if(graph->getIsDirected())
    {
        for(int i = 0; i < graph->getNodesNum(); i++)
        {
            const int outMinusIn = outDegrees[i] - inDegrees[i];

            if(outMinusIn == 1)
            {
                return i;
            }

            if(outDegrees[i] > 0)
            {
                start = i;
            }
        }
    }
    else
    {
        for(int i = 0; i < graph->getNodesNum(); i++)
        {
            if(inDegrees[i] % 2 == 1)
            {
                return i;
            }
        }
    }

    return start;
}

PrimMinimumSpanningTreeAlgorithm::PrimMinimumSpanningTreeAlgorithm(QObject *parent)
{
    setObjectName("Prim's Minimum Spanning Tree Algorithm (Lazy version)");
}

bool PrimMinimumSpanningTreeAlgorithm::canRunAlgorithm(QString &outInfo) const
{
    const GraphBuilder* graphBuilder = qobject_cast<GraphBuilder*>(getSelectedBuilder());
    if(!graphBuilder->getIsGraphDirected())
    {
        return true;
    }

    outInfo = GraphTexts::DirectedGraphIsNotSupported;
    return false;
}

void PrimMinimumSpanningTreeAlgorithm::execute()
{
    const qsizetype nodesNum = graph->getNodesNum();

    QList<int> spanningTree;
    spanningTree.reserve(nodesNum);

    QList<bool> visited(nodesNum, false);

    PriorityQueue edgesPq;
    edgesPq.insert(0, 0);

    auto forEachNeighbour = [&](int value, int neighbour, int weight)
    {
        if(!visited[neighbour])
        {
            edgesPq.insert(neighbour, weight);
        }
        return true;
    };

    while(!edgesPq.empty() && spanningTree.size() < nodesNum)
    {
        int first;
        {
            BENCHMARK_SCOPED_TIMER("edgesPq.extract().first", Qt::red, AggregationMode::Max)

            first = edgesPq.extract().first;
        }

        if(visited[first])
        {
            continue;
        }

        visited[first] = true;

        spanningTree.append(first);

        {
            BENCHMARK_SCOPED_TIMER("forEachNeighbour", Qt::blue, AggregationMode::Max)

            graph->forEachNeighbour(first, forEachNeighbour);
        }
    }

    if(spanningTree.size() != nodesNum)
    {
        spanningTree.clear();
    }
}
