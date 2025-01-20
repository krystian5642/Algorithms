#ifndef ADDGRAPHEDGEDIALOG_H
#define ADDGRAPHEDGEDIALOG_H

#include "ui_addgraphedgedialog.h"

#include <QDialog>
#include <sstream>

class AddGraphEdgeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddGraphEdgeDialog(QWidget *parent = nullptr);
    ~AddGraphEdgeDialog();

    template<class ValueType>
    ValueType getStartValue(bool* ok = nullptr) const;

    template<class ValueType>
    ValueType getEndValue(bool* ok = nullptr) const;

    float getEdgeWeight(bool* ok = nullptr) const;

private slots:
    void on_addEdgeButton_clicked();

private:
    void adjustWidgetPositionToCenter();

    Ui::AddGraphEdgeDialog *ui;
};

template<class ValueType>
ValueType AddGraphEdgeDialog::getStartValue(bool* ok) const
{
    std::istringstream stream(ui->startLineEdit->text().toStdString());
    ValueType value;
    stream >> value;

    if(ok)
    {
        *ok = !stream.fail() && stream.eof();
    }

    return value;
}

template<class ValueType>
ValueType AddGraphEdgeDialog::getEndValue(bool* ok) const
{
    std::istringstream stream(ui->endLineEdit->text().toStdString());
    ValueType value;
    stream >> value;

    if(ok)
    {
        *ok = !stream.fail() && stream.eof();
    }

    return value;
}

#endif // ADDGRAPHEDGEDIALOG_H
