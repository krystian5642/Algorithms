#ifndef GRAPH_AlGORITHMS_H
#define GRAPH_AlGORITHMS_H

#include "../core/algorithm.h"

#include <QQueue>
#include <QSet>

class GraphNode;
class Graph;

class GraphAlgorithm : public Algorithm
{
    Q_OBJECT
public:
    explicit GraphAlgorithm(QObject* parent = nullptr);
    virtual ~GraphAlgorithm() = 0;

    void run() override;
    void clear() override;

    const Graph *getGraph() const;
    void setGraph(const Graph *newGraph);

protected:
    const Graph* graph = nullptr;
};

class BFSIterative : public GraphAlgorithm
{
    Q_OBJECT
public:
    explicit BFSIterative(QObject* parent = nullptr);

    void init() override;
    void clear() override;

protected:
    void execute() override;

    QQueue<const GraphNode*> nodeQueue;
    QList<bool> visited;
};

class DFSRecursive : public GraphAlgorithm
{
    Q_OBJECT
public:
    explicit DFSRecursive(QObject* parent = nullptr);

protected:
    void execute() override;
    void DFSHelper(int begin, QSet<int>& visited);
};

#endif // GRAPH_AlGORITHMS_H
