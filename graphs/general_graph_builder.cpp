#include "general_graph_builder.h"

#include "graph.h"

#include <QRandomGenerator>

GeneralGraphBuilder::GeneralGraphBuilder(QObject *parent)
    : DataStructureBuilder{parent}
    , buildIterations(1000)
    , addEdgePropability(0.5)
{
    setObjectName("General Graph Builder");
}

DataStructure* GeneralGraphBuilder::buildDataStructure()
{
    //Graph* graph = new AdjacencyListGraph;

    for(int i = 0; i < buildIterations; ++i)
    {
        graph->addNode();
    }

    auto func1 = [&](int value1)
    {
        auto func2 = [&](int value2)
        {
            if(value1 != value2)
            {
                double randomDouble = QRandomGenerator::global()->generateDouble();
                if(randomDouble < addEdgePropability)
                {
                    graph->addEdge(value1, value2);
                }
            }
        };

        graph->forEachNode(func2);
    };

    graph->forEachNode(func1);

    return graph;
}
