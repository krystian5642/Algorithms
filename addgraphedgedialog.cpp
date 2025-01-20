#include "addgraphedgedialog.h"

AddGraphEdgeDialog::AddGraphEdgeDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddGraphEdgeDialog)
{
    ui->setupUi(this);

    adjustWidgetPositionToCenter();
}

AddGraphEdgeDialog::~AddGraphEdgeDialog()
{
    delete ui;
}

void AddGraphEdgeDialog::adjustWidgetPositionToCenter()
{
    const auto& widgetGeometry = ui->layoutWidget->geometry();
    const int newX = geometry().size().width() / 2 - widgetGeometry.size().width() / 2;
    const int newY = geometry().size().height() / 2 - widgetGeometry.size().height() / 2;

    ui->layoutWidget->setGeometry(QRect(newX, newY, widgetGeometry.size().width(), widgetGeometry.size().height()));
}

void AddGraphEdgeDialog::on_addEdgeButton_clicked()
{
    accept();
}

float AddGraphEdgeDialog::getEdgeWeight(bool* ok) const
{
    return ui->weightLineEdit->text().toFloat(ok);
}


