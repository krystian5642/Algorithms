#include "add_edge_dialog.h"

#include <QFormLayout>
#include <QIntValidator>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "../graph.h"

AddEdgeDialog::AddEdgeDialog(const Graph* graph)
{
    setupUi(graph);
}

int AddEdgeDialog::getStart() const
{
    return lineEditStart->text().toInt();
}

int AddEdgeDialog::getEnd() const
{
    return lineEditEnd->text().toInt();
}

int AddEdgeDialog::getWeight() const
{
    return lineEditWeight->text().toInt();
}

void AddEdgeDialog::setupUi(const Graph* graph)
{
    setWindowTitle("Add Edge");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QFormLayout* formLayout = new QFormLayout;

    const QIntValidator* addEdgeValidator = new QIntValidator(0, graph->getNodesNum() - 1, formLayout);

    lineEditStart = new QLineEdit("0", this);
    lineEditStart->setValidator(addEdgeValidator);

    lineEditEnd = new QLineEdit("0", this);
    lineEditEnd->setValidator(addEdgeValidator);

    lineEditWeight = new QLineEdit("1", this);

    formLayout->addRow("Start : ", lineEditStart);
    formLayout->addRow("End : ", lineEditEnd);
    formLayout->addRow("Weight : ", lineEditWeight);

    mainLayout->addLayout(formLayout);

    QPushButton* okButton = new QPushButton("ok", this);
    mainLayout->addWidget(okButton);

    connect(okButton, &QPushButton::clicked, this, [this]()
    {
        accept();
    });
}
