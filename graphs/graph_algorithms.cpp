
#include "graph_algorithms.h"
#include "graph.h"

#include <windows.h>

#include <QElapsedTimer>
#include <QRandomGenerator>

GraphAlgorithm::GraphAlgorithm(QObject *parent)
    : Algorithm(parent)
{

}

void GraphAlgorithm::run()
{
    int runNum = 1000;
    QList<QPointF> result;
    result.reserve(runNum);

    static qreal timeout = 30000000000; //ns
    qreal totalTime = 0;

    for(int i = 1; i <= runNum; i++)
    {
        Graph testGraph;

        for(int j = 0; j < i; j++)
        {
            testGraph.addEdge(i, j);
        }

        setGraph(&testGraph);
        init();

        ULONG64 start;
        QueryThreadCycleTime(GetCurrentThread(), &start);

        //QElapsedTimer time;
        //time.start();

        execute();

        ULONG64 end;
        QueryThreadCycleTime(GetCurrentThread(), &end);

        const qreal elapsedTime = static_cast<qreal>(end - start);
        const qreal edgesAndNodesNum = static_cast<qreal>(testGraph.getEdgesNum() + testGraph.getNodesNum());

        const QPointF point(edgesAndNodesNum, elapsedTime);
        result.append(point);

        clear();

        // timeout check
        totalTime += elapsedTime;
        if(totalTime > timeout)
        {
            break;
        }
    }

    emit finished(result);
}

void GraphAlgorithm::clear()
{
    graph = nullptr;
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

void BFSIterative::init()
{
    GraphAlgorithm::init();

    nodeQueue.reserve(graph->getNodesNum());
    visited.fill(false, graph->getNodesNum());
}

void BFSIterative::clear()
{
    GraphAlgorithm::clear();

    nodeQueue.clear();
    visited.clear();
}

void BFSIterative::execute()
{
    const GraphNode* start = graph->getFirstNode();

    nodeQueue.enqueue(start);
    visited[start->getValue()] = true;

    while(!nodeQueue.empty())
    {
        const GraphNode* first = nodeQueue.dequeue();
        const auto& children = first->getChildren();

        for(const auto& child : children)
        {
            const GraphNode* endNode = child->getEndNode();
            if(!visited[endNode->getValue()])
            {
                visited[endNode->getValue()] = true;
                nodeQueue.enqueue(endNode);
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
    // const int start = graph->getFirstValue();

    // QSet<int> visited;
    // visited.reserve(graph->getNodesNum());
    // visited.insert(start);

    // DFSHelper(start, visited);
}

void DFSRecursive::DFSHelper(int begin, QSet<int>& visited)
{
    // const QList<GraphEdge>& neighbourEdges = graph->getNeighbourEdges(begin);
    // for(const auto& neighbour : neighbourEdges)
    // {
    //     const int value = neighbour.getEndValue();
    //     if(!visited.contains(value))
    //     {
    //         visited.insert(value);
    //         DFSHelper(value, visited);
    //     }
    // }
}
