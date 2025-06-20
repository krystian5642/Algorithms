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

    void generateRandomEdges(const double addEdgePropability);
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
    virtual bool hasEdgeTo(int from, int to) = 0;
    virtual qsizetype getEdgesNum() const = 0;
    virtual qsizetype getVerticesNum() const = 0;
    virtual void clear() = 0;
    virtual int getRandomValue(bool* found = nullptr) const = 0;
    virtual qsizetype getNeighboursNum(int vertex) const = 0;

    /** if func returns false, next node will not be processed, forEachNode is supposed to end execution too */
    virtual void forEachNode(std::function<bool(int)> func) = 0;

    /** if func returns false, next edge will not be processed, forEachEdge is supposed to end execution too */
    virtual void forEachEdge(std::function<bool(int, int, int)> func) = 0;

    /** if func returns false, next neighbour will not be processed, forEachNeighbor is supposed to end execution too */
    virtual void forEachNeighbor(int vertex, std::function<bool(int, int, int)> func) = 0;

    void forEachNode(std::function<bool(int)> func) const;
    void forEachEdge(std::function<bool(int, int, int)> func) const;
    void forEachNeighbor(int vertex, std::function<bool(int, int, int)> func) const;

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
    void addEdge(int start, int end, int weight = 1) override;
    void removeEdge(int start, int end) override;
    bool hasEdgeTo(int from, int to) override;
    qsizetype getEdgesNum() const override;
    qsizetype getVerticesNum() const override;
    void clear() override;
    int getRandomValue(bool* found = nullptr) const override;
    void forEachNode(std::function<bool(int)> func) override;
    void forEachEdge(std::function<bool(int, int, int)> func) override;
    void forEachNeighbor(int vertex, std::function<bool(int, int, int)> func) override;
    qsizetype getNeighboursNum(int vertex) const override;

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
    explicit AdjacencyMatrixGraph(QObject *parent = nullptr, bool inIsDirected = false, int vertices = 0);

    void addNode() override;
    void addEdge(int start, int end, int weight = 1) override;
    void removeEdge(int start, int end) override;
    bool hasEdgeTo(int from, int to) override;
    qsizetype getEdgesNum() const override;
    qsizetype getVerticesNum() const override;
    void clear() override;
    int getRandomValue(bool* found = nullptr) const override;
    void forEachNode(std::function<bool(int)> func) override;
    void forEachEdge(std::function<bool(int, int, int)> func) override;
    void forEachNeighbor(int vertex, std::function<bool(int, int, int)> func) override;
    qsizetype getNeighboursNum(int vertex) const override;

private:
    using Neighbours = QList<int>;
    using GraphContainer = QList<Neighbours>;

    GraphContainer adjMatrix;
};

#endif // GRAPH_H
