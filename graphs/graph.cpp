#include "graph.h"

#include "../core/utils.h"

#include <QRandomGenerator>

Graph::Graph(QObject *parent, bool inIsDirected)
    : DataStructure(parent)
    , isDirected(inIsDirected)
{

}

Graph::~Graph()
{

}

void Graph::generateRandomEdges(const double addEdgePropability)
{
    auto func1 = [&](int value1)
    {
        auto func2 = [&](int value2)
        {
            if(value1 != value2 && !hasEdgeTo(value1, value2))
            {
                const double randomDouble = QRandomGenerator::global()->generateDouble();
                if(randomDouble < addEdgePropability)
                {
                    addEdge(value1, value2);
                }
            }
        };

        forEachNode(func2);
    };

    forEachNode(func1);
}

QJsonObject Graph::toJsonObject()
{
    QJsonObject graphAsJsonObject;
    forEachNode([&](int value)
    {
        QJsonArray edgesAsJsonArray;
        forEachNeighbor(value, [&](int start, int end, int weight)
        {
            QJsonObject edgeAsJsonObject;
            edgeAsJsonObject["end"] = end;
            edgeAsJsonObject["weight"] = weight;

            edgesAsJsonArray.append(edgeAsJsonObject);
        });
        graphAsJsonObject[QString::number(value)] = edgesAsJsonArray;
    });
    return graphAsJsonObject;
}

void Graph::fromJsonObject(const QJsonObject &jsonObject)
{
    for (auto it = jsonObject.constBegin(); it != jsonObject.constEnd(); ++it)
    {
        addNode();
    }

    for (auto it = jsonObject.constBegin(); it != jsonObject.constEnd(); ++it)
    {
        const QJsonArray edgesAsJsonArray = it->toArray();
        for(const auto& edgeAsJsonValue : edgesAsJsonArray)
        {
            const QJsonObject edgeAsJsonObject = edgeAsJsonValue.toObject();
            addEdge(it.key().toInt(), edgeAsJsonObject["end"].toInt(), edgeAsJsonObject["weight"].toInt());
        }
    }
}

QString Graph::print() const
{
    QString graphAsText("Graph : \n");

    auto forEachNeighbourFunc = [&](int start, int neighbour, int weight)
    {
        graphAsText.append(" -> " + QString::number(neighbour));
    };

    auto forEachNodeFunc = [&](int value)
    {
        graphAsText.append(QString::number(value));
        forEachNeighbor(value, forEachNeighbourFunc);
        graphAsText.append('\n');
    };

    forEachNode(forEachNodeFunc);

    return graphAsText;
}

bool Graph::getIsDirected() const
{
    return isDirected;
}

void Graph::setIsDirected(bool newIsDirected)
{
    isDirected = newIsDirected;
}

void Graph::forEachNode(std::function<void (int)> func) const
{
    const_cast<Graph*>(this)->forEachNode(func);
}

void Graph::forEachEdge(std::function<void (int, int, int)> func) const
{
    const_cast<Graph*>(this)->forEachEdge(func);
}

void Graph::forEachNeighbor(int vertex, std::function<void (int, int, int)> func) const
{
    const_cast<Graph*>(this)->forEachNeighbor(vertex, func);
}

AdjacencyListGraph::AdjacencyListGraph(QObject *parent, bool inIsDirected)
    : Graph(parent, inIsDirected)
{

}

void AdjacencyListGraph::addNode()
{
    const qsizetype oldSize = adjList.size();
    adjList.resize(oldSize + 1);
    adjList[oldSize] = QList<Edge>();

    emit onNodeAdded();
}

void AdjacencyListGraph::addEdge(int start, int end, int weight)
{
    if(start < 0 || end < 0)
    {
        return;
    }

    if(start >= adjList.size())
    {
        addNode();
    }

    adjList[start].push_back(Edge{end, weight});
    emit onEdgeAdded();

    if(!isDirected)
    {
        if(end >= adjList.size())
        {
            addNode();
        }

        adjList[end].push_back(Edge{start, weight});
        emit onEdgeAdded();
    }
}

void AdjacencyListGraph::removeEdge(int start, int end)
{
    if(start < 0 || end < 0)
    {
        return;
    }

    Utils::eraseIf(adjList[start], [end](const Edge& edge) { return end == edge.endValue; });
    emit onEdgeRemoved();

    if(!isDirected)
    {
        Utils::eraseIf(adjList[end], [start](const Edge& edge) { return start == edge.endValue; });
        emit onEdgeRemoved();
    }
}

bool AdjacencyListGraph::hasEdgeTo(int from, int to)
{
    if(adjList.size() > from)
    {
        return std::any_of(adjList[from].begin(), adjList[from].end(), [to](const Edge& edge)
        {
            return edge.endValue == to;
        });
    }
    return false;
}

