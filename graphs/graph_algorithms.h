#ifndef GRAPH_AlGORITHMS_H
#define GRAPH_AlGORITHMS_H

#include "../core/algorithm.h"

#include <QQueue>
#include <QSet>
#include <QWeakPointer>

class QComboBox;
class DataStructure;
class GraphNode;
class Graph;

class GraphAlgorithm : public Algorithm
{
    Q_OBJECT
public:
    explicit GraphAlgorithm(QObject* parent = nullptr);
    virtual ~GraphAlgorithm() = 0;

    QWidget* createPropertiesWidget(QWidget* parent = nullptr) override;
    void appendPropertiesInfo(QString& infoText) override;

    void run() override;

#ifdef QT_DEBUG
    void debugRun() override;
#endif

    DataStructureBuilder* getSelectedBuilder() const;

protected:
    const Graph* graph;

private:
    QComboBox* builderComboBox;
    QWidget* builderPropertiesWidget;
};

class BFSIterative : public GraphAlgorithm
{
    Q_OBJECT
public:
    explicit BFSIterative(QObject* parent = nullptr);

protected:
    void execute() override;
};

class BFSRecursive : public GraphAlgorithm
{
    Q_OBJECT
public:
    explicit BFSRecursive(QObject* parent = nullptr);

protected:
    void execute() override;
    void BFSRecursiveHelper(QQueue<int>& nodeQueue, std::function<bool(int, int, int)> forEachNeighbourFunc);
};

class DFSIterative : public GraphAlgorithm
{
    Q_OBJECT
public:
    explicit DFSIterative(QObject* parent = nullptr);

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
    void DFSHelper(int begin, QList<bool>& visited);
};

class TreeCenters : public GraphAlgorithm
{
    Q_OBJECT
public:
    explicit TreeCenters(QObject* parent = nullptr);

protected:
    void execute() override;
};

class TopologicalSort : public GraphAlgorithm
{
    Q_OBJECT
public:
    explicit TopologicalSort(QObject* parent = nullptr);

protected:
    void execute() override;
    void TopologicalSortHelper(int begin, QList<bool>& visited, QStack<int>& topologicalOrder);
};

#endif // GRAPH_AlGORITHMS_H
