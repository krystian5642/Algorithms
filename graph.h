#ifndef GRAPH_H
#define GRAPH_H

#include <QList>


template <class ValueType>
class GraphEdge
{
public:
    explicit GraphEdge(float inWeight, const ValueType& inEndValue)
        : weight(inWeight)
        , endValue(inEndValue)
    {
    }

    bool operator==(const GraphEdge& other) const;

    float getWeight() const { return weight; }
    const ValueType& getEndValue() const { return endValue; };
protected:
    float weight = 0;
    ValueType endValue;
};

template<class ValueType>
inline bool GraphEdge<ValueType>::operator==(const GraphEdge &other) const
{
    return weight == other.weight && endValue == other.endValue;
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

    void addEdge(const ValueType& start, const ValueType& end, float weight);
    void addNode(const ValueType& value);

    const GraphContainer& getGraphContainer() const { return graphContainer; };
    const QList<NodeNeighbour>& getNeighbours(int value) const;
protected:
    GraphContainer graphContainer;
    bool isDirected;
};

template<class ValueType>
void Graph<ValueType>::addEdge(const ValueType& start, const ValueType& end, float weight)
{
    const GraphEdge endEdge(weight, end);
    if(!graphContainer[start].contains(endEdge))
    {
        graphContainer[start].emplaceBack(endEdge);
    }

    if(!isDirected)
    {
        const GraphEdge startEdge(weight, start);
        if(!graphContainer[end].contains(startEdge))
        {
            graphContainer[end].emplaceBack(startEdge);
        }
    }
    else
    {
        addNode(end);
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
