
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
    const int begin = randomStart ? graph->getRandomValue() : start;

    if(begin < graph->getVerticesNum())
    {
        QQueue<int> nodeQueue;
        nodeQueue.reserve(graph->getVerticesNum());

        QList<bool> visited;
        visited.fill(false, graph->getVerticesNum());

        resultEdgeList.reserve(graph->getEdgesNum());

        nodeQueue.enqueue(begin);
        visited[begin] = true;

        auto func = [&](int start, int neighbour, int weight)
        {
            if(!visited[neighbour])
            {
                visited[neighbour] = true;
                nodeQueue.enqueue(neighbour);
            }

            resultEdgeList.add(start, neighbour, graph->getIsDirected());
        };

        while(!nodeQueue.empty())
        {
            const int first = nodeQueue.dequeue();
            graph->forEachNeighbor(first, func);
        }

        graphWidget = qobject_cast<GraphWidget*>(widget);
        graphWidget->setNodeColor(begin, Qt::red);

        visualizationTimer.start();
    }
    else
    {
        showStartIsInvalidInfo();
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
    const int begin = randomStart ? graph->getRandomValue() : start;

    if(begin < graph->getVerticesNum())
    {
        QList<int> visited;
        visited.fill(false, graph->getVerticesNum());

        resultEdgeList.reserve(graph->getEdgesNum());

        DFSHelper(begin, visited);

        graphWidget = qobject_cast<GraphWidget*>(widget);
        graphWidget->setNodeColor(begin, Qt::red);
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
    };

    graph->forEachNeighbor(begin, func);
}
