#include "algorithm.h"
#include "property_editor_factory.h"

#include <QComboBox>
#include <QLabel>
#include <QLayout>
#include <QMetaProperty>
#include <QWidget>

Algorithm::Algorithm(QObject* parent)
    : QObject(parent)
    , iterationsNumber(1000)
#ifdef QT_DEBUG
    , isDebugRun(false)
#endif
    , requestedEnd(false)
    , currentIteration(0)
{
    setAutoDelete(false);
}

Algorithm::~Algorithm()
{

}

bool Algorithm::canRunAlgorithm(QString &outInfo) const
{
    return true;
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

void Algorithm::appendPropertiesInfo(QString &infoText)
{
    const QMetaObject* myMetaObject = metaObject();
    for (int i = 0; i < myMetaObject->propertyCount(); ++i)
    {
        const QMetaProperty metaProperty = myMetaObject->property(i);

        QString propertyName(metaProperty.name());
        if(propertyName == "objectName")
        {
            propertyName = "algorithm";
        }

        if(propertyName != "iterationsNumber")
        {
            infoText.append(PropertyEditorFactoryUtils::convertCamelCaseToSpaces(propertyName) + " : " + metaProperty.read(this).toString());
            infoText.append("\n");
        }
    }

    infoText.append("complexity : " + selectedComplexity);
    infoText.append("\n");
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

#ifdef QT_DEBUG
bool Algorithm::getIsDebugRun() const
{
    return isDebugRun;
}

void Algorithm::setIsDebugRun(bool newIsDebugRun)
{
    isDebugRun = newIsDebugRun;
}
#endif
