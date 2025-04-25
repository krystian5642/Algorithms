#ifndef RANDOM_GRAPH_PROPERTIES_DIALOG_H
#define RANDOM_GRAPH_PROPERTIES_DIALOG_H

#include <QComboBox>
#include <QDialog>

namespace GraphTypeNames
{
    const QString AllRandom("All random");
    const QString Grid("Grid");
}

class RandomGraphPropertiesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RandomGraphPropertiesDialog(QWidget *parent = nullptr);
    ~RandomGraphPropertiesDialog();

    QString getSelectedGraphTypeName() const;

private:
    void setupUi();

    QComboBox* graphTypesBox;
};

#endif // RANDOM_GRAPH_PROPERTIES_DIALOG_H
