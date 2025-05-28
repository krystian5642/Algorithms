#ifndef RANDOM_GRAPH_PROPERTIES_DIALOG_H
#define RANDOM_GRAPH_PROPERTIES_DIALOG_H

#include <QComboBox>
#include <QDialog>

class DataStructureVisualBuilder;

class RandomGraphPropertiesDialog : public QDialog
{
    Q_OBJECT
public:
    RandomGraphPropertiesDialog();

    DataStructureVisualBuilder* getSelectedBuidler() const;
    void setGraphBuilders(const QList<DataStructureVisualBuilder*>& graphBuilders);

private:
    void setupUi();

    QComboBox* graphTypesBox;
};

#endif // RANDOM_GRAPH_PROPERTIES_DIALOG_H
