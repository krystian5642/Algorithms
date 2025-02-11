#include "graphalgorithms.h"

void VisitedEdges::insert(int start, int end)
{
    const QPair<int, int> edge(qMin(start, end), qMax(start, end));
    if(!helperSet.contains(edge))
    {
        edges.append(edge);
        helperSet.insert(edge);
    }
}

GraphAlgorithm::GraphAlgorithm()
{

}

GraphAlgorithm::~GraphAlgorithm()
{

}

void BFSGraphAlgorithm::Execute(int start, const Graph<int>& graph, VisitedEdges& visitedEdges)
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

void DFSGraphAlgorithm::Execute(int start, const Graph<int> &graph, VisitedEdges& visitedEdges)
{
    QHash<int, bool> visited;
    visited[start] = true;

    const auto& neighbourValues = graph.getNeighbourValues(start);
    for(const auto& value : neighbourValues)
    {
        visitedEdges.insert(start, value);

        if(!visited[value])
        {
            visited[value] = true;
            DFSHelper(value, graph, visited, visitedEdges);
        }
    }
}

void DFSGraphAlgorithm::DFSHelper(int start, const Graph<int> &graph, QHash<int, bool> &visited, VisitedEdges& visitedEdges)
{
    const auto& neighbourValues = graph.getNeighbourValues(start);
    for(const auto& value : neighbourValues)
    {
        visitedEdges.insert(start, value);

        if(!visited[value])
        {
            visited[value] = true;
            DFSHelper(value, graph, visited, visitedEdges);
        }
    }
}
