#include "property_editor_factory.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLineEdit>
#include <QMetaProperty>
#include <QWidget>

namespace PropertyEditorFactoryUtils
{
    QString convertCamelCaseToSpaces(const QString& input)
    {
        if (input.isEmpty())
        {
            return input;
        }

        QString output;
        output.append(input[0]);

        for(int i = 1; i < input.length(); ++i)
        {
            if(input[i].isUpper())
            {
                output.append(" ");
            }
            output.append(input[i].toLower());
        }
        return output;
    }
}

PropertyEditorFactory &PropertyEditorFactory::get()
{
    static PropertyEditorFactory factor;
    return factor;
}

void PropertyEditorFactory::registerCreateWidgetForType(int type, CreateWidgetFunction func)
{
    widgetCreators[type] = func;
}

QWidget *PropertyEditorFactory::createPropertyWidget(const QMetaProperty &property, QObject *propertyObject, QWidget *parent)
{
    auto it = widgetCreators.constFind(property.typeId());
    if(it != widgetCreators.constEnd())
    {
        return (it.value())(property, propertyObject, parent);
    }
    return nullptr;
}

QWidget *PropertyEditorFactory::createPropertiesWidget(QObject *object, QWidget* parent, const QStringList& hiddenProperties)
{
    if(!object)
    {
        return nullptr;
    }

    QWidget* propertiesWidget = new QWidget(parent);

    QVBoxLayout* verticalLayout = new QVBoxLayout(propertiesWidget);
    verticalLayout->setAlignment(Qt::AlignTop);

    QFormLayout* layout = new QFormLayout;

    const QMetaObject* myMetaObject = object->metaObject();
    for (int i = 0; i < myMetaObject->propertyCount(); ++i)
    {
        const QMetaProperty metaProperty = myMetaObject->property(i);
        const QString propertyNameAsString(metaProperty.name());
        if(propertyNameAsString != "objectName" && !hiddenProperties.contains(propertyNameAsString))
        {
            layout->addRow(PropertyEditorFactoryUtils::convertCamelCaseToSpaces(metaProperty.name()), createPropertyWidget(metaProperty, object, propertiesWidget));
        }
    }

    verticalLayout->addLayout(layout);

    return propertiesWidget;
}

void PropertyEditorFactory::addStringListComboBox(QWidget *propertiesWidget, const QStringList &values, const QString& title, QString &changedValue)
{
    QComboBox* comboBox = new QComboBox(propertiesWidget);

    for(const auto& value : values)
    {
        comboBox->addItem(value);
    }

    changedValue = comboBox->currentText();
    QObject::connect(comboBox, &QComboBox::currentTextChanged, propertiesWidget, [&changedValue](const QString& currentText)
    {
        changedValue = currentText;
    });

    QFormLayout* layout = propertiesWidget->findChild<QFormLayout*>();
    layout->addRow(title, comboBox);
}

PropertyEditorFactory::PropertyEditorFactory()
{
    registerCreateWidgetForType(QMetaType::Int, [](const QMetaProperty& property, QObject* propertyObject, QWidget* parent)
    {
        QLineEdit* lineEdit = new QLineEdit(parent);
        lineEdit->setText(property.read(propertyObject).toString());
        lineEdit->setValidator(new QIntValidator(lineEdit));

        QObject::connect(lineEdit, &QLineEdit::textChanged, propertyObject, [property, propertyObject](const QString& newText)
        {
            propertyObject->setProperty(property.name(), newText.toInt());
        });

        return lineEdit;
    });

    registerCreateWidgetForType(QMetaType::Double, [](const QMetaProperty& property, QObject* propertyObject, QWidget* parent)
    {
        QLineEdit* lineEdit = new QLineEdit(parent);
        lineEdit->setText(property.read(propertyObject).toString());

        QDoubleValidator* validator = new QDoubleValidator(0.0, 1.0, 3, lineEdit);
        validator->setLocale(QLocale::C);
        lineEdit->setValidator(validator);

        QObject::connect(lineEdit, &QLineEdit::textChanged, propertyObject, [property, propertyObject](const QString& newText)
        {
            propertyObject->setProperty(property.name(), newText.toDouble());
        });

        return lineEdit;
    });

    registerCreateWidgetForType(QMetaType::Bool, [](const QMetaProperty& property, QObject* propertyObject, QWidget* parent)
    {
        QCheckBox* checkBox = new QCheckBox(parent);
        checkBox->setCheckState(property.read(propertyObject).toBool() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
        checkBox->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

        QObject::connect(checkBox, &QCheckBox::checkStateChanged, propertyObject, [property, propertyObject](Qt::CheckState newCheckState)
        {
            propertyObject->setProperty(property.name(), newCheckState == Qt::CheckState::Checked);
        });

        return checkBox;
    });

    registerCreateWidgetForType(QMetaType::QString, [](const QMetaProperty& property, QObject* propertyObject, QWidget* parent)
    {
        QLineEdit* lineEdit = new QLineEdit(parent);
        lineEdit->setText(property.read(propertyObject).toString());

        QObject::connect(lineEdit, &QLineEdit::textChanged, propertyObject, [property, propertyObject](const QString& newText)
        {
            propertyObject->setProperty(property.name(), newText);
        });

        return lineEdit;
    });
}
