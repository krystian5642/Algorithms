#ifndef GRAPH_H
#define GRAPH_H

#include <QJsonArray>
#include <QJsonObject>
#include <QList>

class GraphEdge
{
public:
    friend class Graph;

    explicit GraphEdge(int inWeight = 0, int inEndValue = 0);

    bool operator==(const GraphEdge& other) const;

    // serialization
    QJsonValue toJsonValue() const;
    void fromJsonValue(const QJsonValue& jsonValue);

    int getWeight() const;
    int getEndValue() const;

private:
    int weight;
    int endValue;
};

class Graph
{
public:
    using Neighbours = QList<GraphEdge>;
    using GraphContainer = QHash<int, Neighbours>;

    explicit Graph(bool inIsDirected = false);

    void addEdge(int start, int end, int weight = 0);
    void addEdge(int start, const GraphEdge& graphEdge);
    bool addNode(int value);

    qsizetype getEdgesNum() const;
    qsizetype getNodesNum() const;

    void clear();
    int getRandomValue(bool* found = nullptr) const;

    // serialization
    QJsonObject toJsonObject() const;
    void fromJsonObject(const QJsonObject& jsonObj);

    const Neighbours& getNeighbourEdges(int value) const;
    QList<int> getNeighbourValues(int value) const;

    bool getIsDirected() const;

protected:
    GraphContainer graphContainer;
    const bool isDirected;

public:
    // to support range-based operation for loop
    inline GraphContainer::iterator        begin() { return graphContainer.begin(); }
    inline GraphContainer::const_iterator  constBegin() const { return graphContainer.constBegin(); }
    inline GraphContainer::iterator        end() { return graphContainer.end(); }
    inline GraphContainer::const_iterator  constEnd() const { return graphContainer.constEnd(); }

private:
    qsizetype countEdges() const;
};

#endif // GRAPH_H
