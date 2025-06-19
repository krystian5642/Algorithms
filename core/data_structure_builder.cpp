#include "data_structure_builder.h"

#include "property_editor_factory.h"

#include <QMetaProperty>

DataStructureBuilder::DataStructureBuilder(QObject *parent)
    : QObject{parent}
{

}

DataStructureBuilder::~DataStructureBuilder()
{

}

QWidget *DataStructureBuilder::createPropertiesWidget(QWidget *parent)
{
    return PropertyEditorFactory::get().createPropertiesWidget(this, parent, hiddenProperties);
}

void DataStructureBuilder::appendPropertiesInfo(QString &infoText)
{
    const QMetaObject* myMetaObject = metaObject();
    for (int i = 0; i < myMetaObject->propertyCount(); ++i)
    {
        const QMetaProperty metaProperty = myMetaObject->property(i);
        if(QString(metaProperty.name()) != "objectName")
        {
            infoText.append(PropertyEditorFactoryUtils::convertCamelCaseToSpaces(metaProperty.name()) + " : " + metaProperty.read(this).toString());
            infoText.append("\n");
        }
    }
}
