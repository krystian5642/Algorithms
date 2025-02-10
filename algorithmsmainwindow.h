#ifndef ALGORITHMSMAINWINDOW_H
#define ALGORITHMSMAINWINDOW_H

#include <QMainWindow>

class GraphWidget;

namespace Ui
{
class AlgorithmsMainWindow;
}

template <class ValueType>
class GraphEdge;

class AlgorithmsMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AlgorithmsMainWindow(QWidget *parent = nullptr);
    ~AlgorithmsMainWindow();

private slots:
    void on_actionSave_triggered();
    void on_actionLoad_triggered();
    void on_actionClear_triggered();
    void on_actionRun_algorithm_triggered();
    void on_actionGenerateRandomGraph_triggered();

private:
    bool saveGraph();
    bool loadGraph();

    bool saveGraphNodeLocations();
    bool loadGraphNodeLocations();

    void setupUi();

    Ui::AlgorithmsMainWindow *ui;
    GraphWidget* graphWidget;

    struct ProcessedGraphAlgorithResult
    {
        using ConstEdgeIterator = QList<QPair<int, int>>::const_iterator;

        ConstEdgeIterator edgeIt;
    };

    ProcessedGraphAlgorithResult processedGraphAlgorithResult;
};




#endif // ALGORITHMSMAINWINDOW_H
