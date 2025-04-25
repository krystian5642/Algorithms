#ifndef GRAPH_AlGORITHMS_H
#define GRAPH_AlGORITHMS_H

#include "../core/algorithm.h"

#include <QSet>

class Graph;

class GraphAlgorithm : public Algorithm
{
    Q_OBJECT
public:
    explicit GraphAlgorithm(QObject* parent = nullptr);
    virtual ~GraphAlgorithm() = 0;

    void run() override;

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

protected:
    void execute() override;
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
