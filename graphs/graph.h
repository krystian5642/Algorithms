#ifndef GRAPH_H
#define GRAPH_H

#include "../core/data_structure.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QList>

class Graph : public DataStructure
{
    Q_OBJECT
public:
    explicit Graph(QObject *parent = nullptr, bool inIsDirected = false);
    virtual ~Graph();

    void generateRandomEdges(const double addEdgePropability, const int minWeight, const int maxWeight);
    QJsonObject toJsonObject();
    void fromJsonObject(const QJsonObject& jsonObject);

#ifdef QT_DEBUG
    void print() const override;
#endif

    bool getIsDirected() const;
    void setIsDirected(bool newIsDirected);

    virtual void addNode() = 0;
    virtual void addEdge(int start, int end, int weight = 1) = 0;
    virtual void removeEdge(int start, int end) = 0;
    virtual int getEdgeWeight(int from, int to) const = 0;
    virtual bool hasEdgeTo(int from, int to) = 0;
    virtual qsizetype getEdgesNum() const = 0;
    virtual qsizetype getNodesNum() const = 0;
    virtual void clear() = 0;
    virtual int getRandomValue(bool* found = nullptr) const = 0;
    virtual qsizetype getNeighboursNum(int node) const = 0;
    virtual int getNeighbourAt(int node, int at) const = 0;

    /** if func returns false, next edge will not be processed, forEachEdge is supposed to end execution too */
    virtual void forEachEdge(std::function<bool(int, int, int)> func) = 0;

    /** if func returns false, next neighbour will not be processed, forEachNeighbour is supposed to end execution too */
    virtual void forEachNeighbour(int node, std::function<bool(int, int, int)> func) = 0;

    void forEachEdge(std::function<bool(int, int, int)> func) const;
    void forEachNeighbour(int node, std::function<bool(int, int, int)> func) const;

    void getNodeDegrees(QList<int> &inDegrees, QList<int> &outDegrees) const;
    void getNodeInDegrees(QList<int> &inDegrees) const;
    void getNodeOutDegrees(QList<int> &outDegrees) const;

    bool isEmpty() const;

signals:
    void onNodeAdded();
    void onEdgeAdded();
    void onEdgeRemoved();

protected:
    bool isDirected;
};

class AdjacencyListGraph : public Graph
{
    Q_OBJECT
public:
    explicit AdjacencyListGraph(QObject *parent = nullptr, bool inIsDirected = false);

    void addNode() override;
    void addEdge(int from, int to, int weight = 1) override;
    void removeEdge(int from, int to) override;
    int getEdgeWeight(int from, int to) const override;
    bool hasEdgeTo(int from, int to) override;
    qsizetype getEdgesNum() const override;
    qsizetype getNodesNum() const override;
    void clear() override;
    int getRandomValue(bool* found = nullptr) const override;
    void forEachEdge(std::function<bool(int, int, int)> func) override;
    void forEachNeighbour(int node, std::function<bool(int, int, int)> func) override;
    qsizetype getNeighboursNum(int node) const override;
    int getNeighbourAt(int node, int at) const override;

private:
    struct Edge
    {
    public:
        explicit Edge(int inEndValue, int inWeight = 1)
            : endValue(inEndValue)
            , weight(inWeight)
        {
        }

        int endValue;
        int weight;
    };

    using Neighbours = QList<Edge>;
    using GraphContainer = QList<Neighbours>;

    GraphContainer adjList;
};

class AdjacencyMatrixGraph : public Graph
{
    Q_OBJECT
public:
    explicit AdjacencyMatrixGraph(QObject *parent = nullptr, bool inIsDirected = false, int nodes = 0);

    void addNode() override;
    void addEdge(int from, int to, int weight = 1) override;
    void removeEdge(int from, int to) override;
    int getEdgeWeight(int from, int to) const override;
    bool hasEdgeTo(int from, int to) override;
    qsizetype getEdgesNum() const override;
    qsizetype getNodesNum() const override;
    void clear() override;
    int getRandomValue(bool* found = nullptr) const override;
    void forEachEdge(std::function<bool(int, int, int)> func) override;
    void forEachNeighbour(int node, std::function<bool(int, int, int)> func) override;
    qsizetype getNeighboursNum(int node) const override;
    int getNeighbourAt(int node, int at) const override;

private:
    using Neighbours = QList<int>;
    using GraphContainer = QList<Neighbours>;

    GraphContainer adjMatrix;
};

class ResidualGraph : public Graph
{
    Q_OBJECT
public:
    explicit ResidualGraph(QObject *parent = nullptr);

    void addNode() override;
    void addEdge(int from, int to, int weight = 1) override;
    void removeEdge(int from, int to) override;
    int getEdgeWeight(int from, int to) const override;
    bool hasEdgeTo(int from, int to) override;
    qsizetype getEdgesNum() const override;
    qsizetype getNodesNum() const override;
    void clear() override;
    int getRandomValue(bool* found = nullptr) const override;
    void forEachEdge(std::function<bool(int, int, int)> func) override;
    void forEachNeighbour(int node, std::function<bool(int, int, int)> func) override;
    qsizetype getNeighboursNum(int node) const override;
    int getNeighbourAt(int node, int at) const override;

    struct Edge
    {
    public:
        explicit Edge(int inFrom, int inTo, int inCapacity)
            : from(inFrom)
            , to(inTo)
            , capacity(inCapacity)
            , flow(0)
            , residualEdge(nullptr)
        {
        }

        int from;
        int to;
        int capacity;
        int flow;
        Edge* residualEdge;

        void augment(int bottleNeck)
        {
            flow+=bottleNeck;
            residualEdge->flow-=bottleNeck;
        }

        int getRemainingCapacity() const
        {
            return capacity - flow;
        }
    };

private:
    using Neighbours = QList<QSharedPointer<Edge>>;
    using GraphContainer = QList<Neighbours>;

    GraphContainer adjList;

public:
    const GraphContainer& getGraphContainer() const;
};

#endif // GRAPH_H
