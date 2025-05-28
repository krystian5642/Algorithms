#include "random_graph_properties_dialog.h"

#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "../../core/data_structure_visual_builder.h"

RandomGraphPropertiesDialog::RandomGraphPropertiesDialog()
{
    setupUi();
}

DataStructureVisualBuilder *RandomGraphPropertiesDialog::getSelectedBuidler() const
{
    return qvariant_cast<DataStructureVisualBuilder*>(graphTypesBox->currentData(Qt::UserRole));
}

void RandomGraphPropertiesDialog::setGraphBuilders(const QList<DataStructureVisualBuilder*> &graphBuilders)
{
    for(const auto* graphBuilder : graphBuilders)
    {
        graphTypesBox->addItem(graphBuilder->objectName(), QVariant::fromValue(graphBuilder));
    }
}

void RandomGraphPropertiesDialog::setupUi()
{
    setWindowTitle("Random Graph");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QFormLayout* formLayout = new QFormLayout;

    graphTypesBox = new QComboBox(this);
    formLayout->addRow("Graph type : ", graphTypesBox);

    mainLayout->addLayout(formLayout);

    QPushButton* okButton = new QPushButton("ok", this);
    mainLayout->addWidget(okButton);

    connect(okButton, &QPushButton::clicked, this, [this]()
    {
        accept();
    });
}
