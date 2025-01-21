#ifndef GRAPH_H
#define GRAPH_H

#include <QJsonArray>
#include <QJsonObject>
#include <QList>

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
    using GraphContainer = QHash<ValueType, QList<GraphEdge>>;
    using NodeNeighbour = QList<GraphEdge>;

    explicit Graph(bool inIsDirected = false)
        : isDirected(inIsDirected)
    {
    }

    void addEdge(const ValueType& start, const ValueType& end, double weight);
    void addEdge(const ValueType& start, const GraphEdge& graphEdge);
    void addNode(const ValueType& value);

    void clear();

    QJsonObject toJsonObject() const;
    void fromJsonObject(const QJsonObject& jsonObj);

    const GraphContainer& getGraphContainer() const { return graphContainer; };
    const QList<NodeNeighbour>& getNeighbours(int value) const;
protected:
    GraphContainer graphContainer;
    bool isDirected;
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
const QList<typename Graph<ValueType>::NodeNeighbour>& Graph<ValueType>::getNeighbours(int value) const
{
    static const QList<GraphEdge> empty;

    if(graphContainer.contains(value))
    {
        return graphContainer.at(value);
    }
    return empty;
}

#endif // GRAPH_H
