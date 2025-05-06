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

GeneralGraphBuilder::GeneralGraphBuilder(QObject *parent)
    : GraphBuilder{parent}
    , addEdgePropability(0.5)
{
    setObjectName("General Graph Builder");
}

DataStructure* GeneralGraphBuilder::buildDataStructure()
{
    auto createGraphFunc = std::find_if(dataStructures.begin(), dataStructures.end(), [&](const QPair<QString, std::function<DataStructure*()>>& pair)
    {
        return pair.first == selectedImplementation;
    })->second;

    Graph* graph = dynamic_cast<Graph*>(createGraphFunc());

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
    , rows(10)
    , columns(10)
{
    setObjectName("Grid Graph Builder");
}

DataStructure *GridGraphBuilder::buildDataStructure()
{
    return nullptr;
}

int GridGraphBuilder::getRows() const
{
    return rows;
}

void GridGraphBuilder::setRows(int newRows)
{
    if (rows == newRows)
    {
        return;
    }

    rows = newRows;
    emit rowsChanged();
}

int GridGraphBuilder::getColumns() const
{
    return columns;
}

void GridGraphBuilder::setColumns(int newColumns)
{
    if (columns == newColumns)
    {
        return;
    }

    columns = newColumns;
    emit columnsChanged();
}
