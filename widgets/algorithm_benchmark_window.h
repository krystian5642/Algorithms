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
    void onActionClearTriggered();
    void onActionRunBenchmarkTriggered(bool isOn);
    void onActionClearSpikesTriggered();

    void onAlgorithmFinished(const QList<QPointF>& result);

private:
    void setupUi();
    void setupActionsAndToolBar();

    void registerAlgorithms();

    void cancelAlgorithms(bool deleteAll);

    Algorithm* getSelectedAlgorithm() const;

    QTreeView* algorithmsTreeView;

    QLineSeries *defaultSeries;
    QChart *chart;
    QChartView *chartView;

    // Category to algorithms list
    QHash<QString, QList<Algorithm*>> algorithms;
};

#endif // ALGORITHM_BENCHMARK_WINDOW_H
