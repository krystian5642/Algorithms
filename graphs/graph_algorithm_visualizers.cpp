
#include "graph_algorithm_visualizers.h"
#include "widgets/graph_widget.h"

#include "graph_texts.h"

#include "../core/utils.h"

#include <QMessageBox>
#include <QQueue>
#include <queue>

GraphAlgorithmVisualizer::GraphAlgorithmVisualizer(QObject *parent)
    : AlgorithmVisualizer(parent)
    , start(0)
    , randomStart(false)
    , graph(nullptr)
{

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

void GraphAlgorithmVisualizer::startVisualization(QWidget *widget)
{
    if(!resultEdgeList.empty())
    {
        graphWidget = qobject_cast<GraphWidget*>(widget);
        graphWidget->setNodeColor(start, Qt::red);

        visualizationTimer.start();
    }
    else
    {
        finish();
    }
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
        finish();
    }
}

void GraphAlgorithmVisualizer::showInfo(const QString& info) const
{
    QMessageBox::information(qobject_cast<QWidget*>(parent()), "Info", info);
}

void GraphAlgorithmVisualizer::finish()
{
    clear();
    emit finished();
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

    if(start >= nodesNum || end >= nodesNum || start < 0 || end < 0)
    {
        showInfo(GraphTexts::StartOrEndNodesAreInvalid);
        finish();

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
        finish();
    }
}

void PathFindingAlgorithmVisualizer::buildResultPath(const QList<int> &prev)
{
    if(!prev.empty() && prev[start] == end)
    {
        return;
    }

    int prevValue = end;
    while(prevValue != -1)
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

    if(start < nodesNum && start >= 0)
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
        showInfo(GraphTexts::StartNodeIsInvalid);
        finish();
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

    if(start < graph->getNodesNum() && start >= 0)
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
        showInfo(GraphTexts::StartOrEndNodesAreInvalid);
        finish();
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
        visitedLeafLayers.push_back(leafNodes);

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
        finish();
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

    if(start < nodesNum && start >= 0)
    {
        QList<bool> visited(nodesNum, false);

        topologicalOrder.reserve(nodesNum);
        resultEdgeList.reserve(graph->getEdgesNum());

        TopologicalSortHelper(start, visited);

        for(int i = 0; i < nodesNum; ++i)
        {
            if(!visited[i])
            {
                TopologicalSortHelper(i, visited);
            }
        };

        startVisualization(widget);
    }
    else
    {
        showInfo(GraphTexts::StartNodeIsInvalid);
        finish();
    }
}

void TopologicalSortVisualizer::clear()
{
    GraphAlgorithmVisualizer::clear();

    topologicalOrder.clear();
}

