#include "graph_visualization_settings.h"

GraphVisualizationSettings::GraphVisualizationSettings(QObject *parent)
    : QObject{parent}
    , showWeights(true)
    , graphDirected(true)
{

}

bool GraphVisualizationSettings::getShowWeights() const
{
    return showWeights;
}

void GraphVisualizationSettings::setShowWeights(bool newShowWeights)
{
    if (showWeights == newShowWeights)
    {
        return;
    }

    showWeights = newShowWeights;
    emit showWeightsChanged(showWeights);
}

bool GraphVisualizationSettings::getGraphDirected() const
{
    return graphDirected;
}

void GraphVisualizationSettings::setGraphDirected(bool newGraphDirected)
{
    if (graphDirected == newGraphDirected)
    {
        return;
    }

    graphDirected = newGraphDirected;
    emit graphDirectedChanged(graphDirected);
}
