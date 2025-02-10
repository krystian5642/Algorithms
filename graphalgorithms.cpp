#include "graphalgorithms.h"

#include <QThread>

GraphAlgorithm::GraphAlgorithm()
{

}

GraphAlgorithm::~GraphAlgorithm()
{

}

void BFSGraphAlgorithm::Execute(int start, const Graph<int>& graph, QList<QPair<int, int>>& visitedEdges)
{
    QQueue<int> nodeQueue;
    nodeQueue.enqueue(start);

    QHash<int, bool> visited;
    visited[start] = true;

    QSet<QPair<int, int>> helperEdgeSet;
    auto addToVisitedEdges = [&](const QPair<int, int> edge)
    {
        if(!helperEdgeSet.contains(edge))
        {
            visitedEdges.append(edge);
            helperEdgeSet.insert(edge);
        }
    };

    while(!nodeQueue.empty())
    {
        const int first = nodeQueue.dequeue();
        const auto& neighbourValues = graph.getNeighbourValues(first);

        for(const auto& value : neighbourValues)
        {
            const QPair<int, int> edge(qMin(first, value), qMax(first, value));
            addToVisitedEdges(edge);

            if(!visited[value])
            {
                visited[value] = true;               
                nodeQueue.enqueue(value);
            }
        }
    }
}








