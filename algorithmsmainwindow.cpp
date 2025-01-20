#include "algorithmsmainwindow.h"
#include "ui_algorithmsmainwindow.h"
#include "graphwidget.h"

AlgorithmsMainWindow::AlgorithmsMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AlgorithmsMainWindow)
{
    ui->setupUi(this);

    graphWidget = new GraphWidget(this);
    setCentralWidget(graphWidget);
}

AlgorithmsMainWindow::~AlgorithmsMainWindow()
{
    delete ui;
}