bool TopologicalSortVisualizer::isDataStructureSupported(const DataStructure *dataStructure, QString &outInfo) const
{
    const Graph* graphPtr = qobject_cast<const Graph*>(dataStructure);
    if(graphPtr->getIsDirected())
    {
        return true;
    };

    outInfo = GraphTexts::UndirectedGraphIsNotSupported;
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
        finish();
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
    QList<int> inDegrees;
    graph->getNodeInDegrees(inDegrees);

    topologicalOrder.reserve(graph->getNodesNum());
    resultEdgeList.reserve(graph->getEdgesNum());

    QQueue<int> nodeQueue;

    for(int i = 0; i < graph->getNodesNum(); ++i)
    {
        if(inDegrees[i] == 0)
        {
            nodeQueue.enqueue(i);
        }
    };

    auto forEachNeighbour = [&](int value, int neighbour, int weight)
    {
        resultEdgeList.add(value, neighbour, true);

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

    graphWidget = qobject_cast<GraphWidget*>(widget);

    graphWidget->setNodeColor(0, Qt::red);
    visualizationTimer.start();
}

void KahnsAlgorithmVisualizer::clear()
{
    GraphAlgorithmVisualizer::clear();

    topologicalOrder.clear();
}

bool KahnsAlgorithmVisualizer::isDataStructureSupported(const DataStructure *dataStructure, QString &outInfo) const
{
    const Graph* graphPtr = qobject_cast<const Graph*>(dataStructure);
    if(graphPtr->getIsDirected())
    {
        return true;
    };

    outInfo = GraphTexts::UndirectedGraphIsNotSupported;
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
        finish();
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

bool SSSPonDAGVisualizer::isDataStructureSupported(const DataStructure *dataStructure, QString &outInfo) const
{
    const Graph* graphPtr = qobject_cast<const Graph*>(dataStructure);
    if(graphPtr->getIsDirected())
    {
        return true;
    };

    outInfo = GraphTexts::UndirectedGraphIsNotSupported;
    return false;
}

void SSSPonDAGVisualizer::topologicalSort(QList<int> &outTopologicalOrder) const
{
    QList<int> inDegrees;
    graph->getNodeInDegrees(inDegrees);

    outTopologicalOrder.reserve(graph->getNodesNum());

    QQueue<int> nodeQueue;

    const qsizetype nodesNum = graph->getNodesNum();

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

bool LazyDijkstraVisualizer::isDataStructureSupported(const DataStructure *dataStructure, QString &outInfo) const
{
    const Graph* graphPtr = qobject_cast<const Graph*>(dataStructure);
    if(graphPtr->getIsDirected())
    {
        return true;
    };

    outInfo = GraphTexts::UndirectedGraphIsNotSupported;
    return false;
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
    , updateStep(0)
    , breakIfNoChange(true)
    , paintNegativeCycleToBlue(true)
{
    setObjectName("Bellman–Ford");

    resultEdgeList.setAllowDuplicates(true);
}

void BellmanFordVisualizer::clear()
{
    PathFindingAlgorithmVisualizer::clear();

    negativeCycle.clear();
    updateStep = 0;
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

            if(breakIfNoChange && !anyChange)
            {
                break;
            }
        }

        if(!breakIfNoChange || breakIfNoChange && anyChange)
        {
            auto dedectNegativeCycle = [&](int value, int neighbour, int weight)
            {
                if(distances[value] + weight < distances[neighbour])
                {
                    distances[neighbour] = -INF;
                    prev[neighbour] = -1;

                    negativeCycle.add(value, neighbour, graph->getIsDirected());
                }
                return true;
            };

            graph->forEachEdge(dedectNegativeCycle);
        }

        buildResultPath(prev);
        startVisualization(widget);
    }
}

void BellmanFordVisualizer::updateVisualization()
{
    if(updateStep == graph->getEdgesNum())
    {
        graphWidget->setNodesAndEdgesToBlack();
        updateStep = 0;
    }
    updateStep++;

    if(!resultEdgeList.empty())
    {
        const Edge edge = resultEdgeList.takeFirst();

        graphWidget->setNodeColor(edge.getStart(), Qt::red, false);
        graphWidget->setNodeColor(edge.getEnd(), Qt::red, false);

        graphWidget->setEdgeColor(edge.getStart(), edge.getEnd(), Qt::red, false);

        if(resultEdgeList.empty())
        {
            if(!resultPath.empty())
            {
                for(int i = 0; i < resultPath.size(); ++i)
                {
                    graphWidget->setNodeColor(resultPath[i], Qt::green, false);

                    if(i + 1 < resultPath.size())
                    {
                        graphWidget->setEdgeColor(resultPath[i], resultPath[i + 1], Qt::green, false);
                    }
                }
            }

            if(paintNegativeCycleToBlue)
            {
                while(!negativeCycle.empty())
                {
                    const Edge edge = negativeCycle.takeFirst();

                    graphWidget->setNodeColor(edge.getStart(), Qt::blue, false);
                    graphWidget->setNodeColor(edge.getEnd(), Qt::blue, false);

                    graphWidget->setEdgeColor(edge.getStart(), edge.getEnd(), Qt::blue, false);
                }
            }
        }

        graphWidget->update();
    }

    if(resultEdgeList.empty())
    {
        finish();
    }
}

bool BellmanFordVisualizer::getBreakIfNoChange() const
{
    return breakIfNoChange;
}

void BellmanFordVisualizer::setBreakIfNoChange(bool newBreakIfNoChange)
{
    if (breakIfNoChange == newBreakIfNoChange)
    {
        return;
    }

    breakIfNoChange = newBreakIfNoChange;
    emit breakIfNoChangeChanged();
}

bool BellmanFordVisualizer::getPaintNegativeCycleToBlue() const
{
    return paintNegativeCycleToBlue;
}

void BellmanFordVisualizer::setPaintNegativeCycleToBlue(bool newPaintNegativeCycleToBlue)
{
    if (paintNegativeCycleToBlue == newPaintNegativeCycleToBlue)
    {
        return;
    }

    paintNegativeCycleToBlue = newPaintNegativeCycleToBlue;
    emit paintNegativeCycleToBlueChanged();
}

FloydWarshallVisualizer::FloydWarshallVisualizer(QObject *parent)
    : PathFindingAlgorithmVisualizer(parent)
    , paintNegativeCycleToBlue(true)
{
    setObjectName("Floyd-Warshall");
}

void FloydWarshallVisualizer::clear()
{
    PathFindingAlgorithmVisualizer::clear();

    negativeCycle.clear();
}

bool FloydWarshallVisualizer::getPaintNegativeCycleToBlue() const
{
    return paintNegativeCycleToBlue;
}

void FloydWarshallVisualizer::setPaintNegativeCycleToBlue(bool newPaintNegativeCycleToBlue)
{
    if(paintNegativeCycleToBlue == newPaintNegativeCycleToBlue)
    {
        return;
    }

    paintNegativeCycleToBlue = newPaintNegativeCycleToBlue;
    emit paintNegativeCycleToBlueChanged();
}

void FloydWarshallVisualizer::run(QWidget *widget)
{
    if(setStartAndEnd())
    {
        const qsizetype nodesNum = graph->getNodesNum();

        QList<QList<int>> dist(nodesNum, QList<int>(nodesNum, INF));
        QList<QList<int>> next(nodesNum, QList<int>(nodesNum, -1));

        resultEdgeList.reserve(nodesNum * nodesNum);

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

                        resultEdgeList.add(i, k, graph->getIsDirected());
                    }
                    else
                    {
                        resultEdgeList.add(i, j, graph->getIsDirected());
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
                        negativeCycle.add(i, j, graph->getIsDirected());
                    }
                }
            }
        }

        int at = start;
        while(at != end && at != -1)
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

        startVisualization(widget);
    }
}

