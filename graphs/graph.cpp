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
    const qsizetype nodesNum = getNodesNum();

    for(int i = 0; i < nodesNum; ++i)
    {
        for(int j = 0; j < nodesNum; ++j)
        {
            if(i != j && !hasEdgeTo(i, j))
            {
                const double randomDouble = QRandomGenerator::global()->generateDouble();
                if(randomDouble < addEdgePropability)
                {
                    addEdge(i, j, QRandomGenerator::global()->bounded(minWeight, maxWeight + 1));
                }
            }
        };
    };
}

QJsonObject Graph::toJsonObject()
{
    QJsonObject graphAsJsonObject;
    graphAsJsonObject["isDirected"] = isDirected;

    const qsizetype nodesNum = getNodesNum();

    for(int i = 0; i < nodesNum; ++i)
    {
        QJsonArray edgesAsJsonArray;
        forEachNeighbour(i, [&](int start, int end, int weight)
        {
            QJsonObject edgeAsJsonObject;
            edgeAsJsonObject["end"] = end;
            edgeAsJsonObject["weight"] = weight;

            edgesAsJsonArray.append(edgeAsJsonObject);

            return true;
        });
        graphAsJsonObject[QString::number(i)] = edgesAsJsonArray;
    }
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

    const qsizetype nodesNum = getNodesNum();

    for(int i = 0; i < nodesNum; ++i)
    {
        graphAsText.append(QString::number(i) + " -> ");
        forEachNeighbour(i, forEachNeighbourFunc);
        graphAsText.append('\n');
    };

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

void Graph::forEachEdge(std::function<bool (int, int, int)> func) const
{
    const_cast<Graph*>(this)->forEachEdge(func);
}

void Graph::forEachNeighbour(int node, std::function<bool (int, int, int)> func) const
{
    const_cast<Graph*>(this)->forEachNeighbour(node, func);
}

void Graph::getNodeDegrees(QList<int> &inDegrees, QList<int> &outDegrees) const
{
    const qsizetype nodesNum = getNodesNum();

    inDegrees.fill(0, nodesNum);
    outDegrees.fill(0, nodesNum);

    auto forEachEdge = [&](int from, int to, int weight)
    {
        outDegrees[from]++;
        inDegrees[to]++;
        return true;
    };

    Graph::forEachEdge(forEachEdge);
}

void Graph::getNodeInDegrees(QList<int> &inDegrees) const
{
    const qsizetype nodesNum = getNodesNum();

    inDegrees.fill(0, nodesNum);

    auto forEachEdge = [&](int from, int to, int weight)
    {
        inDegrees[to]++;
        return true;
    };

    Graph::forEachEdge(forEachEdge);
}

void Graph::getNodeOutDegrees(QList<int> &outDegrees) const
{
    const qsizetype nodesNum = getNodesNum();

    outDegrees.fill(0, nodesNum);

    auto forEachEdge = [&](int from, int to, int weight)
    {
        outDegrees[from]++;
        return true;
    };

    Graph::forEachEdge(forEachEdge);
}

bool Graph::isEmpty() const
{
    return getNodesNum() == 0;
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

int AdjacencyListGraph::getEdgeWeight(int from, int to) const
{
    if(from < 0 || to < 0 || from > adjList.size() || to > adjList.size())
    {
        return INF;
    }

    const Neighbours& neighbours = adjList[from];
    for(const Edge& edge : neighbours)
    {
        if(edge.endValue == to)
        {
            return edge.weight;
        }
    }

    return INF;
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

int AdjacencyListGraph::getNeighbourAt(int node, int at) const
{
    if(adjList.size() <= node && adjList[node].size() <= at)
    {
        return -1;
    }

    return adjList[node][at].endValue;
}

AdjacencyMatrixGraph::AdjacencyMatrixGraph(QObject *parent, bool inIsDirected, int nodes)
    : Graph(parent, inIsDirected)
{
    adjMatrix.fill(QList<int>{}, nodes);
    for(auto& neighbours : adjMatrix)
    {
        neighbours.fill(INF, nodes);
    }
}

void AdjacencyMatrixGraph::addNode()
{
    const qsizetype nodesNum = getNodesNum();

    adjMatrix.push_back(QList<int>(nodesNum + 1, INF));
    for(int i = 0; i < nodesNum; ++i)
    {
        adjMatrix[i].push_back(INF);
    }

    emit onNodeAdded();
}

void AdjacencyMatrixGraph::addEdge(int from, int to, int weight)
{
    if(from < 0 || to < 0)
    {
        return;
    }

    if(from >= adjMatrix.size())
    {
        addNode();
    }

    if(to >= adjMatrix[from].size())
    {
        addNode();
    }

    adjMatrix[from][to] = weight;
    emit onEdgeAdded();

    if(!isDirected)
    {
        adjMatrix[to][from] = weight;
        emit onEdgeAdded();
    }
}

void AdjacencyMatrixGraph::removeEdge(int from, int to)
{
    if(from < 0 || to < 0 || adjMatrix.size() < from || adjMatrix.size() < to)
    {
        return;
    }

    adjMatrix[from][to] = INF;
    emit onEdgeRemoved();

    if(!isDirected)
    {
        adjMatrix[to][from] = INF;
        emit onEdgeRemoved();
    }
}

int AdjacencyMatrixGraph::getEdgeWeight(int from, int to) const
{
    return adjMatrix.size() > from && adjMatrix[from].size() > to ? adjMatrix[from][to] : INF;
}

bool AdjacencyMatrixGraph::hasEdgeTo(int from, int to)
{
    return adjMatrix.size() > from && adjMatrix[from].size() > to && adjMatrix[from][to] != INF;
}

qsizetype AdjacencyMatrixGraph::getEdgesNum() const
{
    qsizetype edgesNum = 0;
    for(const auto& weights : adjMatrix)
    {
        for(int weight : weights)
        {
            if(weight != INF)
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

void AdjacencyMatrixGraph::forEachEdge(std::function<bool (int, int, int)> func)
{
    for(int i = 0; i < adjMatrix.size(); ++i)
    {
        for(int j = 0; i < adjMatrix[i].size(); ++i)
        {
            if(adjMatrix[i][j] != INF)
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
        if(adjMatrix[node][i] != INF)
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

int AdjacencyMatrixGraph::getNeighbourAt(int node, int at) const
{
    if(adjMatrix.size() <= node && adjMatrix[node].size() <= at)
    {
        return -1;
    }
    return adjMatrix[node][at];
}
