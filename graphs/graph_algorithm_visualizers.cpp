
#include "graph_algorithm_visualizers.h"
#include "widgets/graph_widget.h"

#include "../core/utils.h"

#include <QMessageBox>
#include <QQueue>

GraphAlgorithmVisualizer::GraphAlgorithmVisualizer(QObject *parent)
    : AlgorithmVisualizer(parent)
    , start(0)
    , randomStart(false)
    , graph(nullptr)
{

}

bool GraphAlgorithmVisualizer::supportsDirectedGraph() const
{
    return true;
}

bool GraphAlgorithmVisualizer::supportsUndirectedGraph() const
{
    return true;
}

GraphAlgorithmVisualizer::~GraphAlgorithmVisualizer()
{

}

void GraphAlgorithmVisualizer::clear()
{
    AlgorithmVisualizer::clear();

    visualizationTimer.stop();
    resultEdgeList.clear();
}

int GraphAlgorithmVisualizer::getStart() const
{
    return start;
}

void GraphAlgorithmVisualizer::setStart(int newStart)
{
    if (start == newStart)
    {
        return;
    }

    start = newStart;
    emit startChanged();
}

bool GraphAlgorithmVisualizer::getRandomStart() const
{
    return randomStart;
}

void GraphAlgorithmVisualizer::setRandomStart(bool newRandomStart)
{
    if (randomStart == newRandomStart)
    {
        return;
    }

    randomStart = newRandomStart;
    emit randomStartChanged();
}

Graph *GraphAlgorithmVisualizer::getGraph() const
{
    return graph;
}

void GraphAlgorithmVisualizer::setGraph(Graph *newGraph)
{
    graph = newGraph;
}

void GraphAlgorithmVisualizer::updateVisualization()
{
    if(!resultEdgeList.empty())
    {
        const Edge edge = resultEdgeList.takeFirst();

        graphWidget->setNodeColor(edge.getStart(), Qt::red, false);
        graphWidget->setNodeColor(edge.getEnd(), Qt::red, false);

        graphWidget->setEdgeColor(edge.getStart(), edge.getEnd(), Qt::red);
    }

    if(resultEdgeList.empty())
    {
        clear();
        emit finished();
    }
}

void GraphAlgorithmVisualizer::showInfo(const QString& info) const
{
    QMessageBox::information(qobject_cast<QWidget*>(parent()), "Info", info);
}

PathFindingAlgorithmVisualizer::PathFindingAlgorithmVisualizer(QObject *parent)
    : GraphAlgorithmVisualizer(parent)
    , end(1)
    , randomEnd(false)
{

}

PathFindingAlgorithmVisualizer::~PathFindingAlgorithmVisualizer()
{

}

void PathFindingAlgorithmVisualizer::clear()
{
    GraphAlgorithmVisualizer::clear();

    resultPath.clear();
}

int PathFindingAlgorithmVisualizer::getEnd() const
{
    return end;
}

void PathFindingAlgorithmVisualizer::setEnd(int newEnd)
{
    if (end == newEnd)
    {
        return;
    }

    end = newEnd;
    emit endChanged();
}

bool PathFindingAlgorithmVisualizer::getRandomEnd() const
{
    return randomEnd;
}

void PathFindingAlgorithmVisualizer::setRandomEnd(bool newRandomEnd)
{
    if (randomEnd == newRandomEnd)
    {
        return;
    }

    randomEnd = newRandomEnd;
    emit randomEndChanged();
}

bool PathFindingAlgorithmVisualizer::setStartAndEnd()
{
    if(randomStart)
    {
        setStart(graph->getRandomValue());
    }

    if(randomEnd)
    {
        setEnd(graph->getRandomValue());
    }

    const qsizetype nodesNum = graph->getNodesNum();

    if(start >= nodesNum || end >= nodesNum)
    {
        showInfo("Choosen start or end is not valid");
        emit finished();

        return false;
    }

    return true;
}

