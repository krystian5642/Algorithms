#ifndef GRAPHAlgorithmS_H
#define GRAPHAlgorithmS_H

#include "graph.h"
#include "edgelist.h"

#include <QList>
#include <QQueue>
#include <QObject>

template <class ValueType>
class Graph;

template <class ValueType>
class GraphEdge;

class GraphWidget;
class QTimer;

class GraphAlgorithm : public QObject
{
    Q_OBJECT

public:
    explicit GraphAlgorithm(const Graph<int>& inGraph);
    explicit GraphAlgorithm(int inStart, const Graph<int>& inGraph);
    virtual ~GraphAlgorithm();

    virtual void execute() = 0;
    virtual void showResult(GraphWidget* widget);
    void stop();

signals:
    void onShowResultFinished();

protected:
    int start;
    const Graph<int>& graph;

    QTimer* showEdgesTimer;
    int showEdgeInterval;

    // result
    EdgeList visitedEdges;
    qsizetype resultIndex;
};

class BFS : public GraphAlgorithm
{
    Q_OBJECT

public:
    explicit BFS(const Graph<int>& inGraph);

    virtual void execute() override;
};

class DFS : public GraphAlgorithm
{
    Q_OBJECT

public:
    explicit DFS(const Graph<int>& inGraph);

    virtual void execute() override;

private:
    void DFSHelper(int begin);

    QHash<int, bool> visited;
};

class BFSShortestPath : public GraphAlgorithm
{
    Q_OBJECT

public:
    explicit BFSShortestPath(const Graph<int>& inGraph);

    virtual void execute() override;
    virtual void showResult(GraphWidget* widget) override;

protected:
    int end;

    // result
    EdgeList path;
};

#endif // GRAPHAlgorithmS_H
