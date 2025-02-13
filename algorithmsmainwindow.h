#ifndef AlgorithmsMAINWINDOW_H
#define AlgorithmsMAINWINDOW_H

#include <QMainWindow>

class GraphWidget;
class GraphAlgorithm;
class QComboBox;

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
    void on_actionRun_Algorithm_triggered();
    void on_actionGenerateRandomGraph_triggered();
    void on_actionGenerateRandomGridGraph_triggered();

private:
    bool saveGraph();
    bool loadGraph();

    bool saveGraphNodeLocations();
    bool loadGraphNodeLocations();

    void setupUi();

    Ui::AlgorithmsMainWindow *ui;
    QComboBox* AlgorithmComboBox;
    GraphWidget* graphWidget;

private:
    GraphAlgorithm* getAlgorithmToExecute() const;
    void clearAlgorithm();

    GraphAlgorithm* algorithm;
};




#endif // AlgorithmsMAINWINDOW_H