void PathFindingAlgorithmVisualizer::updateVisualization()
{
    if(!resultEdgeList.empty())
    {
        const Edge edge = resultEdgeList.takeFirst();

        graphWidget->setNodeColor(edge.getStart(), Qt::red, false);
        graphWidget->setNodeColor(edge.getEnd(), Qt::red, false);

        graphWidget->setEdgeColor(edge.getStart(), edge.getEnd(), Qt::red);

        if(!resultPath.empty() && edge.getEnd() == resultPath.last())
        {
            for(int i = 0; i < resultPath.size(); ++i)
            {
                graphWidget->setNodeColor(resultPath[i], Qt::green, false);

                if(i + 1 < resultPath.size())
                {
                    graphWidget->setEdgeColor(resultPath[i], resultPath[i + 1], Qt::green, false);
                }
            }
            graphWidget->update();

            resultEdgeList.clear();
        }
    }

    if(resultEdgeList.empty())
    {
        clear();
        emit finished();
    }
}

void PathFindingAlgorithmVisualizer::startVisualization(QWidget *widget)
{
    graphWidget = qobject_cast<GraphWidget*>(widget);
    graphWidget->setNodeColor(start, Qt::red);

    visualizationTimer.start();
}

void PathFindingAlgorithmVisualizer::buildResultPath(const QList<int> &prev)
{
    int prevValue = end;
    while(prevValue != -1 && prevValue != start)
    {
        resultPath.push_back(prevValue);
        prevValue = prev[prevValue];
    }

    std::reverse(resultPath.begin(), resultPath.end());

    if(resultPath[0] != start)
    {
        resultPath.clear();
    }
}

BFSVisualizer::BFSVisualizer(QObject *parent)
    : GraphAlgorithmVisualizer(parent)
{
    setObjectName("Breadth First Search");
}

void BFSVisualizer::run(QWidget *widget)
{
    if(randomStart)
    {
        setStart(graph->getRandomValue());
    }

    const qsizetype nodesNum = graph->getNodesNum();

    if(start < nodesNum)
    {
        QQueue<int> nodeQueue;
        nodeQueue.reserve(graph->getNodesNum());

        QList<bool> visited(nodesNum, false);

        resultEdgeList.reserve(nodesNum);

        nodeQueue.enqueue(start);
        visited[start] = true;

        auto func = [&](int value, int neighbour, int weight)
        {
            if(!visited[neighbour])
            {
                visited[neighbour] = true;
                nodeQueue.enqueue(neighbour);
            }

            resultEdgeList.add(value, neighbour, graph->getIsDirected());
            return true;
        };

        while(!nodeQueue.empty())
        {
            const int first = nodeQueue.dequeue();
            graph->forEachNeighbour(first, func);
        }

        graphWidget = qobject_cast<GraphWidget*>(widget);
        graphWidget->setNodeColor(start, Qt::red);

        visualizationTimer.start();
    }
    else
    {
        showInfo("Choosen start is not valid");
        emit finished();
    }
}

BFSShortestPathVisualizer::BFSShortestPathVisualizer(QObject *parent)
    : PathFindingAlgorithmVisualizer(parent)
{
    setObjectName("Breadth First Search Shortest Path");
}

void BFSShortestPathVisualizer::run(QWidget *widget)
{
    const qsizetype nodesNum = graph->getNodesNum();

    if(setStartAndEnd())
    {
        QQueue<int> nodeQueue;
        nodeQueue.reserve(nodesNum);

        QList<bool> visited(nodesNum, false);

        resultEdgeList.reserve(graph->getEdgesNum());

        nodeQueue.enqueue(start);
        visited[start] = true;

        QList<int> prev(nodesNum, -1);

        auto func = [&](int value, int neighbour, int weight)
        {
            if(!visited[neighbour])
            {
                visited[neighbour] = true;
                nodeQueue.enqueue(neighbour);

                prev[neighbour] = value;
            }

            resultEdgeList.add(value, neighbour, graph->getIsDirected());

            if(end == neighbour)
            {
                nodeQueue.clear();
                return false;
            }
            return true;
        };

        while(!nodeQueue.empty())
        {
            const int first = nodeQueue.dequeue();
            graph->forEachNeighbour(first, func);
        }

        buildResultPath(prev);
        startVisualization(widget);
    }
}

