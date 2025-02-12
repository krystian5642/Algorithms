#include "graphAlgorithms.h"
#include "graphwidget.h"

#include <QTimer>

GraphAlgorithm::GraphAlgorithm(const Graph<int> &inGraph)
    : graph(inGraph)
    , showEdgeInterval(1000)
    , resultIndex(0)
{
    start = 1;//graph.getRandomValue();

    showEdgesTimer = new QTimer(this);
    showEdgesTimer->setInterval(showEdgeInterval);
}

GraphAlgorithm::GraphAlgorithm(int inStart, const Graph<int>& inGraph)
    : start(inStart)
    , graph(inGraph)
    , showEdgeInterval(1000)
    , resultIndex(0)
{
    showEdgesTimer = new QTimer(this);
    showEdgesTimer->setInterval(showEdgeInterval);
}

GraphAlgorithm::~GraphAlgorithm()
{

}

void GraphAlgorithm::showResult(GraphWidget *widget)
{
    widget->setNodeColor(start, Qt::red);
    connect(showEdgesTimer, &QTimer::timeout, this, [this, widget]()
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

    showEdgesTimer->start();
}

void GraphAlgorithm::stop()
{
    if(showEdgesTimer)
    {
        showEdgesTimer->stop();
        showEdgesTimer->deleteLater();
        showEdgesTimer = nullptr;
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
            visitedEdges.insert(first, value);

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
        visitedEdges.insert(start, value);

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
        visitedEdges.insert(begin, value);

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
    start = 1;
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
            visitedEdges.insert(first, value);

            if(!visited[value])
            {
                visited[value] = true;
                nodeQueue.enqueue(value);

                prev[value] = first;
                if(value == end)
                {
                    nodeQueue = QQueue<int>{};
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
            path.insert(step, prev[step]);
            step = prev[step];
        }
    }
}

void BFSShortestPath::showResult(GraphWidget *widget)
{
    widget->setNodeColor(start, Qt::red);
    connect(showEdgesTimer, &QTimer::timeout, this, [this, widget]()
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

    showEdgesTimer->start();
}





