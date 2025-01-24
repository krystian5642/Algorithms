#ifndef GRAPH_H
#define GRAPH_H

#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QRandomGenerator>

template <class ValueType>
class Graph;

template <class ValueType>
class GraphEdge
{
public:
    friend class Graph<ValueType>;

    GraphEdge() = default;
    explicit GraphEdge(float inWeight, const ValueType& inEndValue)
        : weight(inWeight)
        , endValue(inEndValue)
    {
    }

    bool operator==(const GraphEdge& other) const;

    QJsonValue toJsonValue() const;
    void fromJsonValue(const QJsonValue& jsonValue);

    double getWeight() const { return weight; }
    const ValueType& getEndValue() const { return endValue; };

protected:
    double weight = 0;
    ValueType endValue;
};

template<class ValueType>
inline bool GraphEdge<ValueType>::operator==(const GraphEdge &other) const
{
    return weight == other.weight && endValue == other.endValue;
}

template<class ValueType>
QJsonValue GraphEdge<ValueType>::toJsonValue() const
{
    QJsonObject jsonObject;
    jsonObject["weight"] = weight;
    jsonObject["endValue"] = endValue;
    return jsonObject;
}

template<class ValueType>
void GraphEdge<ValueType>::fromJsonValue(const QJsonValue &jsonValue)
{
    weight = jsonValue["weight"].toDouble();
    endValue = jsonValue["endValue"].toInt();
}

template <class ValueType>
class Graph
{
public:
    using GraphEdge = GraphEdge<ValueType>;
    using Neighbours = QList<GraphEdge>;
    using GraphContainer = QHash<ValueType, Neighbours>;

    explicit Graph(bool inIsDirected = false)
        : isDirected(inIsDirected)
    {
    }

    void addEdge(const ValueType& start, const ValueType& end, double weight);
    void addEdge(const ValueType& start, const GraphEdge& graphEdge);
    void addNode(const ValueType& value);

    inline qsizetype getEdgesNum() const { return 2 * (qMax(0, getNodesNum()) - 1); };
    inline qsizetype getNodesNum() const { return graphContainer.size(); };

    void clear();
    ValueType getRandomValue(bool* found = nullptr) const;

    QJsonObject toJsonObject() const;
    void fromJsonObject(const QJsonObject& jsonObj);

    const Neighbours& getNeighbourEdges(const ValueType& value) const;
    QList<ValueType> getNeighbourValues(const ValueType& value) const;

    inline bool getIsDirected() const { return isDirected; }

protected:
    GraphContainer graphContainer;
    bool isDirected;

public:
    // to support range-based operation for loop
    inline GraphContainer::iterator        begin() { return graphContainer.begin(); }
    inline GraphContainer::const_iterator  constBegin() const { return graphContainer.constBegin(); }
    inline GraphContainer::iterator        end() { return graphContainer.end(); }
    inline GraphContainer::const_iterator  constEnd() const { return graphContainer.constEnd(); }
};

template<class ValueType>
void Graph<ValueType>::addEdge(const ValueType& start, const ValueType& end, double weight)
{
    const GraphEdge endEdge(weight, end);
    addEdge(start, endEdge);
}

template<class ValueType>
inline void Graph<ValueType>::addEdge(const ValueType &start, const GraphEdge &graphEdge)
{
    if(!graphContainer[start].contains(graphEdge))
    {
        graphContainer[start].emplaceBack(graphEdge);
    }

    if(!isDirected)
    {
        const GraphEdge startEdge(graphEdge.weight, start);
        if(!graphContainer[graphEdge.endValue].contains(startEdge))
        {
            graphContainer[graphEdge.endValue].emplaceBack(startEdge);
        }
    }
    else
    {
        addNode(graphEdge.endValue);
    }
}

template<class ValueType>
void Graph<ValueType>::addNode(const ValueType& value)
{
    if(!graphContainer.contains(value))
    {
        graphContainer[value] = QList<GraphEdge>();
    }
}

template<class ValueType>
inline void Graph<ValueType>::clear()
{
    graphContainer.clear();
}

template<class ValueType>
ValueType Graph<ValueType>::getRandomValue(bool* found) const
{
    if (!graphContainer.isEmpty())
    {
        if (found)
        {
            *found = true;
        }

        const auto keys = graphContainer.keys();
        const int randomIndex = QRandomGenerator::global()->bounded(keys.size());
        return keys[randomIndex];
    }

    if (found)
    {
        *found = false;
    }
    return {};
}

template<class ValueType>
QJsonObject Graph<ValueType>::toJsonObject() const
{
    QJsonObject jsonObj;
    for (auto it = graphContainer.constBegin(); it != graphContainer.constEnd(); ++it)
    {
        const auto& key = it.key();
        const auto& values = it.value();

        QJsonArray jsonArray;
        for (const auto& value : values)
        {
            jsonArray.append(value.toJsonValue());
        }

        jsonObj[QString::number(key)] = jsonArray;
    }
    return jsonObj;
}

template<class ValueType>
void Graph<ValueType>::fromJsonObject(const QJsonObject& jsonObj)
{
    clear();
    for (auto it = jsonObj.constBegin(); it != jsonObj.constEnd(); ++it)
    {
        const QJsonArray jsonArray = it.value().toArray();
        for(const auto& value : jsonArray)
        {
            GraphEdge edge;
            edge.fromJsonValue(value.toObject());

            addEdge(it.key().toInt(), edge);
        }
    }
}

template<class ValueType>
const Graph<ValueType>::Neighbours &Graph<ValueType>::getNeighbourEdges(const ValueType &value) const
{
    static const Neighbours empty;

    auto it = graphContainer.constFind(value);
    if(it != graphContainer.constEnd())
    {
        return it.value();
    }
    return empty;
}

template<class ValueType>
QList<ValueType> Graph<ValueType>::getNeighbourValues(const ValueType &value) const
{
    QList<ValueType> neighbourValues;

    auto it = graphContainer.constFind(value);
    if(it != graphContainer.constEnd())
    {
        for(const auto& edge : it.value())
        {
            neighbourValues.append(edge.getEndValue());
        }
        return neighbourValues;
    }
    return neighbourValues;
}

#endif // GRAPH_H