DFSVisualizer::DFSVisualizer(QObject *parent)
    : GraphAlgorithmVisualizer(parent)
{
    setObjectName("Depth First Search");
}

void DFSVisualizer::run(QWidget *widget)
{
    if(randomStart)
    {
        setStart(graph->getRandomValue());
    }

    if(start < graph->getNodesNum())
    {
        QList<bool> visited(graph->getNodesNum(), false);

        resultEdgeList.reserve(graph->getEdgesNum());

        DFSHelper(start, visited);

        graphWidget = qobject_cast<GraphWidget*>(widget);
        graphWidget->setNodeColor(start, Qt::red);
        visualizationTimer.start();
    }
    else
    {
        showInfo("Choosen start or end is not valid");
        emit finished();
    }
}

void DFSVisualizer::DFSHelper(int begin, QList<bool>& visited)
{
    visited[begin] = true;

    auto func = [&](int start, int neighbour, int weight)
    {
        resultEdgeList.add(start, neighbour, graph->getIsDirected());

        if(!visited[neighbour])
        {
            DFSHelper(neighbour, visited);
        }
        return true;
    };

    graph->forEachNeighbour(begin, func);
}

TreeCentersVisualizer::TreeCentersVisualizer(QObject *parent)
    : GraphAlgorithmVisualizer(parent)
{
    setObjectName("Tree Centers");
}

void TreeCentersVisualizer::run(QWidget *widget)
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
        visitedLeafLayers.push_back(leafNodes);

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

    centers = leafNodes;

    graphWidget = qobject_cast<GraphWidget*>(widget);

    updateVisualization();
    visualizationTimer.start();
}

void TreeCentersVisualizer::clear()
{
    GraphAlgorithmVisualizer::clear();

    visitedLeafLayers.clear();
    centers.clear();
}

QWidget *TreeCentersVisualizer::createPropertiesWidget(QWidget *parent)
{
    return nullptr;
}

void TreeCentersVisualizer::updateVisualization()
{
    if(!visitedLeafLayers.empty())
    {
        const QList<int> leafLayer = visitedLeafLayers.takeFirst();
        for(int node : leafLayer)
        {
            graphWidget->setNodeColor(node, Qt::gray, false);
        }
        graphWidget->update();
    }
    else
    {
        for(int center : centers)
        {
            graphWidget->setNodeColor(center, Qt::green, false);
        }
        graphWidget->update();
    }

    if(visitedLeafLayers.empty())
    {
        clear();
        emit finished();
    }
}

TopologicalSortVisualizer::TopologicalSortVisualizer(QObject *parent)
    : GraphAlgorithmVisualizer(parent)
{
    setObjectName("Topological Sort");
}

void TopologicalSortVisualizer::run(QWidget *widget)
{
    if(randomStart)
    {
        setStart(graph->getRandomValue());
    }

    const qsizetype nodesNum = graph->getNodesNum();

    if(start < nodesNum)
    {
        QList<bool> visited(nodesNum, false);

        topologicalOrder.reserve(nodesNum);
        resultEdgeList.reserve(graph->getEdgesNum());

        TopologicalSortHelper(start, visited);

        auto forEachNode = [&](int value)
        {
            TopologicalSortHelper(value, visited);
            return true;
        };
        graph->forEachNode(forEachNode);

        graphWidget = qobject_cast<GraphWidget*>(widget);

        graphWidget->setNodeColor(start, Qt::red);
        visualizationTimer.start();
    }
    else
    {
        showInfo("Choosen start is not valid");
        emit finished();
    }
}

void TopologicalSortVisualizer::clear()
{
    GraphAlgorithmVisualizer::clear();

    topologicalOrder.clear();
}

bool TopologicalSortVisualizer::supportsUndirectedGraph() const
{
    return false;
}

