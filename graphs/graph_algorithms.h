#ifndef GRAPH_AlGORITHMS_H
#define GRAPH_AlGORITHMS_H

#include "../core/algorithm.h"
#include "edge_list.h"

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
    qreal calculateXForCurrentIteration() const override;

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

    bool canRunAlgorithm(QString& outInfo) const override;

    static void getResult(const Graph* graph, QStack<int>& topologicalOrder);

protected:
    void execute() override;
    void TopologicalSortHelper(int begin, QList<bool>& visited, QStack<int>& topologicalOrder);
};

class KahnsAlgorithm : public GraphAlgorithm
{
    Q_OBJECT
public:
    explicit KahnsAlgorithm(QObject* parent = nullptr);

    bool canRunAlgorithm(QString& outInfo) const override;

protected:
    void execute() override;
};

class LazyDijkstraAlgorithm : public GraphAlgorithm
{
    Q_OBJECT
public:
    explicit LazyDijkstraAlgorithm(QObject* parent = nullptr);

    bool canRunAlgorithm(QString& outInfo) const override;

protected:
    void execute() override;
};

class EagerDijkstraAlgorithm : public GraphAlgorithm
{
    Q_OBJECT
public:
    explicit EagerDijkstraAlgorithm(QObject* parent = nullptr);

    bool canRunAlgorithm(QString& outInfo) const override;

protected:
    void execute() override;
};

class BellmanFordAlgorithm : public GraphAlgorithm
{
    Q_OBJECT

    Q_PROPERTY(bool breakIfNoChange READ getBreakIfNoChange WRITE setBreakIfNoChange NOTIFY breakIfNoChangeChanged FINAL)
public:
    explicit BellmanFordAlgorithm(QObject* parent = nullptr);

    bool getBreakIfNoChange() const;
    void setBreakIfNoChange(bool newBreakIfNoChange);

signals:
    void breakIfNoChangeChanged();

protected:
    void execute() override;

    bool breakIfNoChange;
};

class FloydWarshallAlgorithm : public GraphAlgorithm
{
    Q_OBJECT

public:
    explicit FloydWarshallAlgorithm(QObject* parent = nullptr);

protected:
    void execute() override;
};

// Tarjan's Strongly Connected Component
class SCCsAlgorithm : public GraphAlgorithm
{
    Q_OBJECT
public:
    explicit SCCsAlgorithm(QObject* parent = nullptr);

protected:
    void execute() override;
    void SCCsHelper(int begin, QList<int> &visitTime, QStack<int> &stack, QList<int> &low);

    int time;
    int sccCount;
};

class TravelingSalesmanProblemAlgorithmHash : public GraphAlgorithm
{
    Q_OBJECT
public:
    explicit TravelingSalesmanProblemAlgorithmHash(QObject* parent = nullptr);

    struct Index
    {
    public:
        int vertex;
        QSet<int> vertices;

    public:
        bool operator==(const Index &other) const
        {
            return vertex == other.vertex && vertices == other.vertices;
        }
    };

protected:
    void execute() override;
    QList<int> buildResultPath(const Index& index, const QHash<Index, int> &parents);

    int getDistanceTo(int end, const Index& index, const QHash<Index, int>& minDistances) const;
    void generateCombinations(QList<QSet<int>>& combinations) const;
};

static uint qHash(const TravelingSalesmanProblemAlgorithmHash::Index &key, uint seed = 0)
{
    uint h = qHash(key.vertex, seed);

    for (int v : key.vertices)
    {
        h ^= qHash(v, seed) + 0x9e3779b9 + (h << 6) + (h >> 2);
    }

    return h;
}

// This version using bitmasking is much faster and more memory-efficient than TravelingSalesmanProblemAlgorithmHash
class TravelingSalesmanProblemAlgorithmBitmask : public GraphAlgorithm
{
    Q_OBJECT
public:
    explicit TravelingSalesmanProblemAlgorithmBitmask(QObject* parent = nullptr);

protected:
    void execute() override;
    bool isNotInCombination(int i, int combination) const;
    QList<int> buildResultPath(const QList<QList<int>> &memo);

    void generateCombinations(int r, QList<int> &combinations) const;
    void generateCombinations(int subSet, int pos, int r, QList<int> &combinations) const;
};

class EulerianPathAlgorithm : public GraphAlgorithm
{
    Q_OBJECT
public:
    explicit EulerianPathAlgorithm(QObject* parent = nullptr);

protected:
    void execute() override;

    void DFSHelper(int begin, QList<int>& outDegrees, QList<int>& eulerianPath, EdgeList &visitedUndirectedEdges) const;
    bool doesEulerianPathExist(const QList<int> &inDegrees, const QList<int> &outDegrees) const;
    int findStart(const QList<int> &inDegrees, const QList<int> &outDegrees) const;
};

class LazyPrimMinimumSpanningTreeAlgorithm : public GraphAlgorithm
{
    Q_OBJECT
public:
    explicit LazyPrimMinimumSpanningTreeAlgorithm(QObject* parent = nullptr);

    bool canRunAlgorithm(QString& outInfo) const override;

protected:
    void execute() override;

};

class EagerPrimMinimumSpanningTreeAlgorithm : public GraphAlgorithm
{
    Q_OBJECT
public:
    explicit EagerPrimMinimumSpanningTreeAlgorithm(QObject* parent = nullptr);

    bool canRunAlgorithm(QString& outInfo) const override;

protected:
    void execute() override;

};


#endif // GRAPH_AlGORITHMS_H
