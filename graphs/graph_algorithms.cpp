
#include "graph_algorithms.h"
#include "graph.h"

#include "../core/property_layout_factory.h"

#include "general_graph_builder.h"

#include <windows.h>

#include <QBoxLayout>
#include <QElapsedTimer>
#include <QRandomGenerator>
#include <QWidget>

GraphAlgorithm::GraphAlgorithm(QObject *parent)
    : Algorithm(parent)
    , graph(nullptr)
    , addEdgePropability(0.5)
{
}

GraphAlgorithm::~GraphAlgorithm()
{

}

QWidget *GraphAlgorithm::createPropertiesWidget(QWidget *parent, bool addStretch)
{
    QWidget* propertiesWidget = Algorithm::createPropertiesWidget(parent, false);

    QStringList implementationsStringList;
    implementationsStringList.push_back("Adjacency List");
    implementationsStringList.push_back("Adjacency Matrix");

    PropertyLayoutFactory::get().addComboBox(propertiesWidget, implementationsStringList, "implementation", selectedImplementation);

    QBoxLayout* layout = qobject_cast<QBoxLayout*>(propertiesWidget->layout());
    layout->addStretch(1);

    return propertiesWidget;
}

void GraphAlgorithm::run()
{
    requestedEnd = false;
    emit started();

    QList<QPointF> result;
    result.reserve(iterationsNumber);

    QScopedPointer<GeneralGraphBuilder> graphBuilder(new GeneralGraphBuilder);
    graphBuilder->addEdgePropability = addEdgePropability;

    auto complexityFunc = std::find_if(complexityList.begin(), complexityList.end(), [&](const StringToFunction& pair)
    {
        return pair.first == selectedComplexity;
    })->second;

    for(int i = 0; i < iterationsNumber; ++i)
    {
        if(requestedEnd)
        {
            clear();
            break;
        }

        graphBuilder->buildIterations = i + 1;

        graphBuilder->setGraph(dynamic_cast<Graph*>(createSelectedDataStructure())); // temp

        QScopedPointer<Graph> testGraph(dynamic_cast<Graph*>(graphBuilder->buildDataStructure()));
        setGraph(testGraph.get());

        init();

        ULONG64 start;
        QueryThreadCycleTime(GetCurrentThread(), &start);

        execute();

        ULONG64 end;
        QueryThreadCycleTime(GetCurrentThread(), &end);

        const QPointF point(complexityFunc(i, testGraph->getVerticesNum(), testGraph->getEdgesNum()), end - start);
        result.append(point);

        clear();
    }

    setGraph(nullptr);

    emit finished(result);
}

Graph *GraphAlgorithm::getGraph() const
{
    return graph;
}

void GraphAlgorithm::setGraph(Graph *newGraph)
{
    graph = newGraph;
}

double GraphAlgorithm::getAddEdgePropability() const
{
    return addEdgePropability;
}

void GraphAlgorithm::setAddEdgePropability(double newAddEdgePropability)
{
    if (qFuzzyCompare(addEdgePropability, newAddEdgePropability))
    {
        return;
    }

    addEdgePropability = newAddEdgePropability;
    emit addEdgePropabilityChanged();
}

DataStructure *GraphAlgorithm::createSelectedDataStructure() const
{
    if(selectedImplementation == "Adjacency List")
    {
        return new AdjacencyListGraph;
    }
    else if(selectedImplementation == "Adjacency Matrix")
    {
        return new AdjacencyMatrixGraph;
    }
    return nullptr;
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