void FloydWarshallVisualizer::updateVisualization()
{
    if(!resultEdgeList.empty())
    {
        const Edge edge = resultEdgeList.takeFirst();

        graphWidget->setNodeColor(edge.getStart(), Qt::red, false);
        graphWidget->setNodeColor(edge.getEnd(), Qt::red, false);

        graphWidget->setEdgeColor(edge.getStart(), edge.getEnd(), Qt::red, false);

        if(resultEdgeList.empty())
        {
            if(!resultPath.empty())
            {
                for(int i = 0; i < resultPath.size(); ++i)
                {
                    graphWidget->setNodeColor(resultPath[i], Qt::green, false);

                    if(i + 1 < resultPath.size())
                    {
                        graphWidget->setEdgeColor(resultPath[i], resultPath[i + 1], Qt::green, false);
                    }
                }
            }

            if(paintNegativeCycleToBlue)
            {
                while(!negativeCycle.empty())
                {
                    const Edge edge = negativeCycle.takeFirst();

                    graphWidget->setNodeColor(edge.getStart(), Qt::blue, false);
                    graphWidget->setNodeColor(edge.getEnd(), Qt::blue, false);

                    graphWidget->setEdgeColor(edge.getStart(), edge.getEnd(), Qt::blue, false);
                }
            }
        }

        graphWidget->update();
    }

    if(resultEdgeList.empty())
    {
        finish();
    }
}

SCCsVisualizer::SCCsVisualizer(QObject *parent)
    : GraphAlgorithmVisualizer(parent)
    , sccCount(0)
    , time(0)
{
    setObjectName("Tarjan's Strongly Connected Component");
}

void SCCsVisualizer::clear()
{
    GraphAlgorithmVisualizer::clear();

    sccCount = 0;
    time = 0;
    SCCs.clear();
    sccStart.clear();
}

static const int UNVISITED = -1;

void SCCsVisualizer::run(QWidget *widget)
{
    if(randomStart)
    {
        setStart(graph->getRandomValue());
    }

    const qsizetype nodesNum = graph->getNodesNum();

    if(start < nodesNum && start >= 0)
    {
        const qsizetype nodesNum = graph->getNodesNum();

        QList<int> low(nodesNum, 0);
        QList<int> visitTime(nodesNum, UNVISITED);
        QStack<int> stack;

        SCCsHelper(start, visitTime, stack, low);

        for(int i = 0; i < nodesNum; i++)
        {
            if(visitTime[i] == UNVISITED)
            {
                SCCsHelper(i, visitTime, stack, low);
            }
        }

        startVisualization(widget);
    }
    else
    {
        showInfo(GraphTexts::StartNodeIsInvalid);
        finish();
    }
}

