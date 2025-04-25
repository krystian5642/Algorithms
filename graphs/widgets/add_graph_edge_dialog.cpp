#include "add_graph_edge_dialog.h"

#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

AddGraphEdgeDialog::AddGraphEdgeDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
}

AddGraphEdgeDialog::~AddGraphEdgeDialog()
{

}

int AddGraphEdgeDialog::getStartValue(bool* ok) const
{
    return lineEditStart->text().toInt(ok);
}

int AddGraphEdgeDialog::getEndValue(bool* ok) const
{
    return lineEditEnd->text().toInt(ok);
}

int AddGraphEdgeDialog::getEdgeWeight(bool* ok) const
{
    return lineEditWeight->text().toInt(ok);
}

void AddGraphEdgeDialog::setupUi()
{
    setFixedSize(250, 250);
    setWindowTitle("Add Graph Edge");

    QWidget* layoutWidget = new QWidget(this);

    constexpr int width = 170;
    constexpr int height = 170;

    const int X = geometry().size().width() / 2 - width / 2;
    const int Y = geometry().size().height() / 2 - height / 2;
    layoutWidget->setGeometry(QRect(X, Y, width, height));

    constexpr int lineEditWidth = 100;

    QVBoxLayout* verticalLayout = new QVBoxLayout(layoutWidget);

    QHBoxLayout* horizontalLayout1 = new QHBoxLayout(layoutWidget);
    QLabel* labelStart = new QLabel("Start : ");
    lineEditStart = new QLineEdit(layoutWidget);
    lineEditStart->setFixedWidth(lineEditWidth);
    lineEditStart->setValidator(new QIntValidator(lineEditStart));
    horizontalLayout1->addWidget(labelStart);
    horizontalLayout1->addWidget(lineEditStart);

    verticalLayout->addLayout(horizontalLayout1);

    QHBoxLayout* horizontalLayout2 = new QHBoxLayout(layoutWidget);
    QLabel* labelEnd = new QLabel("End : ");
    lineEditEnd = new QLineEdit(layoutWidget);
    lineEditEnd->setFixedWidth(lineEditWidth);
    lineEditEnd->setValidator(new QIntValidator(lineEditStart));
    horizontalLayout2->addWidget(labelEnd);
    horizontalLayout2->addWidget(lineEditEnd);

    verticalLayout->addLayout(horizontalLayout2);

    QHBoxLayout* horizontalLayout3 = new QHBoxLayout(layoutWidget);
    QLabel* labelWeight = new QLabel("Weight : ");
    lineEditWeight = new QLineEdit(layoutWidget);
    lineEditWeight->setFixedWidth(lineEditWidth);
    lineEditWeight->setValidator(new QIntValidator(lineEditStart));
    horizontalLayout3->addWidget(labelWeight);
    horizontalLayout3->addWidget(lineEditWeight);

    verticalLayout->addLayout(horizontalLayout3);

    QPushButton* acceptButton = new QPushButton("Add Edge");
    verticalLayout->addWidget(acceptButton);
    connect(acceptButton, &QPushButton::clicked, this, [this] ()
    {
        accept();
    });
}
