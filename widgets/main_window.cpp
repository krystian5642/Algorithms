#include "main_window.h"

#include "algorithm_visualization_window.h"

#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
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

    horizontalLayout->addWidget(tabWidget);
}
