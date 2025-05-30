#ifndef REMOVE_EDGE_DIALOG_H
#define REMOVE_EDGE_DIALOG_H

#include <QDialog>

class Graph;
class QLineEdit;

class RemoveEdgeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RemoveEdgeDialog(const Graph* graph);

    int getStart() const;
    int getEnd() const;

private:
    void setupUi(const Graph* graph);

    QLineEdit* lineEditStart;
    QLineEdit* lineEditEnd;
};

#endif // REMOVE_EDGE_DIALOG_H
