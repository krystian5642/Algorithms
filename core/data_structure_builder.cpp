#include "data_structure_builder.h"

#include "property_editor_factory.h"

DataStructureBuilder::DataStructureBuilder(QObject *parent)
    : QObject{parent}
{

}

DataStructureBuilder::~DataStructureBuilder()
{

}

QWidget *DataStructureBuilder::createPropertiesWidget(QWidget *parent)
{
    return PropertyEditorFactory::get().createPropertiesWidget(this, parent);
}
