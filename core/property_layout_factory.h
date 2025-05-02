#ifndef PROPERTY_LAYOUT_FACTORY_H
#define PROPERTY_LAYOUT_FACTORY_H

#include <QHash>
#include <QObject>

class QBoxLayout;

namespace PropertyLayoutFactoryUtils
{
    QString convertCamelCaseToSpaces(const QString& input);
}

class PropertyLayoutFactory
{
    using CreateLayoutFunction = std::function<QBoxLayout*(const QMetaProperty&, QObject*, QWidget*)>;

public:
    static PropertyLayoutFactory& get();

    void registerCreateLayoutForType(int type, CreateLayoutFunction func);
    QBoxLayout* createLayoutForProperty(const QMetaProperty& property, QObject* propertyObject, QWidget* parent);

    QWidget* createPropertiesWidget(QObject *object, QWidget* parent = nullptr, bool addStretch = true);

private:
    PropertyLayoutFactory();

    QHash<int, CreateLayoutFunction> layoutCreators;
};

#endif // PROPERTY_LAYOUT_FACTORY_H
