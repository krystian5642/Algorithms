#include "graph_builders.h"

#include "../core/property_editor_factory.h"

#include "graph.h"

#include <QRandomGenerator>

GraphBuilder::GraphBuilder(QObject *parent)
    : DataStructureBuilder{parent}
    , buildIterations(100)
{
    dataStructures.push_back(qMakePair("Adjacency List",   []() {return new AdjacencyListGraph; }));
    dataStructures.push_back(qMakePair("Adjacency Matrix", []() {return new AdjacencyMatrixGraph; }));
}

GraphBuilder::~GraphBuilder()
{

}

QWidget *GraphBuilder::createPropertiesWidget(QWidget *parent)
{
    QWidget* propertiesWidget  = DataStructureBuilder::createPropertiesWidget(parent);

    QStringList stringList;
    stringList.reserve(dataStructures.size());

    for(const auto& dataStructure : dataStructures)
    {
        stringList.push_back(dataStructure.first);
    }

    PropertyEditorFactory::get().addStringListComboBox(propertiesWidget, stringList, "implemention", selectedImplementation);

    return propertiesWidget;
}

Graph *GraphBuilder::createGraph() const
{
    auto createGraphFunc = std::find_if(dataStructures.begin(), dataStructures.end(), [&](const QPair<QString, std::function<DataStructure*()>>& pair)
    {
        return pair.first == selectedImplementation;
    })->second;

    return dynamic_cast<Graph*>(createGraphFunc());
}

GeneralGraphBuilder::GeneralGraphBuilder(QObject *parent)
    : GraphBuilder{parent}
    , addEdgePropability(0.5)
{
    setObjectName("General Graph");
}

DataStructure* GeneralGraphBuilder::buildDataStructure()
{
    Graph* graph = createGraph();

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
                const double randomDouble = QRandomGenerator::global()->generateDouble();
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

double GeneralGraphBuilder::getAddEdgePropability() const
{
    return addEdgePropability;
}

void GeneralGraphBuilder::setAddEdgePropability(double newAddEdgePropability)
{
    if (qFuzzyCompare(addEdgePropability, newAddEdgePropability))
    {
        return;
    }

    addEdgePropability = newAddEdgePropability;
    emit addEdgePropabilityChanged();
}

GridGraphBuilder::GridGraphBuilder(QObject *parent)
    : GraphBuilder{parent}
{
    setObjectName("Grid Graph");
}

DataStructure *GridGraphBuilder::buildDataStructure()
{
    Graph* graph = createGraph();

    QList<int> prevRow(buildIterations);
    for(int i = 0; i < buildIterations; i++)
    {
        int prevValue = -1;
        for(int j = 0; j < buildIterations; j++)
        {
            graph->addNode();
            const int value = i * buildIterations + j;

            if(j > 0)
            {
                graph->addEdge(prevValue, value);
            }

            if(i > 0)
            {
                graph->addEdge(prevRow[j], value);
            }

            prevRow[j] = value;

            prevValue = value;
        }
    }

    return graph;
}
