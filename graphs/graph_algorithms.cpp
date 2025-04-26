
#include "graph_algorithms.h"
#include "graph.h"

#include <QElapsedTimer>
#include <QQueue>
#include <QRandomGenerator>

GraphAlgorithm::GraphAlgorithm(QObject *parent)
    : Algorithm(parent)
{

}

void GraphAlgorithm::run()
{
    int runNum = 800;

    QList<QPointF> result;
    result.reserve(runNum);

    static qreal timeout = 30000000000; //ns
    qreal totalTime = 0;

    for(int i = 1; i <= runNum; i++)
    {
        Graph testGraph;

        for(int j = 0; j < i; j++)
        {
            const int randomValueX = QRandomGenerator::global()->bounded(1000000);
            const int randomValueY = QRandomGenerator::global()->bounded(1000000);

            testGraph.addEdge(randomValueX, testGraph.getRandomValue());
            testGraph.addEdge(randomValueY, testGraph.getRandomValue());
        }

        setGraph(&testGraph);

        QElapsedTimer executionTime;
        executionTime.start();

        execute();

        const qreal elapsedTime = static_cast<qreal>(executionTime.nsecsElapsed());
        const qreal edgesAndNodesNum = static_cast<qreal>(testGraph.getEdgesNum() + testGraph.getNodesNum());

        const QPointF point(edgesAndNodesNum, elapsedTime);
        result.append(point);

        // timeout check
        totalTime += elapsedTime;
        if(totalTime > timeout)
        {
            break;
        }
    }

    emit finished(result);
}

GraphAlgorithm::~GraphAlgorithm()
{

}

const Graph *GraphAlgorithm::getGraph() const
{
    return graph;
}

void GraphAlgorithm::setGraph(const Graph *newGraph)
{
    graph = newGraph;
}

BFSIterative::BFSIterative(QObject *parent)
    : GraphAlgorithm(parent)
{
    setObjectName("Breadth First Search (Iterative ?)");
}

void BFSIterative::execute()
{
    const int start = graph->getFirstValue();

    QQueue<int> nodeQueue;
    nodeQueue.reserve(graph->getNodesNum());
    nodeQueue.enqueue(start);

    QSet<int> visited;
    visited.reserve(graph->getNodesNum());
    visited.insert(start);

    while(!nodeQueue.empty())
    {
        const int first = nodeQueue.dequeue();
        const QList<GraphEdge>& neighbourEdges = graph->getNeighbourEdges(first);

        for(const auto& neighbour : neighbourEdges)
        {
            const int value = neighbour.getEndValue();
            if(!visited.contains(value))
            {
                visited.insert(value);
                nodeQueue.enqueue(value);
            }
        }
    }
}

DFSRecursive::DFSRecursive(QObject *parent)
    : GraphAlgorithm(parent)
{
    setObjectName("Depth First Search (Recursive)");
}

void DFSRecursive::execute()
{
    const int start = graph->getFirstValue();

    QSet<int> visited;
    visited.reserve(graph->getNodesNum());
    visited.insert(start);

    DFSHelper(start, visited);
}

void DFSRecursive::DFSHelper(int begin, QSet<int>& visited)
{
    const QList<GraphEdge>& neighbourEdges = graph->getNeighbourEdges(begin);
    for(const auto& neighbour : neighbourEdges)
    {
        const int value = neighbour.getEndValue();
        if(!visited.contains(value))
        {
            visited.insert(value);
            DFSHelper(value, visited);
        }
    }
}
