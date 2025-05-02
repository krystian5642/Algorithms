#ifndef ALGORITHM_BENCHMARK_WINDOW_H
#define ALGORITHM_BENCHMARK_WINDOW_H

#include <QCheckBox>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QQueue>
#include <QTimer>

class QLineSeries;
class QChart;
class QChartView;
class QVBoxLayout;
class QTreeView;
class Algorithm;

class AlgorithmBenchmarkWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit AlgorithmBenchmarkWindow(QWidget *parent = nullptr);
    ~AlgorithmBenchmarkWindow();

private slots:
    void onActionSaveTriggered();
    void onActionLoadTriggered();
    void onActionClearTriggered();
    void onActionRunBenchmarkTriggered(bool isOn);
    void onActionClearSpikesTriggered();

    void onAlgorithmStarted();
    void onAlgorithmFinished(const QList<QPointF>& result);

private:
    void setupUi();
    void setupActionsAndToolBar();

    void registerAlgorithms();

    void endBenchmarkRun(bool deleteAllAlgorithms = false);

    Algorithm* getSelectedAlgorithm() const;

    void saveSeriesToFile();
    void loadSeriesFromFile();

    QTreeView* algorithmsTreeView;

    QLineSeries *defaultSeries;
    QChart *chart;
    QChartView *chartView;

    // Actions
    QAction* actionSave;
    QAction* actionLoad;
    QAction* actionClear;
    QAction* actionRunBenchmark;
    QAction* actionClearSpikes;

    // Category to algorithms list
    QHash<QString, QList<Algorithm*>> algorithms;
};

#endif // ALGORITHM_BENCHMARK_WINDOW_H
