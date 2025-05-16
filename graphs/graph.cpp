#include "graph.h"

#include <QRandomGenerator>

Graph::Graph(bool inIsDirected)
    : isDirected(inIsDirected)
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

AdjacencyListGraph::AdjacencyListGraph(bool inIsDirected)
    : Graph(inIsDirected)
{

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

    if(!isDirected)
    {
        if(end >= adjList.size())
        {
            addNode();
        }

        adjList[end].push_back(Edge{start, weight});
    }
}

void AdjacencyListGraph::addNode()
{
    const qsizetype oldSize = adjList.size();
    adjList.resize(oldSize + 1);
    adjList[oldSize] = QList<Edge>();
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

AdjacencyMatrixGraph::AdjacencyMatrixGraph(int vertices, bool inIsDirected)
    : Graph(inIsDirected)
{
    adjMatrix.fill(QList<int>{}, vertices);
    for(auto& neighbours : adjMatrix)
    {
        neighbours.fill(INT_MIN, vertices);
    }
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

    if(!isDirected)
    {
        adjMatrix[end][start] = weight;
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
