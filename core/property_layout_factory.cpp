#include "property_layout_factory.h"

#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QMetaProperty>
#include <QWidget>

namespace PropertyLayoutFactoryUtils
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

PropertyLayoutFactory &PropertyLayoutFactory::get()
{
    static PropertyLayoutFactory factor;
    return factor;
}

void PropertyLayoutFactory::registerCreateLayoutForType(int type, CreateLayoutFunction func)
{
    layoutCreators[type] = func;
}

QBoxLayout *PropertyLayoutFactory::createLayoutForProperty(const QMetaProperty& property, QObject* propertyObject, QWidget* parent)
{
    auto it = layoutCreators.constFind(property.typeId());
    if(it != layoutCreators.constEnd())
    {
        return (it.value())(property, propertyObject, parent);
    }
    return nullptr;
}

QWidget *PropertyLayoutFactory::createPropertiesWidget(QObject *object, QWidget* parent, bool addStretch)
{
    if(!object)
    {
        return nullptr;
    }

    QWidget* propertiesWidget = new QWidget(parent);

    QVBoxLayout* propertyLayout = new QVBoxLayout;
    propertiesWidget->setLayout(propertyLayout);

    const QMetaObject* myMetaObject = object->metaObject();
    for (int i = 0; i < myMetaObject->propertyCount(); ++i)
    {
        const QMetaProperty metaProperty = myMetaObject->property(i);
        if(QString(metaProperty.name()) != "objectName")
        {
            propertyLayout->addLayout(createLayoutForProperty(metaProperty, object, propertiesWidget));
        }
    }

    if(addStretch)
    {
        propertyLayout->addStretch();
    }

    return propertiesWidget;
}

void PropertyLayoutFactory::addComboBox(QWidget *propertiesWidget, const QStringList &values, const QString& title, QString &changedValue)
{
    QBoxLayout* layout = qobject_cast<QBoxLayout*>(propertiesWidget->layout());

    QLabel* label = new QLabel(title, propertiesWidget);
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

    QHBoxLayout* horizontalLayout = new QHBoxLayout;

    horizontalLayout->addWidget(label);
    horizontalLayout->addWidget(comboBox);

    layout->addLayout(horizontalLayout);
}

PropertyLayoutFactory::PropertyLayoutFactory()
{
    constexpr int lineEditMaxWidth = 130;

    registerCreateLayoutForType(QMetaType::Int, [](const QMetaProperty& property, QObject* propertyObject, QWidget* parent)
    {
        QHBoxLayout* layout = new QHBoxLayout;

        QLabel* label = new QLabel(parent);
        label->setText(PropertyLayoutFactoryUtils::convertCamelCaseToSpaces(property.name()));

        QLineEdit* lineEdit = new QLineEdit(parent);
        lineEdit->setMaximumWidth(lineEditMaxWidth);
        lineEdit->setText(property.read(propertyObject).toString());
        lineEdit->setValidator(new QIntValidator(lineEdit));

        QObject::connect(lineEdit, &QLineEdit::textChanged, propertyObject, [property, propertyObject](const QString& newText)
        {
            propertyObject->setProperty(property.name(), newText.toInt());
        });

        layout->addWidget(label);
        layout->addWidget(lineEdit);

        return layout;
    });

    registerCreateLayoutForType(QMetaType::Double, [](const QMetaProperty& property, QObject* propertyObject, QWidget* parent)
    {
        QHBoxLayout* layout = new QHBoxLayout;

        QLabel* label = new QLabel(parent);
        label->setText(PropertyLayoutFactoryUtils::convertCamelCaseToSpaces(property.name()));

        QLineEdit* lineEdit = new QLineEdit(parent);
        lineEdit->setMaximumWidth(lineEditMaxWidth);
        lineEdit->setText(property.read(propertyObject).toString());
        lineEdit->setValidator(new QDoubleValidator(lineEdit));

        QObject::connect(lineEdit, &QLineEdit::textChanged, propertyObject, [property, propertyObject](const QString& newText)
        {
            propertyObject->setProperty(property.name(), newText.toDouble());
        });

        layout->addWidget(label);
        layout->addWidget(lineEdit);

        return layout;
    });

    registerCreateLayoutForType(QMetaType::Bool, [](const QMetaProperty& property, QObject* propertyObject, QWidget* parent)
    {
        QHBoxLayout* layout = new QHBoxLayout;

        QLabel* label = new QLabel(parent);
        label->setText(PropertyLayoutFactoryUtils::convertCamelCaseToSpaces(property.name()));

        QCheckBox* checkbox = new QCheckBox(parent);
        checkbox->setCheckState(property.read(propertyObject).toBool() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
        checkbox->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

        QObject::connect(checkbox, &QCheckBox::checkStateChanged, propertyObject, [property, propertyObject](Qt::CheckState newCheckState)
        {
            propertyObject->setProperty(property.name(), newCheckState == Qt::CheckState::Checked ? true : false);
        });

        layout->addWidget(label);
        layout->addWidget(checkbox);

        return layout;
    });

    registerCreateLayoutForType(QMetaType::QString, [](const QMetaProperty& property, QObject* propertyObject, QWidget* parent)
    {
        QHBoxLayout* layout = new QHBoxLayout;

        QLabel* label = new QLabel(parent);
        label->setText(PropertyLayoutFactoryUtils::convertCamelCaseToSpaces(property.name()));

        QLineEdit* lineEdit = new QLineEdit(parent);
        lineEdit->setMaximumWidth(lineEditMaxWidth);
        lineEdit->setText(property.read(propertyObject).toString());

        QObject::connect(lineEdit, &QLineEdit::textChanged, propertyObject, [property, propertyObject](const QString& newText)
        {
            propertyObject->setProperty(property.name(), newText);
        });

        layout->addWidget(label);
        layout->addWidget(lineEdit);

        return layout;
    });
}
