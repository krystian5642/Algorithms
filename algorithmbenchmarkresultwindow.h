#ifndef AlgorithmBenchmarkResultWindow_H
#define AlgorithmBenchmarkResultWindow_H

#include <QMainWindow>

class QLineSeries;
class QChart;
class QChartView;

namespace Ui {
class AlgorithmBenchmarkResultWindow;
}

class AlgorithmBenchmarkResultWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AlgorithmBenchmarkResultWindow(QWidget *parent = nullptr);
    ~AlgorithmBenchmarkResultWindow();

    void setChartTitle(const QString& title);
    void setResult(const QList<QPointF>& result);

private slots:
    void on_actionSave_result_to_file_triggered();
    void on_actionLoad_result_from_file_triggered();

private:
    void setupUi();

    Ui::AlgorithmBenchmarkResultWindow *ui;
    QLineSeries *defaultSeries;
    QChart *chart;
    QChartView *chartView;

    void saveResultToFile();
    void loadResultFromFile();
};

#endif // AlgorithmBenchmarkResultWindow_H
