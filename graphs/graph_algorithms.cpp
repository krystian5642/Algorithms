
#include "graph_algorithms.h"
#include "graph.h"

#include <QQueue>

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

void BFS::run()
{
    const int start = graph->getRandomValue();

    QQueue<int> nodeQueue;
    nodeQueue.reserve(graph->getNodesNum());
    nodeQueue.enqueue(start);

    QSet<int> visited;
    visited.reserve(graph->getNodesNum());
    visited.insert(start);

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
            }
        }
    }
}

void DFS::run()
{
    const int start = graph->getRandomValue();

    QSet<int> visited;
    visited.reserve(graph->getNodesNum());
    visited.insert(start);

    DFSHelper(start, visited);
}

void DFS::DFSHelper(int begin, QSet<int>& visited)
{
    const QList<int>& neighbourValues = graph->getNeighbourValues(begin);
    for(int neighbour : neighbourValues)
    {
        if(!visited.contains(neighbour))
        {
            visited.insert(neighbour);
            DFSHelper(neighbour, visited);
        }
    }
}