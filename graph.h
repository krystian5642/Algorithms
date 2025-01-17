#ifndef GRAPH_H
#define GRAPH_H

#include <QPainter>
#include <QPoint>

template <class ValueType>
class GraphNode
{
public:
    friend struct std::hash<GraphNode>;

    GraphNode(const ValueType& inValue, const QPoint& inLocation)
        : value(inValue)
        , location(inLocation)
    {
    }

    bool operator==(const GraphNode& other) const;

    const ValueType& getValue() const { return value; };
    const QPoint& getLocation() const { return location; };

protected:
    ValueType value;
    QPoint location;
};

namespace std
{
template <class ValueType>
struct hash<GraphNode<ValueType>>
{
    size_t operator()(const GraphNode<ValueType> &key, size_t seed) const
    {
        return qHash(key.value, seed);
    }
};
}

template<class ValueType>
inline bool GraphNode<ValueType>::operator==(const GraphNode &other) const
{
    return value == other.value && location == other.location;
}

template <class ValueType>
class GraphEdge
{
public:
    using GraphNode = GraphNode<ValueType>;

    GraphEdge(float inWeight, const GraphNode& inEndNode)
        : weight(inWeight)
        , endNode(inEndNode)
    {
    }

    float getWeight() const { return weight; }
    const GraphNode& getEndNode() const { return endNode; };
protected:
    float weight = 0;
    GraphNode endNode;
};

template <class ValueType>
class Graph
{
public:
    using GraphNode = GraphNode<ValueType>;
    using GraphEdge = GraphEdge<ValueType>;
    using GraphContainer = QHash<GraphNode, QList<GraphEdge>>;
    using NodeNeighbour = QList<GraphEdge>;

    explicit Graph(bool inIsDirected = false)
        : isDirected(inIsDirected)
    {
    }

    void addEdge(const GraphNode& start, const GraphNode& end, float weight);
    void addNode(const GraphNode& value);

    void paint(QPainter& painter) const;

    const GraphContainer& getGraphContainer() const { return graphContainer; };
    const QList<NodeNeighbour>& getNeighbours(int value) const;
protected:
    GraphContainer graphContainer;
    bool isDirected;
};

template<class ValueType>
void Graph<ValueType>::addEdge(const GraphNode& start, const GraphNode& end, float weight)
{
    graphContainer[start].emplaceBack(weight, end);

    if(!isDirected)
    {
        graphContainer[end].emplaceBack(weight, start);
    }
    else
    {
        addNode(end);
    }
}

template<class ValueType>
void Graph<ValueType>::addNode(const GraphNode& value)
{
    if(!graphContainer.contains(value))
    {
        graphContainer[value] = QList<GraphEdge>();
    }
}

template<class ValueType>
void Graph<ValueType>::paint(QPainter &painter) const
{
    painter.save();

    for (auto it = graphContainer.constBegin(); it != graphContainer.constEnd(); ++it)
    {
        const auto& node = it.key();
        const auto& neighbours = it.value();

        painter.drawEllipse(node.getLocation(), 15, 15);

        for(const auto& neighbour : neighbours)
        {
            painter.drawLine(node.getLocation(), neighbour.getEndNode().getLocation());
        }
    }

    QPen textPen;
    textPen.setBrush(Qt::white);

    painter.setPen(textPen);

    for (auto it = graphContainer.constBegin(); it != graphContainer.constEnd(); ++it)
    {
        const auto& node = it.key();
        const auto& neighbours = it.value();

        painter.drawText(node.getLocation() + QPointF(-3.5, 3), QString("%1").arg(node.getValue()));
    }

    painter.restore();
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
