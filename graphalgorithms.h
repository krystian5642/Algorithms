#ifndef GRAPHALGORITHMS_H
#define GRAPHALGORITHMS_H

#include "graph.h"

#include <QList>
#include <QQueue>

template <class ValueType>
class Graph;

template <class ValueType>
class GraphEdge;

struct VisitedEdges
{
    void insert(int start, int end);
private:
    using GraphEdge = QPair<int, int>;

    QSet<GraphEdge> helperSet;
    QList<GraphEdge> edges;

public:
    // to support range-based operation for loop
    inline QList<GraphEdge>::iterator        begin() { return edges.begin(); }
    inline QList<GraphEdge>::const_iterator  constBegin() const { return edges.constBegin(); }
    inline QList<GraphEdge>::iterator        end() { return edges.end(); }
    inline QList<GraphEdge>::const_iterator  constEnd() const { return edges.constEnd(); }
};

class GraphAlgorithm
{
public:
    explicit GraphAlgorithm();
    virtual ~GraphAlgorithm();

    virtual void Execute(int start, const Graph<int>& graph, VisitedEdges& visitedEdges) = 0;

signals:
    void onNodeVisited(int value);
    void onEdgeVisited(int start, int end);
};

class BFSGraphAlgorithm : public GraphAlgorithm
{
public:
    virtual void Execute(int start, const Graph<int>& graph, VisitedEdges& visitedEdges) override;
};

class DFSGraphAlgorithm : public GraphAlgorithm
{
public:
    virtual void Execute(int start, const Graph<int>& graph, VisitedEdges& visitedEdges) override;

private:
    void DFSHelper(int start, const Graph<int>& graph, QHash<int, bool>& visited, VisitedEdges& visitedEdges);
};

#endif // GRAPHALGORITHMS_H
