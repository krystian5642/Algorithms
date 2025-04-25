#include "random_graph_properties_dialog.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

RandomGraphPropertiesDialog::RandomGraphPropertiesDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
}

RandomGraphPropertiesDialog::~RandomGraphPropertiesDialog()
{

}

QString RandomGraphPropertiesDialog::getSelectedGraphTypeName() const
{
    return graphTypesBox->currentText();
}

void RandomGraphPropertiesDialog::setupUi()
{
    setWindowTitle("Random Graph");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QHBoxLayout* layout = new QHBoxLayout;
    mainLayout->addLayout(layout);

    QLabel* label = new QLabel(this);
    label->setText("Graph type : ");

    graphTypesBox = new QComboBox(this);
    graphTypesBox->addItem(GraphTypeNames::AllRandom);
    graphTypesBox->addItem(GraphTypeNames::Grid);

    layout->addWidget(label);
    layout->addWidget(graphTypesBox);

    QPushButton* okButton = new QPushButton("ok", this);
    mainLayout->addWidget(okButton);

    connect(okButton, &QPushButton::clicked, this, [this]()
    {
        accept();
    });
}
