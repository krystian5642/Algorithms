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

QWidget *AlgorithmVisualizer::createPropertiesWidget() const
{
    QWidget* propertiesWidget = new QWidget();

    QVBoxLayout* propertyLayout = new QVBoxLayout(propertiesWidget);

    const QMetaObject* myMetaObject = metaObject();
    for (int i = 0; i < myMetaObject->propertyCount(); ++i)
    {
        const QMetaProperty metaProperty = myMetaObject->property(i);
        if(QString(metaProperty.name()) != "objectName")
        {
            propertyLayout->addLayout(PropertyLayoutFactory::get().createLayoutForProperty(metaProperty, const_cast<AlgorithmVisualizer*>(this), propertiesWidget));
        }
    }

    propertyLayout->addStretch();
    return propertiesWidget;
}

int AlgorithmVisualizer::getStepTime() const
{
    return stepTime;
}

void AlgorithmVisualizer::setStepTime(int newStepTime)
{
    stepTime = newStepTime;
}
