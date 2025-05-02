#include "algorithm_visualizer.h"

#include "property_layout_factory.h"

#include <QMetaProperty>
#include <QVBoxLayout>

AlgorithmVisualizer::AlgorithmVisualizer(QObject *parent)
    : QObject(parent)
{
    visualizationTimer.setInterval(stepTime);
}

AlgorithmVisualizer::~AlgorithmVisualizer()
{
}

void AlgorithmVisualizer::reset()
{
    visualizationTimer.stop();
}

void AlgorithmVisualizer::setPause(bool pause)
{
    if(pause)
    {
        visualizationTimer.stop();
    }
    else
    {
        visualizationTimer.start();
    }
}

QWidget *AlgorithmVisualizer::createPropertiesWidget(QWidget* parent)
{
    return PropertyLayoutFactory::get().createPropertiesWidget(this, parent);
}

int AlgorithmVisualizer::getStepTime() const
{
    return stepTime;
}

void AlgorithmVisualizer::setStepTime(int newStepTime)
{
    stepTime = newStepTime;
}
