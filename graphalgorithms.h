#ifndef GRAPHALGORITHMS_H
#define GRAPHALGORITHMS_H

#include "graph.h"

#include <QList>
#include <QQueue>

template <class ValueType>
class Graph;

template <class ValueType>
class GraphEdge;

template <class ValueType>
class GraphAlgorithm
{
public:
    GraphAlgorithm() = default;
    virtual ~GraphAlgorithm() = default;

    virtual void Execute(const ValueType& start, const Graph<ValueType>& graph, QList<GraphEdge<ValueType>>& result) const = 0;
};

template <class ValueType>
class BFSGraphAlgorithm : public GraphAlgorithm<ValueType>
{
public:
    virtual void Execute(const ValueType& start, const Graph<ValueType>& graph, QList<GraphEdge<ValueType>>& result) const override;
};

template<class ValueType>
void BFSGraphAlgorithm<ValueType>::Execute(const ValueType &start, const Graph<ValueType> &graph, QList<GraphEdge<ValueType>> &result) const
{
    QQueue<ValueType> nodeQueue;
    nodeQueue.enqueue(start);

    QHash<ValueType, bool> visited;
    visited[start] = true;

    while(!nodeQueue.empty())
    {
        const ValueType first = nodeQueue.dequeue();
        const auto& graphContainer = graph.getGraphContainer();

        for(const auto& edge : graphContainer[first])
        {
            if(!visited[edge.getEndValue()])
            {
                visited[edge.getEndValue()] = true;
                nodeQueue.enqueue(edge.getEndValue());
                result.append(edge);
            }
        }
    }
}

#endif // GRAPHALGORITHMS_H
