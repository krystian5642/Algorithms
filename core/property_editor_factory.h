#ifndef PROPERTY_EDITOR_FACTORY_H
#define PROPERTY_EDITOR_FACTORY_H

#include <QHash>
#include <QObject>

namespace PropertyEditorFactoryUtils
{
    QString convertCamelCaseToSpaces(const QString& input);
}

class PropertyEditorFactory
{
    using CreateWidgetFunction = std::function<QWidget*(const QMetaProperty&, QObject*, QWidget*)>;

public:
    static PropertyEditorFactory& get();

    void registerCreateWidgetForType(int type, CreateWidgetFunction func);
    QWidget* createPropertyWidget(const QMetaProperty& property, QObject* propertyObject, QWidget* parent);

    QWidget* createPropertiesWidget(QObject *object, QWidget* parent = nullptr);
    void addStringListComboBox(QWidget *propertiesWidget, const QStringList &values, const QString& title, QString &changedValue);

private:
    PropertyEditorFactory();

    QHash<int, CreateWidgetFunction> widgetCreators;
};

#endif // PROPERTY_EDITOR_FACTORY_H