void SCCsVisualizer::SCCsHelper(int begin, QList<int> &visitTime, QStack<int> &stack, QList<int> &low)
{
    visitTime[begin] = low[begin] = time++;
    stack.push(begin);

    auto forEachNeighbour = [&](int value, int neighbour, int weight)
    {
        resultEdgeList.add(value, neighbour, graph->getIsDirected());

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
        SCCs.append(EdgeList{});

        int prev = begin;
        while(!stack.empty())
        {
            const int popValue = stack.pop();
            low[popValue] = visitTime[begin];

            if(graph->hasEdgeTo(popValue, prev))
            {
                SCCs[sccCount].add(popValue, prev, graph->getIsDirected());
            }
            prev = popValue;

            if(popValue == begin)
            {
                break;
            }
        }

        std::reverse(SCCs[sccCount].begin(), SCCs[sccCount].end());

        sccCount++;
    }
}

void SCCsVisualizer::updateVisualization()
{
    if(!resultEdgeList.empty())
    {
        const Edge edge = resultEdgeList.takeFirst();

        if(graphWidget->getNodeColor(edge.getStart()) == Qt::black)
        {
            graphWidget->setNodeColor(edge.getStart(), Qt::red, false);
        }

        if(graphWidget->getNodeColor(edge.getEnd()) == Qt::black)
        {
            graphWidget->setNodeColor(edge.getEnd(), Qt::red, false);
        }

        if(graphWidget->getEdgeColor(edge.getStart(), edge.getEnd()) == Qt::black)
        {
            graphWidget->setEdgeColor(edge.getStart(), edge.getEnd(), Qt::red, false);
        }

        for(int i = 0; i < SCCs.size(); i++)
        {
            EdgeList& scc = SCCs[i];
            if(!scc.empty())
            {
                if(scc[0].getStart() == edge.getStart())
                {
                    sccStart.push(scc[0].getStart());
                }
                else if(!sccStart.empty() && scc.getLastEdge().getEnd() == edge.getEnd() && sccStart.top() == scc.getLastEdge().getEnd())
                {
                    const QColor& color = sccColors[SCCs.size() % sccColors.size()];
                    while(!scc.empty())
                    {
                        const Edge sccEdge = scc.takeFirst();

                        graphWidget->setNodeColor(sccEdge.getStart(), color, false);
                        graphWidget->setNodeColor(sccEdge.getEnd(), color, false);

                        graphWidget->setEdgeColor(sccEdge.getStart(), sccEdge.getEnd(), color, false);
                    }

                    SCCs.removeAt(i);
                    sccStart.pop();

                    break;
                }
            }
        }

        graphWidget->update();
    }

    if(resultEdgeList.empty())
    {
        finish();
    }
}

TravelingSalesmanProblemVisualizer::TravelingSalesmanProblemVisualizer(QObject *parent)
    : GraphAlgorithmVisualizer(parent)
{
    setObjectName("Traveling Salesman Problem (Held-Karp, Bitmask method)");
}

