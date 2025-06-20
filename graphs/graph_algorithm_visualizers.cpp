
#include "graph_algorithm_visualizers.h"
#include "widgets/graph_widget.h"

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

void GraphAlgorithmVisualizer::showStartIsInvalidInfo()
{
    QMessageBox::information(qobject_cast<QWidget*>(parent()), "Info", "Choosen start is too large or negative");
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

    if(start < graph->getNodesNum())
    {
        QQueue<int> nodeQueue;
        nodeQueue.reserve(graph->getNodesNum());

        QList<bool> visited;
        visited.fill(false, graph->getNodesNum());

        resultEdgeList.reserve(graph->getEdgesNum());

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
        showStartIsInvalidInfo();
        emit finished();
    }
}

BFSShortestPathVisualizer::BFSShortestPathVisualizer(QObject *parent)
    : GraphAlgorithmVisualizer(parent)
    , end(1)
    , randomEnd(false)
{
    setObjectName("Breadth First Search Shortest Path");
}

void BFSShortestPathVisualizer::run(QWidget *widget)
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

    if(start < nodesNum && end < nodesNum)
    {
        QQueue<int> nodeQueue;
        nodeQueue.reserve(nodesNum);

        QList<bool> visited;
        visited.fill(false, nodesNum);

        resultEdgeList.reserve(graph->getEdgesNum());

        nodeQueue.enqueue(start);
        visited[start] = true;

        QList<int> prev;
        prev.fill(INT_MIN, nodesNum);

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

        resultPath.push_back(end);

        int prevValue = end;
        while(prevValue != INT_MIN)
        {
            resultPath.push_back(prev[prevValue]);
            prevValue = prev[prevValue];
        }

        std::reverse(resultPath.begin(), resultPath.end());

        graphWidget = qobject_cast<GraphWidget*>(widget);
        graphWidget->setNodeColor(start, Qt::red);

        visualizationTimer.start();
    }
    else
    {
        showStartIsInvalidInfo();
        emit finished();
    }
}

void BFSShortestPathVisualizer::clear()
{
    GraphAlgorithmVisualizer::clear();

    resultPath.clear();
}

int BFSShortestPathVisualizer::getEnd() const
{
    return end;
}

void BFSShortestPathVisualizer::setEnd(int newEnd)
{
    if (end == newEnd)
    {
        return;
    }

    end = newEnd;
    emit endChanged();
}

bool BFSShortestPathVisualizer::getRandomEnd() const
{
    return randomEnd;
}

void BFSShortestPathVisualizer::setRandomEnd(bool newRandomEnd)
{
    if (randomEnd == newRandomEnd)
    {
        return;
    }

    randomEnd = newRandomEnd;
    emit randomEndChanged();
}

void BFSShortestPathVisualizer::updateVisualization()
{
    if(!resultEdgeList.empty())
    {
        const Edge edge = resultEdgeList.takeFirst();

        graphWidget->setNodeColor(edge.getStart(), Qt::red, false);
        graphWidget->setNodeColor(edge.getEnd(), Qt::red, false);

        graphWidget->setEdgeColor(edge.getStart(), edge.getEnd(), Qt::red);

        if(!resultPath.empty() && edge.getEnd() == resultPath.last())
        {
            for(int i = 1; i < resultPath.size(); ++i)
            {
                graphWidget->setNodeColor(resultPath[i], Qt::green, false);

                if(i + 1 < resultPath.size())
                {
                    graphWidget->setEdgeColor(resultPath[i], resultPath[i + 1], Qt::green, false);
                }
            }
            graphWidget->update();
        }
    }

    if(resultEdgeList.empty())
    {
        clear();
        emit finished();
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
        QList<bool> visited;
        visited.fill(false, graph->getNodesNum());

        resultEdgeList.reserve(graph->getEdgesNum());

        DFSHelper(start, visited);

        graphWidget = qobject_cast<GraphWidget*>(widget);
        graphWidget->setNodeColor(start, Qt::red);
        visualizationTimer.start();
    }
    else
    {
        showStartIsInvalidInfo();
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
        QList<bool> visited;
        visited.fill(false, nodesNum);

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
        showStartIsInvalidInfo();
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
