#include "remove_edge_dialog.h"

#include "../graph.h"

#include <QFormLayout>
#include <QIntValidator>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

RemoveEdgeDialog::RemoveEdgeDialog(const Graph *graph)
{
    setupUi(graph);
}

int RemoveEdgeDialog::getStart() const
{
    return lineEditStart->text().toInt();
}

int RemoveEdgeDialog::getEnd() const
{
    return lineEditEnd->text().toInt();
}

void RemoveEdgeDialog::setupUi(const Graph *graph)
{
    setWindowTitle("Remove Edge");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QFormLayout* formLayout = new QFormLayout;

    const QIntValidator* removeEdgeValidator = new QIntValidator(0, graph->getVerticesNum() - 1, formLayout);

    lineEditStart = new QLineEdit("0", this);
    lineEditStart->setValidator(removeEdgeValidator);

    lineEditEnd = new QLineEdit("0", this);
    lineEditEnd->setValidator(removeEdgeValidator);

    formLayout->addRow("Start : ", lineEditStart);
    formLayout->addRow("End : ", lineEditEnd);

    mainLayout->addLayout(formLayout);

    QPushButton* okButton = new QPushButton("ok", this);
    mainLayout->addWidget(okButton);

    connect(okButton, &QPushButton::clicked, this, [this]()
    {
        accept();
    });
}
