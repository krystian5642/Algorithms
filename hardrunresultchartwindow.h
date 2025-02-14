#ifndef HARDRUNRESULTCHARTWINDOW_H
#define HARDRUNRESULTCHARTWINDOW_H

#include <QMainWindow>

class QLineSeries;
class QChart;
class QChartView;

namespace Ui {
class HardRunResultChartWindow;
}

class HardRunResultChartWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HardRunResultChartWindow(QWidget *parent = nullptr);
    ~HardRunResultChartWindow();

    void setChartTitle(const QString& title);
    void setResult(const QList<QPointF>& result);

private slots:
    void on_actionSave_result_to_file_triggered();
    void on_actionLoad_result_from_file_triggered();

private:
    void setupUi();

    Ui::HardRunResultChartWindow *ui;
    QLineSeries *series;
    QChart *chart;
    QChartView *chartView;

    void saveChartToFile();
    void loadChartFromFile();
};

#endif // HARDRUNRESULTCHARTWINDOW_H
