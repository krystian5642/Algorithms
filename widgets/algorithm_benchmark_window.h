#ifndef ALGORITHM_BENCHMARK_WINDOW_H
#define ALGORITHM_BENCHMARK_WINDOW_H

#include <QCheckBox>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QQueue>
#include <QTimer>

class QLabel;
class QLineSeries;
class QChart;
class QChartView;
class QVBoxLayout;
class QTreeView;
class Algorithm;
struct AlgorithmBenchmarkResult;

class AlgorithmBenchmarkWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit AlgorithmBenchmarkWindow(QWidget *parent = nullptr);
    ~AlgorithmBenchmarkWindow();

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void onActionSaveTriggered();
    void onActionLoadTriggered();
    void onActionClearTriggered();
    void onActionRunBenchmarkTriggered(bool isOn);
    void onActionClearSpikesTriggered();

#ifdef QT_DEBUG
    void onActionDebugRunTriggered();
#endif

    void onAlgorithmTreeItemClicked(const QModelIndex& index);

    void onAlgorithmStarted();
    void onAlgorithmFinished(const AlgorithmBenchmarkResult &resultData);

    void onLineSeriesPressed(const QPointF& point);

    void onPlotAreaChanged(const QRectF &plotArea);

    void onLegendMarkerClicked();
    void onLegendMarkerHovered(bool state);

private:
    void setupUi();
    void setupActionsAndToolBar();

    void registerAlgorithms();

    void endBenchmarkRun(bool deleteAllAlgorithms = false);

    Algorithm* getSelectedAlgorithm() const;

    void saveSeriesToFile();
    void loadSeriesFromFile();

    void clearSeriesSelection();
    void addSeries(QLineSeries* series);

    QTreeView* algorithmsTreeView;

    QChart *chart;
    QChartView *chartView;

    QVBoxLayout* verticalLayout;

    QHash<QLineSeries*, AlgorithmBenchmarkResult> seriesToResultData;
    QHash<QLineSeries*, QLabel*> seriesToLabel;

    // Actions
    QAction* actionSave;
    QAction* actionLoad;
    QAction* actionClear;
    QAction* actionRunBenchmark;
    QAction* actionClearSpikes;

#ifdef QT_DEBUG
    QAction* debugRun;
#endif

    // Category to algorithms list
    QHash<QString, QList<Algorithm*>> algorithms;
};

#endif // ALGORITHM_BENCHMARK_WINDOW_H