qsizetype AdjacencyListGraph::getEdgesNum() const
{
    qsizetype edgesNum = 0;
    for(const auto& neighbours : adjList)
    {
        edgesNum += neighbours.size();
    }

    if(!isDirected)
    {
        edgesNum /= 2;
    }

    return edgesNum;
}

qsizetype AdjacencyListGraph::getVerticesNum() const
{
    return adjList.size();
}

void AdjacencyListGraph::clear()
{
    adjList.clear();
}

int AdjacencyListGraph::getRandomValue(bool *found) const
{
    if (!adjList.isEmpty())
    {
        if (found)
        {
            *found = true;
        }

        return QRandomGenerator::global()->bounded(adjList.size());
    }
    else if (found)
    {
        *found = false;
    }
    return 0;
}

void AdjacencyListGraph::forEachNode(std::function<void (int)> func)
{
    for(int i = 0; i < adjList.size(); ++i)
    {
        func(i);
    }
}

void AdjacencyListGraph::forEachEdge(std::function<void (int, int, int)> func)
{
    for(int i = 0; i < adjList.size(); ++i)
    {
        forEachNeighbor(i, func);
    }
}

void AdjacencyListGraph::forEachNeighbor(int vertex, std::function<void (int, int, int)> func)
{
    for(const Edge& edge : adjList[vertex])
    {
        func(vertex, edge.endValue, edge.weight);
    }
}

AdjacencyMatrixGraph::AdjacencyMatrixGraph(QObject *parent, bool inIsDirected, int vertices)
    : Graph(parent, inIsDirected)
{
    adjMatrix.fill(QList<int>{}, vertices);
    for(auto& neighbours : adjMatrix)
    {
        neighbours.fill(INT_MIN, vertices);
    }
}

void AdjacencyMatrixGraph::addNode()
{
    const qsizetype verticesNum = getVerticesNum();

    adjMatrix.push_back(QList<int>(verticesNum + 1, INT_MIN));
    for(int i = 0; i < verticesNum; ++i)
    {
        adjMatrix[i].push_back(INT_MIN);
    }

    emit onNodeAdded();
}

void AdjacencyMatrixGraph::addEdge(int start, int end, int weight)
{
    if(start < 0 || end < 0)
    {
        return;
    }

    if(start >= adjMatrix.size())
    {
        addNode();
    }

    if(end >= adjMatrix[start].size())
    {
        addNode();
    }

    adjMatrix[start][end] = weight;
    emit onEdgeAdded();

    if(!isDirected)
    {
        adjMatrix[end][start] = weight;
        emit onEdgeAdded();
    }
}

void AdjacencyMatrixGraph::removeEdge(int start, int end)
{
    if(start < 0 || end < 0 || adjMatrix.size() < start || adjMatrix.size() < end)
    {
        return;
    }

    adjMatrix[start][end] = INT_MIN;
    emit onEdgeRemoved();

    if(!isDirected)
    {
        adjMatrix[end][start] = INT_MIN;
        emit onEdgeRemoved();
    }
}

bool AdjacencyMatrixGraph::hasEdgeTo(int from, int to)
{
    return adjMatrix.size() > from && adjMatrix[from].size() > to && adjMatrix[from][to] != INT_MIN;
}

qsizetype AdjacencyMatrixGraph::getEdgesNum() const
{
    qsizetype edgesNum = 0;
    for(const auto& weights : adjMatrix)
    {
        for(int weight : weights)
        {
            if(weight != INT_MIN)
            {
                edgesNum += 1;
            }
        }
    }

    if(!isDirected)
    {
        edgesNum /= 2;
    }

    return edgesNum;
}

qsizetype AdjacencyMatrixGraph::getVerticesNum() const
{
    return adjMatrix.size();
}

void AdjacencyMatrixGraph::clear()
{
    adjMatrix.clear();
}

int AdjacencyMatrixGraph::getRandomValue(bool *found) const
{
    if (!adjMatrix.isEmpty())
    {
        if (found)
        {
            *found = true;
        }

        return QRandomGenerator::global()->bounded(adjMatrix.size());
    }
    else if (found)
    {
        *found = false;
    }
    return 0;
}

void AdjacencyMatrixGraph::forEachNode(std::function<void (int)> func)
{
    for(int i = 0; i < adjMatrix.size(); ++i)
    {
        func(i);
    }
}

void AdjacencyMatrixGraph::forEachEdge(std::function<void (int, int, int)> func)
{
    for(int i = 0; i < adjMatrix.size(); ++i)
    {
        forEachNeighbor(i, func);
    }
}

void AdjacencyMatrixGraph::forEachNeighbor(int vertex, std::function<void (int, int, int)> func)
{
    for(int i = 0; i < adjMatrix[vertex].size(); ++i)
    {
        if(adjMatrix[vertex][i] != INT_MIN)
        {
            func(vertex, i, adjMatrix[vertex][i]);
        }
    }
}
