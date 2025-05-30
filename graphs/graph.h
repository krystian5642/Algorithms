#ifndef GRAPH_H
#define GRAPH_H

#include "../core/data_structure.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QList>

class Graph : public DataStructure
{
public:
    explicit Graph(bool inIsDirected = false);
    virtual ~Graph();

    void generateRandomEdges(const double addEdgePropability);
    QJsonObject toJsonObject();
    void fromJsonObject(const QJsonObject& jsonObject);

    bool getIsDirected() const;
    void setIsDirected(bool newIsDirected);

    virtual void addEdge(int start, int end, int weight = 1) = 0;
    virtual void removeEdge(int start, int end) = 0;
    virtual void addNode() = 0;
    virtual bool hasEdgeTo(int from, int to) = 0;
    virtual qsizetype getEdgesNum() const = 0;
    virtual qsizetype getVerticesNum() const = 0;
    virtual void clear() = 0;
    virtual int getRandomValue(bool* found = nullptr) const = 0;
    virtual void forEachNode(std::function<void(int)> func) = 0;
    virtual void forEachEdge(std::function<void(int, int, int)> func) = 0;
    virtual void forEachNeighbor(int vertex, std::function<void(int, int, int)> func) = 0;

protected:
    bool isDirected;
};

class AdjacencyListGraph : public Graph
{
public:
    explicit AdjacencyListGraph(bool inIsDirected = false);

    void addEdge(int start, int end, int weight = 1) override;
    void removeEdge(int start, int end) override;
    void addNode() override;
    bool hasEdgeTo(int from, int to) override;
    qsizetype getEdgesNum() const override;
    qsizetype getVerticesNum() const override;
    void clear() override;
    int getRandomValue(bool* found = nullptr) const override;
    void forEachNode(std::function<void(int)> func) override;
    void forEachEdge(std::function<void(int, int, int)> func) override;
    void forEachNeighbor(int vertex, std::function<void(int, int, int)> func) override;

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
public:
    explicit AdjacencyMatrixGraph(int vertices = 0,  bool inIsDirected = false);

    void addEdge(int start, int end, int weight = 1) override;
    void removeEdge(int start, int end) override;
    void addNode() override;
    bool hasEdgeTo(int from, int to) override;
    qsizetype getEdgesNum() const override;
    qsizetype getVerticesNum() const override;
    void clear() override;
    int getRandomValue(bool* found = nullptr) const override;
    void forEachNode(std::function<void(int)> func) override;
    void forEachEdge(std::function<void(int, int, int)> func) override;
    void forEachNeighbor(int vertex, std::function<void(int, int, int)> func) override;

private:
    using Neighbours = QList<int>;
    using GraphContainer = QList<Neighbours>;

    GraphContainer adjMatrix;
};

#endif // GRAPH_H
