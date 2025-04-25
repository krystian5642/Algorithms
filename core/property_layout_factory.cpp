#include "property_layout_factory.h"

#include <QCheckBox>
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
