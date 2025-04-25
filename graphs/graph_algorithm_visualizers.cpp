
#include "graph_algorithm_visualizers.h"
#include "widgets/graph_widget.h"

#include <QQueue>

GraphAlgorithmVisualizer::GraphAlgorithmVisualizer(QObject *parent)
    : AlgorithmVisualizer(parent)
{

}

GraphAlgorithmVisualizer::~GraphAlgorithmVisualizer()
{
}

void GraphAlgorithmVisualizer::reset()
{
    AlgorithmVisualizer::reset();

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

const Graph *GraphAlgorithmVisualizer::getGraph() const
{
    return graph;
}

void GraphAlgorithmVisualizer::setGraph(const Graph *newGraph)
{
    graph = newGraph;
}

BFSVisualizer::BFSVisualizer(QObject *parent)
    : GraphAlgorithmVisualizer(parent)
{
    setObjectName("Breadth First Search");
}

void BFSVisualizer::run(QWidget *widget)
{
    GraphWidget* graphWidget = qobject_cast<GraphWidget*>(widget);
    Q_CHECK_PTR(graphWidget);

    QQueue<int> nodeQueue;
    nodeQueue.reserve(graph->getNodesNum());
    nodeQueue.enqueue(start);

    QSet<int> visited;
    visited.reserve(graph->getNodesNum());
    visited.insert(start);

    resultEdgeList.reserve(graph->getEdgesNum());

    while(!nodeQueue.empty())
    {
        const int first = nodeQueue.dequeue();
        const QList<int>& neighbourValues = graph->getNeighbourValues(first);

        for(int neighbour : neighbourValues)
        {
            if(!visited.contains(neighbour))
            {
                visited.insert(neighbour);
                nodeQueue.enqueue(neighbour);

                resultEdgeList.add(first, neighbour);
            }
        }
    }

    graphWidget->setNodeColor(start, Qt::red);
    connect(&visualizationTimer, &QTimer::timeout, this, [this, graphWidget]()
    {
        if(resultEdgeList.isValidIndex(resultIndex))
        {
            graphWidget->setNodeColor(resultEdgeList[resultIndex].first, Qt::red, false);
            graphWidget->setNodeColor(resultEdgeList[resultIndex].second, Qt::red, false);

            graphWidget->setEdgeColor(resultEdgeList[resultIndex].first, resultEdgeList[resultIndex].second, Qt::red);

            resultIndex++;
        }
        else
        {
            reset();
        }
    });

    visualizationTimer.start();
}

DFSVisualizer::DFSVisualizer(QObject *parent)
    : GraphAlgorithmVisualizer(parent)
{
    setObjectName("Depth First Search");
}

void DFSVisualizer::run(QWidget *widget)
{
    GraphWidget* graphWidget = qobject_cast<GraphWidget*>(widget);
    Q_CHECK_PTR(graphWidget);

    visited.reserve(graph->getNodesNum());
    visited.insert(start);

    resultEdgeList.reserve(graph->getEdgesNum());

    DFSHelper(start);

    graphWidget->setNodeColor(start, Qt::red);
    connect(&visualizationTimer, &QTimer::timeout, this, [this, graphWidget]()
    {
        if(resultEdgeList.isValidIndex(resultIndex))
        {
            graphWidget->setNodeColor(resultEdgeList[resultIndex].first, Qt::red, false);
            graphWidget->setNodeColor(resultEdgeList[resultIndex].second, Qt::red, false);

            graphWidget->setEdgeColor(resultEdgeList[resultIndex].first, resultEdgeList[resultIndex].second, Qt::red);

            resultIndex++;
        }
        else
        {
            reset();
        }
    });

    visualizationTimer.start();
}

void DFSVisualizer::reset()
{
    GraphAlgorithmVisualizer::reset();

    visited.clear();
}

void DFSVisualizer::DFSHelper(int begin)
{
    const QList<int>& neighbourValues = graph->getNeighbourValues(begin);
    for(int neighbour : neighbourValues)
    {
        if(!visited.contains(neighbour))
        {
            visited.insert(neighbour);

            resultEdgeList.add(begin, neighbour);

            DFSHelper(neighbour);
        }
    }
}
