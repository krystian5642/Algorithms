#ifndef GRAPH_AlGORITHMS_H
#define GRAPH_AlGORITHMS_H

#include "../Core/algorithm.h"

#include <QSet>

class Graph;

class GraphAlgorithm : public Algorithm
{
public:
    virtual ~GraphAlgorithm() = 0;

    const Graph *getGraph() const;
    void setGraph(const Graph *newGraph);

protected:
    const Graph* graph = nullptr;
};

class BFS : public GraphAlgorithm
{
public:
    void run() override;
};

class DFS : public GraphAlgorithm
{
public:
    void run() override;

private:
    void DFSHelper(int begin, QSet<int>& visited);
};

#endif // GRAPH_AlGORITHMS_H