void TopologicalSortVisualizer::updateVisualization()
{
    if(!resultEdgeList.empty())
    {
        const Edge& edge = resultEdgeList.first();
        if(graphWidget->getNodeColor(edge.getStart()) != Qt::red)
        {
            graphWidget->setNodeColor(edge.getStart(), Qt::red);
        }
        else
        {
            graphWidget->setNodeColor(edge.getEnd(), Qt::red, false);

            graphWidget->setEdgeColor(edge.getStart(), edge.getEnd(), Qt::red);

            resultEdgeList.takeFirst();
        }
    }
    else if(!topologicalOrder.empty())
    {
        graphWidget->setNodeColor(topologicalOrder.pop(), Qt::green);
    }

    if(resultEdgeList.empty() && topologicalOrder.empty())
    {
        clear();
        emit finished();
    }
}

void TopologicalSortVisualizer::TopologicalSortHelper(int begin, QList<bool>& visited)
{
    visited[begin] = true;

    auto forEachNeighbour = [&](int start, int neighbour, int weight)
    {
        resultEdgeList.add(start, neighbour, true);

        if(!visited[neighbour])
        {
            TopologicalSortHelper(neighbour, visited);
        }
        return true;
    };

    graph->forEachNeighbour(begin, forEachNeighbour);

    if(!topologicalOrder.contains(begin))
    {
        topologicalOrder.push(begin);
    }
}


KahnsAlgorithmVisualizer::KahnsAlgorithmVisualizer(QObject *parent)
    : GraphAlgorithmVisualizer(parent)
{
    setObjectName("Kahn's Algorithm (Topological Sort)");

    hiddenProperties.push_back("start");
    hiddenProperties.push_back("randomStart");
}

void KahnsAlgorithmVisualizer::run(QWidget *widget)
{
    QList<int> nodeDegrees = graph->getNodeDegrees();

    topologicalOrder.reserve(graph->getNodesNum());
    resultEdgeList.reserve(graph->getEdgesNum());

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
        resultEdgeList.add(value, neighbour, true);

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

    graphWidget = qobject_cast<GraphWidget*>(widget);

    graphWidget->setNodeColor(0, Qt::red);
    visualizationTimer.start();
}

void KahnsAlgorithmVisualizer::clear()
{
    GraphAlgorithmVisualizer::clear();

    topologicalOrder.clear();
}

bool KahnsAlgorithmVisualizer::supportsUndirectedGraph() const
{
    return false;
}

void KahnsAlgorithmVisualizer::updateVisualization()
{
    if(!resultEdgeList.empty())
    {
        const Edge& edge = resultEdgeList.first();
        if(graphWidget->getNodeColor(edge.getStart()) != Qt::red)
        {
            graphWidget->setNodeColor(edge.getStart(), Qt::red);
        }
        else
        {
            graphWidget->setNodeColor(edge.getEnd(), Qt::red, false);

            graphWidget->setEdgeColor(edge.getStart(), edge.getEnd(), Qt::red);

            resultEdgeList.takeFirst();
        }
    }
    else if(!topologicalOrder.empty())
    {
        graphWidget->setNodeColor(topologicalOrder.takeFirst(), Qt::green);
    }

    if(resultEdgeList.empty() && topologicalOrder.empty())
    {
        clear();
        emit finished();
    }
}

SSSPonDAGVisualizer::SSSPonDAGVisualizer(QObject *parent)
    : PathFindingAlgorithmVisualizer(parent)
{
    setObjectName("Single Source Shortest Path (SSSP on DAG)");
}

void SSSPonDAGVisualizer::run(QWidget *widget)
{
    const qsizetype nodesNum = graph->getNodesNum();

    if(setStartAndEnd())
    {
        QList<int> topologicalSortList;
        topologicalSort(topologicalSortList);

        resultEdgeList.reserve(nodesNum);

        QList<int> distances(nodesNum, INT_MAX);
        distances[start] = 0;

        QList<int> prev(nodesNum, -1);

        auto forEachNeighbour = [&](int value, int neighbour, int weight)
        {
            resultEdgeList.add(value, neighbour, true);

            const int newDist = distances[value] + weight;
            if(distances[neighbour] > newDist)
            {
                distances[neighbour] = newDist;
                prev[neighbour] = value;
            }
            return true;
        };

        for(int node : topologicalSortList)
        {
            if (distances[node] == INT_MAX)
            {
                continue;
            }

            graph->forEachNeighbour(node, forEachNeighbour);
        }

        buildResultPath(prev);
        startVisualization(widget);
    }
}

