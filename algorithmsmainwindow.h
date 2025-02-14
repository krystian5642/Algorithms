#ifndef AlgorithmsMAINWINDOW_H
#define AlgorithmsMAINWINDOW_H

#include <QMainWindow>

class GraphWidget;
class GraphAlgorithm;
class QComboBox;

class AlgorithmBenchmarkResultWindow;

namespace Ui
{
class AlgorithmsMainWindow;
}

template <class ValueType>
class Graph;

template <class ValueType>
class GraphEdge;

class AlgorithmsMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AlgorithmsMainWindow(QWidget *parent = nullptr);
    ~AlgorithmsMainWindow();

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void on_actionSave_triggered();
    void on_actionLoad_triggered();
    void on_actionClear_triggered();
    void on_actionRun_Algorithm_triggered(bool isOn);
    void on_actionGenerateRandomGraph_triggered();
    void on_actionGenerateRandomGridGraph_triggered();
    void on_actionAlgorithm_benchmark_triggered();

private:
    bool saveGraph();
    bool loadGraph();

    bool saveGraphNodeLocations();
    bool loadGraphNodeLocations();

    void setupUi();

    Ui::AlgorithmsMainWindow *ui;
    QComboBox* algorithmComboBox;
    GraphWidget* graphWidget;

private:
    GraphAlgorithm* getAlgorithmToExecute(const Graph<int>& graph) const;
    void clearAlgorithm();

    GraphAlgorithm* algorithm;
    int lastAlgorithmExecutionTime;

    // algorithm benchmark
    AlgorithmBenchmarkResultWindow* algorithmBenchmarkResultWindow;
};




#endif // AlgorithmsMAINWINDOW_H