void TravelingSalesmanProblemVisualizer::run(QWidget *widget)
{
    if(randomStart)
    {
        setStart(graph->getRandomValue());
    }

    const qsizetype nodesNum = graph->getNodesNum();

    if(start < nodesNum && start >= 0)
    {
        QList<QList<int>> memo(nodesNum, QList<int>( 1 << nodesNum, INF));
        for(int i = 0; i < nodesNum; i++)
        {
            if(i == start)
            {
                continue;
            }

            memo[i][1 << start | 1 << i] = graph->getEdgeWeight(start, i);
        }

        for(int r = 3; r <= nodesNum; r++)
        {
            QList<int> combinations;
            generateCombinations(r, combinations);

            for(int combination : combinations)
            {
                if(isNotInCombination(start, combination))
                {
                    continue;
                }

                for(int next = 0; next < nodesNum; next++)
                {
                    if(next == start || isNotInCombination(next, combination))
                    {
                        continue;
                    }

                    const int prevState = combination ^ (1 << next);

                    for(int end = 0; end < nodesNum; end++)
                    {
                        if(end == start || end == next || isNotInCombination(end, combination))
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

        startVisualization(widget);
    }
    else
    {
        showInfo(GraphTexts::StartNodeIsInvalid);
        finish();
    }
}

bool TravelingSalesmanProblemVisualizer::isNotInCombination(int i, int combination) const
{
    return ((1 << i) & combination) == 0;
}

void TravelingSalesmanProblemVisualizer::buildResultPath(const QList<QList<int>> &memo)
{
    const qsizetype nodesNum = graph->getNodesNum();

    int state = ( 1 << nodesNum ) - 1;
    int to = start;

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

        if(last != start && minDistance == INF)
        {
            resultEdgeList.clear();
            break;
        }

        resultEdgeList.add(last, to, graph->getIsDirected());
        state ^= (1 << last);

        to = last;
    }

    std::reverse(resultEdgeList.begin(), resultEdgeList.end());
}

void TravelingSalesmanProblemVisualizer::generateCombinations(int r, QList<int> &combinations) const
{
    generateCombinations(0, 0, r, combinations);
}

void TravelingSalesmanProblemVisualizer::generateCombinations(int subSet, int pos, int r, QList<int> &combinations) const
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

EulerianPathAlgorithmVisualizer::EulerianPathAlgorithmVisualizer(QObject *parent)
    : GraphAlgorithmVisualizer(parent)
{
    setObjectName("Eulerian Path / (Hierholzer's algorithm)");

    hiddenProperties.push_back("start");
    hiddenProperties.push_back("randomStart");
}

void EulerianPathAlgorithmVisualizer::run(QWidget *widget)
{
    QList<int> inDegrees;
    QList<int> outDegrees;

    graph->getNodeDegrees(inDegrees, outDegrees);

    if(doesEulerianPathExist(inDegrees, outDegrees))
    {
        const qsizetype nodesNum = graph->getNodesNum();

        QList<int> eulerianPath;
        eulerianPath.reserve(nodesNum + 1);

        EdgeList visitedUndirectedEdges; // It is only used with an undirected graph
        start = findStart(inDegrees, outDegrees);
        DFSHelper(start, outDegrees, eulerianPath, visitedUndirectedEdges);

        const int edgesNum = graph->getEdgesNum();

        if(eulerianPath.size() != edgesNum + 1)
        {
            finish();
            return;
        }

        for(int p = 1; p < edgesNum + 1; p++)
        {
            resultEdgeList.add(eulerianPath[p-1], eulerianPath[p], graph->getIsDirected());
        }

        startVisualization(widget);
    }
    else
    {
        finish();
    }
}

void EulerianPathAlgorithmVisualizer::DFSHelper(int begin, QList<int> &outDegrees, QList<int> &eulerianPath, EdgeList &visitedUndirectedEdges) const
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

    eulerianPath.push_front(begin);
}

bool EulerianPathAlgorithmVisualizer::doesEulerianPathExist(const QList<int> &inDegrees, const QList<int> &outDegrees) const
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

int EulerianPathAlgorithmVisualizer::findStart(const QList<int> &inDegrees, const QList<int> &outDegrees) const
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


PrimMinimumSpanningTreeAlgorithmVisualizer::PrimMinimumSpanningTreeAlgorithmVisualizer(QObject *parent)
    : GraphAlgorithmVisualizer(parent)
{
    setObjectName("Prim's Minimum Spanning Tree Algorithm (Lazy version)");
}

bool PrimMinimumSpanningTreeAlgorithmVisualizer::isDataStructureSupported(const DataStructure *dataStructure, QString &outInfo) const
{
    const Graph* graphPtr = qobject_cast<const Graph*>(dataStructure);
    if(!graphPtr->getIsDirected())
    {
        return true;
    };

    outInfo = GraphTexts::DirectedGraphIsNotSupported;
    return false;
}

void PrimMinimumSpanningTreeAlgorithmVisualizer::run(QWidget *widget)
{
    using namespace std;

    if(randomStart)
    {
        setStart(graph->getRandomValue());
    }

    const qsizetype nodesNum = graph->getNodesNum();

    if(start < nodesNum && start >= 0)
    {
        QList<bool> visited(nodesNum, false);
        visited[start] = true;

        resultEdgeList.reserve(nodesNum);

        priority_queue<pair<int, Edge>, vector<pair<int, Edge>>, CompareByMinWeight> edgePq;

        auto forEachNeighbour = [&](int value, int neighbour, int weight)
        {
            const Edge e{value, neighbour, false};
            edgePq.emplace(weight, e);
            return true;
        };

        graph->forEachNeighbour(start, forEachNeighbour);

        while(!edgePq.empty())
        {
            Edge edge = edgePq.top().second;
            int node = visited[edge.getStart()] ? edge.getEnd() : edge.getStart();
            while(visited[node] && !edgePq.empty())
            {
                edge = edgePq.top().second;
                edgePq.pop();

                node = visited[edge.getStart()] ? edge.getEnd() : edge.getStart();
            }

            if(visited[node])
            {
                break;
            }

            visited[node] = true;

            resultEdgeList.add(edge);

            graph->forEachNeighbour(node, forEachNeighbour);
        }

        startVisualization(widget);
    }
    else
    {
        showInfo(GraphTexts::StartNodeIsInvalid);
        finish();
    }
}













