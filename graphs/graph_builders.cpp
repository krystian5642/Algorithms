#include "graph_builders.h"

#include "../core/property_editor_factory.h"

#include "graph.h"

#include <QRandomGenerator>

GraphBuilder::GraphBuilder(QObject *parent)
    : DataStructureBuilder(parent)
    , buildIterations(100)
    , minWeight(1)
    , maxWeight(100)
{
    dataStructures.push_back(qMakePair("Adjacency List",   []() {return new AdjacencyListGraph; }));
    dataStructures.push_back(qMakePair("Adjacency Matrix", []() {return new AdjacencyMatrixGraph; }));
}

GraphBuilder::~GraphBuilder()
{

}

QWidget *GraphBuilder::createPropertiesWidget(QWidget *parent)
{
    QWidget* propertiesWidget = DataStructureBuilder::createPropertiesWidget(parent);

    QStringList stringList;
    stringList.reserve(dataStructures.size());

    for(const auto& dataStructure : dataStructures)
    {
        stringList.push_back(dataStructure.first);
    }

    PropertyEditorFactory::get().addStringListComboBox(propertiesWidget, stringList, "implemention", selectedImplementation);

    return propertiesWidget;
}

void GraphBuilder::appendPropertiesInfo(QString &infoText)
{
    DataStructureBuilder::appendPropertiesInfo(infoText);

    infoText.append("implemention : " + selectedImplementation);
    infoText.append("\n");
}

Graph *GraphBuilder::createGraph() const
{
    auto createGraphFunc = std::find_if(dataStructures.begin(), dataStructures.end(), [&](const QPair<QString, std::function<DataStructure*()>>& pair)
    {
        return pair.first == selectedImplementation;
    })->second;

    Graph* graph = qobject_cast<Graph*>(createGraphFunc());
    graph->setIsDirected(isGraphDirected);

    return graph;
}

bool GraphBuilder::getIsGraphDirected() const
{
    return isGraphDirected;
}

void GraphBuilder::setIsGraphDirected(bool newIsGraphDirected)
{
    if (isGraphDirected == newIsGraphDirected)
    {
        return;
    }

    isGraphDirected = newIsGraphDirected;
    emit isGraphDirectedChanged();
}

int GraphBuilder::getMinWeight() const
{
    return minWeight;
}

void GraphBuilder::setMinWeight(int newMinWeight)
{
    if (minWeight == newMinWeight)
    {
        return;
    }

    minWeight = newMinWeight;
    emit minWeightChanged();
}

int GraphBuilder::getMaxWeight() const
{
    return maxWeight;
}

void GraphBuilder::setMaxWeight(int newMaxWeight)
{
    if (maxWeight == newMaxWeight)
    {
        return;
    }

    maxWeight = newMaxWeight;
    emit maxWeightChanged();
}

GeneralGraphBuilder::GeneralGraphBuilder(QObject *parent)
    : GraphBuilder(parent)
    , addEdgePropability(0.5)
{
    setObjectName("General Graph");
}

DataStructure* GeneralGraphBuilder::createDataStructure()
{
    Graph* graph = createGraph();

    for(int i = 0; i < buildIterations; ++i)
    {
        graph->addNode();
    }

    graph->generateRandomEdges(addEdgePropability, minWeight, maxWeight);

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
    : GraphBuilder(parent)
{
    setObjectName("Grid Graph");
}

DataStructure *GridGraphBuilder::createDataStructure()
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
                graph->addEdge(prevValue, value, QRandomGenerator::global()->bounded(minWeight, maxWeight + 1));
            }

            if(i > 0)
            {
                graph->addEdge(prevRow[j], value, QRandomGenerator::global()->bounded(minWeight, maxWeight + 1));
            }

            prevRow[j] = value;

            prevValue = value;
        }
    }

    return graph;
}

TreeGraphBuilder::TreeGraphBuilder(QObject *parent)
    : GraphBuilder(parent)
{
    setObjectName("Tree Graph Builder");
    isGraphDirected = false;

    hiddenProperties.push_back("isGraphDirected");
}

DataStructure *TreeGraphBuilder::createDataStructure()
{
    Graph* graph = createGraph();

    graph->addNode(); // root
    int parent = 0;

    for(int i = 1; i <= buildIterations * 2; i+=2)
    {
        graph->addEdge(i, parent, QRandomGenerator::global()->bounded(minWeight, maxWeight + 1));
        graph->addEdge(i + 1, parent, QRandomGenerator::global()->bounded(minWeight, maxWeight + 1));

        parent = i;
    }

    return graph;
}

GeneralResidualGraphBuilder::GeneralResidualGraphBuilder(QObject *parent)
    : GeneralGraphBuilder(parent)
{
    setObjectName("General Residual Builder");

    dataStructures.clear();
    dataStructures.push_back(qMakePair("Residual Graph",   []() {return new ResidualGraph; }));

    hiddenProperties.push_back("isGraphDirected");
}

GridResidualGraphBuilder::GridResidualGraphBuilder(QObject *parent)
    : GridGraphBuilder(parent)
{
    setObjectName("Grid Residual Builder");

    dataStructures.clear();
    dataStructures.push_back(qMakePair("Residual Graph",   []() {return new ResidualGraph; }));

    hiddenProperties.push_back("isGraphDirected");
}

TreeResidualGraphBuilder::TreeResidualGraphBuilder(QObject *parent)
    : TreeGraphBuilder(parent)
{
    setObjectName("Tree Residual Builder");

    dataStructures.clear();
    dataStructures.push_back(qMakePair("Residual Graph",   []() {return new ResidualGraph; }));
}
