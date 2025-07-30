#include "algorithm_visualizer.h"

#include "property_editor_factory.h"

#include <QMetaProperty>
#include <QVBoxLayout>

AlgorithmVisualizer::AlgorithmVisualizer(QObject *parent)
    : QObject(parent)
    , stepTime(1000)
{
    visualizationTimer.setInterval(stepTime);
    connect(&visualizationTimer, &QTimer::timeout, this, &AlgorithmVisualizer::updateVisualization);
}

AlgorithmVisualizer::~AlgorithmVisualizer()
{

}

void AlgorithmVisualizer::clear()
{
    if(visualizationTimer.isActive())
    {
        emit finished();
    }
    visualizationTimer.stop();
}

bool AlgorithmVisualizer::isDataStructureSupported(const DataStructure *dataStructure, QString& outInfo) const
{
    return !!dataStructure;
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
    return PropertyEditorFactory::get().createPropertiesWidget(this, parent, hiddenProperties);
}

int AlgorithmVisualizer::getStepTime() const
{
    return stepTime;
}

void AlgorithmVisualizer::setStepTime(int newStepTime)
{
    if(stepTime == newStepTime)
    {
        return;
    }

    stepTime = newStepTime;
    visualizationTimer.setInterval(stepTime);

    emit stepTimeChanged();
}
