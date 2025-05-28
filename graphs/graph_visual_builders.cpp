#include "graph_visual_builders.h"

#include "widgets/graph_widget.h"
#include "../core/widgets/properties_dialog.h"

#include <QRandomGenerator>

GraphVisualBuilder::GraphVisualBuilder(QObject *parent)
    : DataStructureVisualBuilder{parent}
{

}

GraphVisualBuilder::~GraphVisualBuilder()
{

}

GeneralGraphVisualBuilder::GeneralGraphVisualBuilder(QObject *parent)
    : GraphVisualBuilder{parent}
    , nodesNumber(50)
    , minXLocation(0)
    , maxXLocation(1000)
    , minYLocation(0)
    , maxYLocation(1000)
    , addEdgePropability(0.2)
{
    setObjectName("General Graph");
}

void GeneralGraphVisualBuilder::buildDataStructureVisualization(DataStructureWidget *dataStructureWidget)
{
    if(!dataStructureWidget)
    {
        return;
    }

    PropertiesDialog propertiesDialog(this);
    const int result = propertiesDialog.exec();

    if(result == QDialog::DialogCode::Accepted)
    {
        GraphWidget* graphWidget = qobject_cast<GraphWidget*>(dataStructureWidget);
        graphWidget->clear();

        for(int i=0; i < nodesNumber; i++)
        {
            const QPoint randomLocation(QRandomGenerator::global()->bounded(minXLocation, maxXLocation)
                                        , QRandomGenerator::global()->bounded(minYLocation, maxYLocation));

            graphWidget->addNode(randomLocation);
        }
        graphWidget->generateRandomEdges(addEdgePropability);
    }
}

int GeneralGraphVisualBuilder::getNodesNumber() const
{
    return nodesNumber;
}

void GeneralGraphVisualBuilder::setNodesNumber(int newNodesNumber)
{
    if (nodesNumber == newNodesNumber)
    {
        return;
    }
    nodesNumber = newNodesNumber;
    emit nodesNumberChanged();
}

int GeneralGraphVisualBuilder::getMinXLocation() const
{
    return minXLocation;
}

void GeneralGraphVisualBuilder::setMinXLocation(int newMinXLocation)
{
    if (minXLocation == newMinXLocation)
    {
        return;
    }
    minXLocation = newMinXLocation;
    emit minXLocationChanged();
}

int GeneralGraphVisualBuilder::getMaxXLocation() const
{
    return maxXLocation;
}

void GeneralGraphVisualBuilder::setMaxXLocation(int newMaxXLocation)
{
    if (maxXLocation == newMaxXLocation)
    {
        return;
    }
    maxXLocation = newMaxXLocation;
    emit maxXLocationChanged();
}

int GeneralGraphVisualBuilder::getMinYLocation() const
{
    return minYLocation;
}

void GeneralGraphVisualBuilder::setMinYLocation(int newMinYLocation)
{
    if (minYLocation == newMinYLocation)
    {
        return;
    }
    minYLocation = newMinYLocation;
    emit minYLocationChanged();
}

int GeneralGraphVisualBuilder::getMaxYLocation() const
{
    return maxYLocation;
}

void GeneralGraphVisualBuilder::setMaxYLocation(int newMaxYLocation)
{
    if (maxYLocation == newMaxYLocation)
    {
        return;
    }
    maxYLocation = newMaxYLocation;
    emit maxYLocationChanged();
}

double GeneralGraphVisualBuilder::getAddEdgePropability() const
{
    return addEdgePropability;
}

void GeneralGraphVisualBuilder::setAddEdgePropability(double newAddEdgePropability)
{
    if (qFuzzyCompare(addEdgePropability, newAddEdgePropability))
    {
        return;
    }
    addEdgePropability = newAddEdgePropability;
    emit addEdgePropabilityChanged();
}

GridGraphVisualBuilder::GridGraphVisualBuilder(QObject *parent)
    : GraphVisualBuilder{parent}
{
    setObjectName("Grid Graph");
}

void GridGraphVisualBuilder::buildDataStructureVisualization(DataStructureWidget *dataStructureWidget)
{
    if(!dataStructureWidget)
    {
        return;
    }

    // this info will be taken from UI later
    constexpr int columns = 20;
    constexpr int rows = 20;
    constexpr int nodeSpace = 50;
    constexpr QPoint startLoc(50, 50);

    GraphWidget* graphWidget = qobject_cast<GraphWidget*>(dataStructureWidget);

    QList<int> prevRow(columns);
    for(int i = 0; i < rows; i++)
    {
        int prevValue = -1;
        for(int j = 0; j < columns; j++)
        {
            const int value = i * rows + j;

            graphWidget->addNode(QPoint(startLoc.x()  + nodeSpace * j, startLoc.y() + nodeSpace * i));

            if(j > 0 && prevValue != -1)
            {
                graphWidget->addEdge(prevValue, value);
            }

            if(i > 0 && prevRow[j] != -1)
            {
                graphWidget->addEdge(prevRow[j], value);
            }

            prevRow[j] = value;
            prevValue = value;
        }
    }
}
