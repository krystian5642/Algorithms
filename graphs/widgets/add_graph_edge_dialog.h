#ifndef ADD_GRAPH_EDGE_DIALOG_H
#define ADD_GRAPH_EDGE_DIALOG_H

#include <QDialog>
#include <QLineEdit>

class AddGraphEdgeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddGraphEdgeDialog(QWidget *parent = nullptr);
    ~AddGraphEdgeDialog();

    int getStartValue(bool* ok = nullptr) const;
    int getEndValue(bool* ok = nullptr) const;
    int getEdgeWeight(bool* ok = nullptr) const;

private:
    void setupUi();

    QLineEdit* lineEditStart;
    QLineEdit* lineEditEnd;
    QLineEdit* lineEditWeight;
};

#endif // ADD_GRAPH_EDGE_DIALOG_H
