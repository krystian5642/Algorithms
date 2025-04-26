#include "main_window.h"

#include "algorithm_visualization_window.h"
#include "algorithm_benchmark_window.h"

#include <QCoreApplication>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    warmUp();
}

MainWindow::~MainWindow()
{

}

void MainWindow::setupUi()
{
    resize(1600, 900);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* horizontalLayout = new QHBoxLayout(centralWidget);
    QTabWidget* tabWidget = new QTabWidget(this);

    AlgorithmVisualizationWindow* visualizationWindow = new AlgorithmVisualizationWindow;
    tabWidget->addTab(visualizationWindow, "Visualization");

    AlgorithmBenchmarkWindow* banchmarkWindow = new AlgorithmBenchmarkWindow;
    tabWidget->addTab(banchmarkWindow, "Benchmark");

    tabWidget->setCurrentWidget(banchmarkWindow);

    horizontalLayout->addWidget(tabWidget);
}

void MainWindow::warmUp()
{
    constexpr int max = 10000;

    QList<int> list;
    for(int i = 0; i < max; ++i) list.append(i);
    for(int i = 0; i < max; ++i) list.contains(i);

    QSet<int> set;
    for(int i = 0; i < max; ++i) set.insert(i);
    for(int i = 0; i < max; ++i) set.contains(i);

    QQueue<int> queue;
    for(int i = 0; i < max; ++i) queue.enqueue(i);
    for(int i = 0; i < max; ++i) queue.contains(i);

    QHash<int, int> hash;
    for(int i = 0; i < max; ++i) hash.emplace(i, i);
    for(int i = 0; i < max; ++i) hash.contains(i);

    for(int i = 0; i < 10; ++i) QCoreApplication::processEvents();
}
