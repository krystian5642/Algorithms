#include "graphAlgorithms.h"
#include "graphwidget.h"

#include <QTimer>

GraphAlgorithm::GraphAlgorithm(const Graph<int> &inGraph)
    : graph(inGraph)
    , showEdgeInterval(1000)
    , resultIndex(0)
{
    start = graph.getRandomValue();

    showResultTimer = new QTimer(this);
    showResultTimer->setInterval(showEdgeInterval);
}

GraphAlgorithm::GraphAlgorithm(int inStart, const Graph<int>& inGraph)
    : start(inStart)
    , graph(inGraph)
    , showEdgeInterval(1000)
    , resultIndex(0)
{
    showResultTimer = new QTimer(this);
    showResultTimer->setInterval(showEdgeInterval);
}

GraphAlgorithm::~GraphAlgorithm()
{
}

void GraphAlgorithm::showResult(GraphWidget *widget)
{
    widget->setNodeColor(start, Qt::red);
    connect(showResultTimer, &QTimer::timeout, this, [this, widget]()
    {
        if(visitedEdges.isValidIndex(resultIndex))
        {
            widget->setNodeColor(visitedEdges[resultIndex].first, Qt::red, false);
            widget->setNodeColor(visitedEdges[resultIndex].second, Qt::red, false);

            widget->setEdgeColor(visitedEdges[resultIndex].first, visitedEdges[resultIndex].second, Qt::red);

            resultIndex++;
        }
        else
        {
            emit onShowResultFinished();
        }
    });

    showResultTimer->start();
}

void GraphAlgorithm::close()
{
    if(showResultTimer)
    {
        showResultTimer->stop();
        showResultTimer->deleteLater();
        showResultTimer = nullptr;
    }
}

void GraphAlgorithm::setPause(bool pause)
{
    if(showResultTimer)
    {
        if(pause)
        {
            showResultTimer->stop();
        }
        else
        {
            showResultTimer->start();
        }
    }
}

BFS::BFS(const Graph<int> &inGraph)
    : GraphAlgorithm(inGraph)
{

}

void BFS::execute()
{
    QQueue<int> nodeQueue;
    nodeQueue.enqueue(start);

    QHash<int, bool> visited;
    visited[start] = true;

    while(!nodeQueue.empty())
    {
        const int first = nodeQueue.dequeue();
        const auto& neighbourValues = graph.getNeighbourValues(first);

        for(const auto& value : neighbourValues)
        {
            visitedEdges.add(first, value);

            if(!visited[value])
            {
                visited[value] = true;               
                nodeQueue.enqueue(value);
            }
        }
    }
}

DFS::DFS(const Graph<int> &inGraph)
    : GraphAlgorithm(inGraph)
{

}

void DFS::execute()
{  
    visited[start] = true;

    const auto& neighbourValues = graph.getNeighbourValues(start);
    for(const auto& value : neighbourValues)
    {
        visitedEdges.add(start, value);

        if(!visited[value])
        {
            visited[value] = true;
            DFSHelper(value);
        }
    }
}

void DFS::DFSHelper(int begin)
{
    const auto& neighbourValues = graph.getNeighbourValues(begin);
    for(const auto& value : neighbourValues)
    {
        visitedEdges.add(begin, value);

        if(!visited[value])
        {
            visited[value] = true;
            DFSHelper(value);
        }
    }
}

BFSShortestPath::BFSShortestPath(const Graph<int> &inGraph)
    : GraphAlgorithm(inGraph)
    , end(5)
{
    end = graph.getRandomValue();
}

void BFSShortestPath::execute()
{
    QQueue<int> nodeQueue;
    nodeQueue.enqueue(start);

    QHash<int, bool> visited;
    visited[start] = true;

    constexpr int intMin = std::numeric_limits<int>::min();

    QHash<int, int> prev;
    prev[start] = intMin;

    while(!nodeQueue.empty())
    {
        const int first = nodeQueue.dequeue();
        const auto& neighbourValues = graph.getNeighbourValues(first);

        for(const auto& value : neighbourValues)
        {
            visitedEdges.add(first, value);

            if(!visited[value])
            {
                visited[value] = true;
                nodeQueue.enqueue(value);

                prev[value] = first;
                if(value == end)
                {
                    nodeQueue.clear();
                    break;
                }
            }
        }
    }

    if(prev.contains(end))
    {
        int step = end;
        while(step != intMin)
        {
            path.add(step, prev[step]);
            step = prev[step];
        }

        std::reverse(path.begin(), path.end());
    }
}

