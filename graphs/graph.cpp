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

void Graph::generateRandomEdges(const double addEdgePropability, const int minWeight, const int maxWeight)
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
                    addEdge(value1, value2, QRandomGenerator::global()->bounded(minWeight, maxWeight + 1));
                }
            }
            return true;
        };
        forEachNode(func2);

        return true;
    };

    forEachNode(func1);
}

QJsonObject Graph::toJsonObject()
{
    QJsonObject graphAsJsonObject;
    graphAsJsonObject["isDirected"] = isDirected;

    forEachNode([&](int value)
    {
        QJsonArray edgesAsJsonArray;
        forEachNeighbour(value, [&](int start, int end, int weight)
        {
            QJsonObject edgeAsJsonObject;
            edgeAsJsonObject["end"] = end;
            edgeAsJsonObject["weight"] = weight;

            edgesAsJsonArray.append(edgeAsJsonObject);

            return true;
        });
        graphAsJsonObject[QString::number(value)] = edgesAsJsonArray;

        return true;
    });
    return graphAsJsonObject;
}

void Graph::fromJsonObject(const QJsonObject &jsonObject)
{
    setIsDirected(jsonObject["isDirected"].toBool());

    for (auto it = jsonObject.constBegin(); it != jsonObject.constEnd(); ++it)
    {
        bool ok = false;
        it.key().toInt(&ok);
        if(ok)
        {
            addNode();
        }
    }

    for (auto it = jsonObject.constBegin(); it != jsonObject.constEnd(); ++it)
    {
        if(it->isArray())
        {
            const QJsonArray edgesAsJsonArray = it->toArray();
            for(const auto& edgeAsJsonValue : edgesAsJsonArray)
            {
                const QJsonObject edgeAsJsonObject = edgeAsJsonValue.toObject();
                addEdge(it.key().toInt(), edgeAsJsonObject["end"].toInt(), edgeAsJsonObject["weight"].toInt());
            }
        }
    }
}

#ifdef QT_DEBUG
void Graph::print() const
{
    QString graphAsText("Graph : \n");

    auto forEachNeighbourFunc = [&](int start, int neighbour, int weight)
    {
        graphAsText.append(" [ " + QString::number(neighbour) + " , weight : " + QString::number(weight) + " ] ");
        return true;
    };

    auto forEachNodeFunc = [&](int value)
    {
        graphAsText.append(QString::number(value) + " -> ");
        forEachNeighbour(value, forEachNeighbourFunc);
        graphAsText.append('\n');

        return true;
    };

    forEachNode(forEachNodeFunc);

    qDebug().noquote() << graphAsText;
}
#endif

bool Graph::getIsDirected() const
{
    return isDirected;
}

void Graph::setIsDirected(bool newIsDirected)
{
    isDirected = newIsDirected;
}

void Graph::forEachNode(std::function<bool (int)> func) const
{
    const_cast<Graph*>(this)->forEachNode(func);
}

void Graph::forEachEdge(std::function<bool (int, int, int)> func) const
{
    const_cast<Graph*>(this)->forEachEdge(func);
}

void Graph::forEachNeighbour(int node, std::function<bool (int, int, int)> func) const
{
    const_cast<Graph*>(this)->forEachNeighbour(node, func);
}

QList<int> Graph::getNodeDegrees() const
{
    QList<int> nodeDegrees;
    nodeDegrees.fill(0, getNodesNum());

    auto forEachNeighbourFunc = [&](int start, int neighbour, int weight)
    {
        ++nodeDegrees[neighbour];
        return true;
    };

    auto forEachNodeFunc = [&](int value)
    {
        forEachNeighbour(value, forEachNeighbourFunc);
        return true;
    };

    forEachNode(forEachNodeFunc);

    return nodeDegrees;
}

qsizetype Graph::getNeighboursNum(int node) const
{
    qsizetype neighboursNum = 0;
    auto countNeighbours = [&](int start, int neighbour, int weight)
    {
        neighboursNum++;
        return true;
    };

    forEachNeighbour(node, countNeighbours);

    return neighboursNum;
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
    if(start < 0 || end < 0 || hasEdgeTo(start, end))
    {
        return;
    }

    if(start >= adjList.size())
    {
        addNode();
    }

    adjList[start].push_back(Edge{end, weight});
    emit onEdgeAdded();

    if(!isDirected && !hasEdgeTo(end, start))
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

qsizetype AdjacencyListGraph::getNodesNum() const
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

void AdjacencyListGraph::forEachNode(std::function<bool (int)> func)
{
    for(int i = 0; i < adjList.size(); ++i)
    {
        if(!func(i))
        {
            return;
        }
    }
}

void AdjacencyListGraph::forEachEdge(std::function<bool (int, int, int)> func)
{
    for(int i = 0; i < adjList.size(); ++i)
    {
        for(const Edge& edge : adjList[i])
        {
            if(!func(i, edge.endValue, edge.weight))
            {
                return;
            }
        }
    }
}

void AdjacencyListGraph::forEachNeighbour(int node, std::function<bool (int, int, int)> func)
{
    for(const Edge& edge : adjList[node])
    {
        if(!func(node, edge.endValue, edge.weight))
        {
            return;
        }
    }
}

qsizetype AdjacencyListGraph::getNeighboursNum(int node) const
{
    if(adjList.size() <= node)
    {
        return 0;
    }

    return adjList[node].size();
}

AdjacencyMatrixGraph::AdjacencyMatrixGraph(QObject *parent, bool inIsDirected, int nodes)
    : Graph(parent, inIsDirected)
{
    adjMatrix.fill(QList<int>{}, nodes);
    for(auto& neighbours : adjMatrix)
    {
        neighbours.fill(INT_MIN, nodes);
    }
}

void AdjacencyMatrixGraph::addNode()
{
    const qsizetype nodesNum = getNodesNum();

    adjMatrix.push_back(QList<int>(nodesNum + 1, INT_MIN));
    for(int i = 0; i < nodesNum; ++i)
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

qsizetype AdjacencyMatrixGraph::getNodesNum() const
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

void AdjacencyMatrixGraph::forEachNode(std::function<bool (int)> func)
{
    for(int i = 0; i < adjMatrix.size(); ++i)
    {
        if(!func(i))
        {
            return;
        }
    }
}

void AdjacencyMatrixGraph::forEachEdge(std::function<bool (int, int, int)> func)
{
    for(int i = 0; i < adjMatrix.size(); ++i)
    {
        for(int j = 0; i < adjMatrix[i].size(); ++i)
        {
            if(adjMatrix[i][j] != INT_MIN)
            {
                if(!func(i, j, adjMatrix[i][j]))
                {
                    return;
                }
            }
        }
    }
}

void AdjacencyMatrixGraph::forEachNeighbour(int node, std::function<bool (int, int, int)> func)
{
    for(int i = 0; i < adjMatrix[node].size(); ++i)
    {
        if(adjMatrix[node][i] != INT_MIN)
        {
            if(!func(node, i, adjMatrix[node][i]))
            {
                return;
            }
        }
    }
}

qsizetype AdjacencyMatrixGraph::getNeighboursNum(int node) const
{
    qsizetype neighboursNum = 0;
    auto countNeighbours = [&](int start, int neighbour, int weight)
    {
        neighboursNum++;
        return true;
    };

    Graph::forEachNeighbour(node, countNeighbours);

    return neighboursNum;
}
