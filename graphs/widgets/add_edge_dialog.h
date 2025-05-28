#ifndef ADD_EDGE_DIALOG_H
#define ADD_EDGE_DIALOG_H

#include <QDialog>

class Graph;
class QLineEdit;

class AddEdgeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddEdgeDialog(const Graph* graph);

    int getStart() const;
    int getEnd() const;
    int getWeight() const;

private:
    void setupUi(const Graph* graph);

    QLineEdit* lineEditStart;
    QLineEdit* lineEditEnd;
    QLineEdit* lineEditWeight;
};

#endif // ADD_EDGE_DIALOG_H
