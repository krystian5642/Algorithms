#include "properties_dialog.h"

#include "../property_editor_factory.h"

#include <QPushButton>
#include <QVBoxLayout>

PropertiesDialog::PropertiesDialog(QObject* object)
{
    setupUi(object);
}

void PropertiesDialog::setupUi(QObject* object)
{
    resize(300, 200);

    setWindowTitle("Properties");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    layout->addWidget(PropertyEditorFactory::get().createPropertiesWidget(object));

    QPushButton* okButton = new QPushButton("ok", this);
    layout->addWidget(okButton);

    connect(okButton, &QPushButton::clicked, this, [this]()
    {
        accept();
    });
}
