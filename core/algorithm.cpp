#include "algorithm.h"
#include "property_editor_factory.h"

#include <QComboBox>
#include <QLabel>
#include <QLayout>
#include <QWidget>

Algorithm::Algorithm(QObject* parent)
    : QObject(parent)
    , iterationsNumber(1000)
    , requestedEnd(false)
{
    setAutoDelete(false);

    complexityList.push_back(qMakePair("O(1)",   [](int I, int V, int E) { return I; }));
    complexityList.push_back(qMakePair("O(V)",   [](int I, int V, int E) { return V; }));
    complexityList.push_back(qMakePair("O(E)",   [](int I, int V, int E) { return E; }));
    complexityList.push_back(qMakePair("O(V+E)", [](int I, int V, int E) { return V + E; }));
    complexityList.push_back(qMakePair("O(V^2)", [](int I, int V, int E) { return V * V; }));
}

Algorithm::~Algorithm()
{

}

QWidget *Algorithm::createPropertiesWidget(QWidget* parent)
{
    QStringList complexityStringList;
    complexityStringList.reserve(complexityList.size());

    for(const auto& complexity : complexityList)
    {
        complexityStringList.push_back(complexity.first);
    }

    PropertyEditorFactory& propertyEditorFactory = PropertyEditorFactory::get();

    QWidget* propertiesWidget = propertyEditorFactory.createPropertiesWidget(this, parent);
    propertyEditorFactory.addStringListComboBox(propertiesWidget, complexityStringList, "complexity", selectedComplexity);

    return propertiesWidget;
}

void Algorithm::requestEnd()
{
    requestedEnd = true;
}

int Algorithm::getIterationsNumber() const
{
    return iterationsNumber;
}

void Algorithm::setIterationsNumber(int newIterationsNumber)
{
    if (iterationsNumber == newIterationsNumber)
    {
        return;
    }

    iterationsNumber = newIterationsNumber;
    emit iterationsNumberChanged();
}