bool SSSPonDAGVisualizer::supportsUndirectedGraph() const
{
    return false;
}

void SSSPonDAGVisualizer::topologicalSort(QList<int> &outTopologicalOrder) const
{
    QList<int> nodeDegrees = graph->getNodeDegrees();

    outTopologicalOrder.reserve(graph->getNodesNum());

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
        outTopologicalOrder.push_back(first);
        graph->forEachNeighbour(first, forEachNeighbour);
    }

    if(outTopologicalOrder.size() < graph->getNodesNum())
    {
        outTopologicalOrder.clear(); // cycle
    }
}

LazyDijkstraVisualizer::LazyDijkstraVisualizer(QObject *parent)
    : PathFindingAlgorithmVisualizer(parent)
{
    setObjectName("Lazy Dijkstra's");
}

void LazyDijkstraVisualizer::run(QWidget *widget)
{
    if(setStartAndEnd())
    {
        const qsizetype nodesNum = graph->getNodesNum();

        resultEdgeList.reserve(nodesNum);

        QList<int> distances(nodesNum, INT_MAX);
        distances[start] = 0;

        QList<int> prev(nodesNum, -1);

        PriorityQueue nodeDistancePairs;
        nodeDistancePairs.insert(start, 0);

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

            if(prev[top.first] != -1)
            {
                resultEdgeList.add(prev[top.first], top.first, true);
            }

            if(top.first == end)
            {
                break;
            }

            graph->forEachNeighbour(top.first, forEachNeighbour);
        }

        buildResultPath(prev);
        startVisualization(widget);
    }
}

BellmanFordVisualizer::BellmanFordVisualizer(QObject *parent)
    : PathFindingAlgorithmVisualizer(parent)
{
    setObjectName("Bellman–Ford");
}

void BellmanFordVisualizer::clear()
{
    PathFindingAlgorithmVisualizer::clear();

    negativeCycle.clear();
}

void BellmanFordVisualizer::run(QWidget *widget)
{
    if(setStartAndEnd())
    {
        const qsizetype nodesNum = graph->getNodesNum();

        resultEdgeList.reserve(nodesNum);

        QList<int> distances(nodesNum, INT_MAX);
        distances[start] = 0;

        QList<int> prev(nodesNum, -1);

        bool anyChange = false;
        auto forEachEdge = [&](int value, int neighbour, int weight)
        {
            resultEdgeList.add(value, neighbour, graph->getIsDirected());

            if(distances[value] + weight < distances[neighbour])
            {
                distances[neighbour] = distances[value] + weight;
                prev[neighbour] = value;
                anyChange = true;
            }
            return true;
        };

        for(int i = 0; i < nodesNum - 1; ++i)
        {
            anyChange = false;
            graph->forEachEdge(forEachEdge);

            if(!anyChange)
            {
                break;
            }
        }

        if(anyChange)
        {
            auto dedectNegativeCycle = [&](int value, int neighbour, int weight)
            {
                if(distances[neighbour] != INT_MIN && distances[value] + weight < distances[neighbour])
                {
                    distances[neighbour] = INT_MIN;

                    negativeCycle.add(value, neighbour, graph->getIsDirected());

                    anyChange = true;
                }
                return true;
            };

            for(int i = 0; i < nodesNum - 1; ++i)
            {
                anyChange = false;
                graph->forEachEdge(dedectNegativeCycle);

                if(!anyChange)
                {
                    break;
                }
            }
        }

        buildResultPath(prev);
        startVisualization(widget);
    }
}

void BellmanFordVisualizer::updateVisualization()
{
    EdgeList negativeCycleCopy = resultEdgeList.size() <= 1 ? negativeCycle : EdgeList{};

    PathFindingAlgorithmVisualizer::updateVisualization();

    while(!negativeCycleCopy.empty())
    {
        const Edge edge = negativeCycleCopy.takeFirst();

        graphWidget->setNodeColor(edge.getStart(), Qt::blue, false);
        graphWidget->setNodeColor(edge.getEnd(), Qt::blue, false);

        graphWidget->setEdgeColor(edge.getStart(), edge.getEnd(), Qt::blue);
    }
}
