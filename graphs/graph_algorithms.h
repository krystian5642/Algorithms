#ifndef GRAPH_AlGORITHMS_H
#define GRAPH_AlGORITHMS_H

#include "../core/algorithm.h"

#include <QQueue>
#include <QSet>

class DataStructure;
class GraphNode;
class Graph;

class GraphAlgorithm : public Algorithm
{
    Q_OBJECT

    Q_PROPERTY(double addEdgePropability READ getAddEdgePropability WRITE setAddEdgePropability NOTIFY addEdgePropabilityChanged FINAL)
public:
    explicit GraphAlgorithm(QObject* parent = nullptr);
    virtual ~GraphAlgorithm() = 0;

    QWidget* createPropertiesWidget(QWidget* parent = nullptr, bool addStretch = true) override;

    void run() override;

    Graph *getGraph() const;
    void setGraph(Graph *newGraph);

    double getAddEdgePropability() const;
    void setAddEdgePropability(double newAddEdgePropability);

    DataStructure* createSelectedDataStructure() const;

signals:
    void addEdgePropabilityChanged();

protected:
    Graph* graph;

    double addEdgePropability;

    QString selectedImplementation;
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

    QQueue<int> nodeQueue;
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
