#include "graphalgorithms.h"

#include <QThread>

GraphAlgorithm::GraphAlgorithm()
{

}

GraphAlgorithm::~GraphAlgorithm()
{

}

void BFSGraphAlgorithm::Execute(int start, const Graph<int>& graph, QList<int>& visitedNodes, QSet<QPair<int, int>>& visitedEdges)
{
    QQueue<int> nodeQueue;
    nodeQueue.enqueue(start);

    QHash<int, bool> visited;
    visited[start] = true;

    while(!nodeQueue.empty())
    {
        const int first = nodeQueue.dequeue();
        const auto& neighbourValues = graph.getNeighbourValues(first);

        visitedNodes.push_back(first);

        for(const auto& value : neighbourValues)
        {
            const QPair<int, int> edge(qMin(first, value), qMax(first, value));
            visitedEdges.insert(edge);

            if(!visited[value])
            {
                visited[value] = true;
                nodeQueue.enqueue(value);
            }
        }
    }
}








