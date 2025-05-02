#include "algorithm.h"
#include "property_layout_factory.h"

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

    complexityList.push_back(qMakePair("O(1)", [](int I, int V, int E) { return I; }));
    complexityList.push_back(qMakePair("O(V)", [](int I, int V, int E) { return V; }));
    complexityList.push_back(qMakePair("O(E)", [](int I, int V, int E) { return E; }));
    complexityList.push_back(qMakePair("O(V+E)", [](int I, int V, int E) { return V + E; }));
    complexityList.push_back(qMakePair("O(V^2)", [](int I, int V, int E) { return V * V; }));
}

Algorithm::~Algorithm()
{

}

QWidget *Algorithm::createPropertiesWidget(QWidget* parent)
{
    QWidget* propertiesWidget = PropertyLayoutFactory::get().createPropertiesWidget(this, parent, false);
    QBoxLayout* layout = qobject_cast<QBoxLayout*>(propertiesWidget->layout());

    QHBoxLayout* horizontalLayout = new QHBoxLayout(propertiesWidget);

    QLabel* complexityLabel = new QLabel("complexity");
    complexityComboBox = new QComboBox(propertiesWidget);

    for(const auto& complexity : complexityList)
    {
        complexityComboBox->addItem(complexity.first);
    }

    horizontalLayout->addWidget(complexityLabel);
    horizontalLayout->addWidget(complexityComboBox);

    layout->addLayout(horizontalLayout);
    layout->addStretch(1);

    return propertiesWidget;
}

QString Algorithm::getSelectedComplexity() const
{
    return complexityComboBox->currentText();
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
