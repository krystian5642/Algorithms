
#include "graph_algorithms.h"
#include "graph.h"

#include <windows.h>

#include <QElapsedTimer>
#include <QRandomGenerator>

GraphAlgorithm::GraphAlgorithm(QObject *parent)
    : Algorithm(parent)
{
}

void GraphAlgorithm::run()
{
    requestedEnd = false;
    emit started();

    QList<QPointF> result;
    result.reserve(iterationsNumber);

    QScopedPointer<Graph> testGraph(new AdjacencyListGraph);
    setGraph(testGraph.get());

    for(int i = 1; i <= iterationsNumber; ++i)
    {      
        for(int j = 0; j < i; ++j)
        {         
            if(requestedEnd)
            {
                break;
            }
            testGraph->addEdge(j, testGraph->getRandomValue());
        }

        if(requestedEnd)
        {
            clear();
            break;
        }

        init();

        ULONG64 start;
        QueryThreadCycleTime(GetCurrentThread(), &start);

        execute();

        ULONG64 end;
        QueryThreadCycleTime(GetCurrentThread(), &end);

        const QString selectedComplexity = getSelectedComplexity();
        auto it = std::find_if(complexityList.begin(), complexityList.end(), [&](const ComplexityPair& pair)
        {
            return pair.first == selectedComplexity;
        });

        const QPointF point((it->second)(i - 1, testGraph->getVerticesNum(), testGraph->getEdgesNum()), end - start);
        result.append(point);

        clear();
    }

    setGraph(nullptr);

    emit finished(result);
}

GraphAlgorithm::~GraphAlgorithm()
{

}

Graph *GraphAlgorithm::getGraph() const
{
    return graph;
}

void GraphAlgorithm::setGraph(Graph *newGraph)
{
    graph = newGraph;
}

BFSIterative::BFSIterative(QObject *parent)
    : GraphAlgorithm(parent)
{
    setObjectName("Breadth First Search (Iterative ?)");
}

void BFSIterative::init()
{
    GraphAlgorithm::init();

    nodeQueue.reserve(graph->getVerticesNum());
    visited.fill(false, graph->getVerticesNum());
}

void BFSIterative::clear()
{
    GraphAlgorithm::clear();

    nodeQueue.clear();
    visited.clear();
}

void BFSIterative::execute()
{
    nodeQueue.enqueue(0);
    visited[0] = true;

    while(!nodeQueue.empty())
    {
        const int first = nodeQueue.dequeue();

        auto func = [&](int start, int neighbour, int weight)
        {
            if(!visited[neighbour])
            {
                visited[neighbour] = true;
                nodeQueue.enqueue(neighbour);
            }
        };

        graph->forEachNeighbor(first, func);
    }
}

DFSRecursive::DFSRecursive(QObject *parent)
    : GraphAlgorithm(parent)
{
    setObjectName("Depth First Search (Recursive)");
}

void DFSRecursive::execute()
{
    // const int start = graph->getFirstValue();

    // QSet<int> visited;
    // visited.reserve(graph->getNodesNum());
    // visited.insert(start);

    // DFSHelper(start, visited);
}

void DFSRecursive::DFSHelper(int begin, QSet<int>& visited)
{
    // const QList<GraphEdge>& neighbourEdges = graph->getNeighbourEdges(begin);
    // for(const auto& neighbour : neighbourEdges)
    // {
    //     const int value = neighbour.getEndValue();
    //     if(!visited.contains(value))
    //     {
    //         visited.insert(value);
    //         DFSHelper(value, visited);
    //     }
    // }
}
