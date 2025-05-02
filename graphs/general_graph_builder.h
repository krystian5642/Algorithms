#ifndef GENERAL_GRAPH_BUILDER_H
#define GENERAL_GRAPH_BUILDER_H

#include "../core/data_structure_builder.h"

#include "graph.h" // temp !!

class GeneralGraphBuilder : public DataStructureBuilder
{
    Q_OBJECT
public:
    explicit GeneralGraphBuilder(QObject *parent = nullptr);

    void setGraph(Graph* inGraph) { graph = inGraph; }; // temp
    DataStructure* buildDataStructure() override;

    int buildIterations;
    double addEdgePropability;

    Graph* graph;
};

#endif // GENERAL_GRAPH_BUILDER_H
