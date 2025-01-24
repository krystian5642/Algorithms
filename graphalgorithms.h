#ifndef GRAPHALGORITHMS_H
#define GRAPHALGORITHMS_H

#include "graph.h"

#include <QList>
#include <QQueue>

template <class ValueType>
class Graph;

template <class ValueType>
class GraphEdge;

class GraphAlgorithm
{
public:
    explicit GraphAlgorithm();
    virtual ~GraphAlgorithm();

    virtual void Execute(int start, const Graph<int>& graph, QList<int>& visitedNodes, QSet<QPair<int, int>>& visitedEdges) = 0;

signals:
    void onNodeVisited(int value);
    void onEdgeVisited(int start, int end);
};

class BFSGraphAlgorithm : public GraphAlgorithm
{
public:
    virtual void Execute(int start, const Graph<int>& graph, QList<int>& visitedNodes, QSet<QPair<int, int>>& visitedEdges) override;
};

#endif // GRAPHALGORITHMS_H
