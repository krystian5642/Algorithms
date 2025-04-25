#include "graph.h"

#include <QRandomGenerator>

GraphEdge::GraphEdge(int inWeight, int inEndValue)
    : weight(inWeight)
    , endValue(inEndValue)
{

}

bool GraphEdge::operator==(const GraphEdge& other) const
{
    return weight == other.weight && endValue == other.endValue;
}

QJsonValue GraphEdge::toJsonValue() const
{
    QJsonObject jsonObject;
    jsonObject["weight"] = weight;
    jsonObject["endValue"] = endValue;
    return jsonObject;
}

void GraphEdge::fromJsonValue(const QJsonValue &jsonValue)
{
    weight = jsonValue["weight"].toInt();
    endValue = jsonValue["endValue"].toInt();
}

int GraphEdge::getWeight() const
{
    return weight;
}

int GraphEdge::getEndValue() const
{
    return endValue;
}

Graph::Graph(bool inIsDirected)
    : isDirected(inIsDirected)
{

}

void Graph::addEdge(int start, int end, int weight)
{
    const GraphEdge endEdge(weight, end);
    addEdge(start, endEdge);
}

void Graph::addEdge(int start, const GraphEdge &graphEdge)
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

bool Graph::addNode(int value)
{
    if(!graphContainer.contains(value))
    {
        graphContainer[value] = QList<GraphEdge>();
        return true;
    }
    return false;
}

qsizetype Graph::getEdgesNum() const
{
    return countEdges();
}

qsizetype Graph::getNodesNum() const
{
    return graphContainer.size();
}

void Graph::clear()
{
    graphContainer.clear();
}

int Graph::getRandomValue(bool* found) const
{
    if (!graphContainer.isEmpty())
    {
        if (found)
        {
            *found = true;
        }

        const auto keys = graphContainer.keys();
        const qint64 randomIndex = QRandomGenerator::global()->bounded(keys.size());
        return keys[randomIndex];
    }

    if (found)
    {
        *found = false;
    }
    return 0;
}

QJsonObject Graph::toJsonObject() const
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

void Graph::fromJsonObject(const QJsonObject& jsonObj)
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
const Graph::Neighbours& Graph::getNeighbourEdges(int value) const
{
    static const Neighbours empty;

    auto it = graphContainer.constFind(value);
    if(it != graphContainer.constEnd())
    {
        return it.value();
    }
    return empty;
}

qsizetype Graph::countEdges() const
{
    qsizetype edgesNum = 0;
    for (auto it = graphContainer.constBegin(); it != graphContainer.constEnd(); ++it)
    {
        edgesNum += it.value().size();
    }

    if(!isDirected)
    {
        edgesNum /= 2;
    }

    return edgesNum;
}