void BFSShortestPath::showResult(GraphWidget *widget)
{
    widget->setNodeColor(start, Qt::red);
    connect(showResultTimer, &QTimer::timeout, this, [this, widget]()
    {
        if(visitedEdges.isValidIndex(resultIndex))
        {
            if(visitedEdges[resultIndex].first == end || visitedEdges[resultIndex].second == end)
            {
                for(auto it = path.constBegin(); it != path.constEnd(); it++)
                {
                    widget->setNodeColor(it->first, Qt::green, false);
                    widget->setNodeColor(it->second, Qt::green, false);

                    widget->setEdgeColor(it->first, it->second, Qt::green);
                }

                emit onShowResultFinished();
                return;
            }

            widget->setNodeColor(visitedEdges[resultIndex].first, Qt::red, false);
            widget->setNodeColor(visitedEdges[resultIndex].second, Qt::red, false);

            widget->setEdgeColor(visitedEdges[resultIndex].first, visitedEdges[resultIndex].second, Qt::red);

            resultIndex++;
        }
        else
        {
            emit onShowResultFinished();
        }
    });

    showResultTimer->start();
}

TreeCenters::TreeCenters(const Graph<int> &inGraph)
    : GraphAlgorithm(inGraph)
{

}

void TreeCenters::execute()
{
    const qsizetype nodesNum = graph.getNodesNum();

    QHash<int, qsizetype> nodeDegrees;
    nodeDegrees.reserve(nodesNum);

    QList<int> leaves;

    for(auto it = graph.constBegin(); it != graph.constEnd(); it++)
    {
        const auto& nodeValue = it.key();
        const auto& nodeNeighbours = it.value();

        qsizetype degree = nodeNeighbours.size();
        if(degree == 0 || degree == 1)
        {
            leaves.append(nodeValue);
            degree = 0;
        }

        nodeDegrees[nodeValue] = degree;
    }

    qsizetype count = leaves.size();
    bool canFindCenters = count != 0;

    while(count < nodesNum)
    {
        if(!canFindCenters)
        {
            return;
        }

        leavesLayerOrder.append(leaves);

        QList<int> newLeaves;
        for(const auto& leaf : leaves)
        {
            const auto& nodeNeighbours = graph.getNeighbourValues(leaf);
            for(const auto& neighbour : nodeNeighbours)
            {
                auto& degree = nodeDegrees[neighbour];
                if(degree > 0)
                {
                    degree--;

                    if(degree == 1)
                    {
                        newLeaves.append(neighbour);
                    }
                }
            }
            nodeDegrees[leaf] = 0;
        }
        leaves = newLeaves;
        count += leaves.size();

        canFindCenters = leaves.size() != 0;
    }
    centers = leaves;
}

void TreeCenters::showResult(GraphWidget *widget)
{
    auto timeoutShowResult = [this, widget]()
    {
        if(leavesLayerOrder.size() > resultIndex)
        {
            for(const auto& leaf : leavesLayerOrder[resultIndex])
            {
                widget->setNodeColor(leaf, Qt::gray, false);

                const auto& nodeNeighbours = graph.getNeighbourValues(leaf);
                for(const auto& neighbour : nodeNeighbours)
                {
                    widget->setEdgeColor(leaf, neighbour, Qt::gray, false);
                }
            }

            resultIndex++;
            widget->update();
        }
        else if(!centers.empty())
        {
            for(const auto& center : centers)
            {
                widget->setNodeColor(center, Qt::green, false);
            }

            if(centers.size() == 2)
            {
                widget->setEdgeColor(centers[0], centers[1], Qt::green, false);
            }

            widget->update();

            emit onShowResultFinished();
        }
        else
        {
            emit onShowResultFinished();
        }
    };

    connect(showResultTimer, &QTimer::timeout, this, timeoutShowResult);

    timeoutShowResult();
    showResultTimer->start();
}

void TreeCenters::showResultImplementation()
{

}




