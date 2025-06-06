
#include "graph_algorithm_visualizers.h"
#include "widgets/graph_widget.h"

#include <QMessageBox>
#include <QQueue>

GraphAlgorithmVisualizer::GraphAlgorithmVisualizer(QObject *parent)
    : AlgorithmVisualizer(parent)
    , start(0)
    , randomStart(false)
    , graph(nullptr)
    , resultIndex(0)
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
    resultIndex = 0;
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
    if(resultEdgeList.isValidIndex(resultIndex))
    {
        graphWidget->setNodeColor(resultEdgeList[resultIndex].getStart(), Qt::red, false);
        graphWidget->setNodeColor(resultEdgeList[resultIndex].getEnd(), Qt::red, false);

        graphWidget->setEdgeColor(resultEdgeList[resultIndex].getStart(), resultEdgeList[resultIndex].getEnd(), Qt::red);

        resultIndex++;
    }
    else
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

    if(start < graph->getVerticesNum())
    {
        QQueue<int> nodeQueue;
        nodeQueue.reserve(graph->getVerticesNum());

        QList<bool> visited;
        visited.fill(false, graph->getVerticesNum());

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
            graph->forEachNeighbor(first, func);
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

    const qsizetype verticesNum = graph->getVerticesNum();

    if(start < verticesNum && end < verticesNum)
    {
        QQueue<int> nodeQueue;
        nodeQueue.reserve(verticesNum);

        QList<bool> visited;
        visited.fill(false, verticesNum);

        resultEdgeList.reserve(graph->getEdgesNum());

        nodeQueue.enqueue(start);
        visited[start] = true;

        QList<int> prev;
        prev.fill(INT_MIN, verticesNum);

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
            graph->forEachNeighbor(first, func);
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
    if(resultEdgeList.isValidIndex(resultIndex))
    {
        graphWidget->setNodeColor(resultEdgeList[resultIndex].getStart(), Qt::red, false);
        graphWidget->setNodeColor(resultEdgeList[resultIndex].getEnd(), Qt::red, false);

        graphWidget->setEdgeColor(resultEdgeList[resultIndex].getStart(), resultEdgeList[resultIndex].getEnd(), Qt::red);

        if(!resultPath.isEmpty() && resultEdgeList[resultIndex].getEnd() == resultPath.last())
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

            clear();
            emit finished();
        }
        else
        {
            resultIndex++;
        }
    }
    else
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

    if(start < graph->getVerticesNum())
    {
        QList<int> visited;
        visited.fill(false, graph->getVerticesNum());

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

void DFSVisualizer::DFSHelper(int begin, QList<int>& visited)
{
    auto func = [&](int start, int neighbour, int weight)
    {
        resultEdgeList.add(start, neighbour, graph->getIsDirected());

        if(!visited[neighbour])
        {
            visited[neighbour] = true;
            DFSHelper(neighbour, visited);
        }
        return true;
    };

    graph->forEachNeighbor(begin, func);
}
