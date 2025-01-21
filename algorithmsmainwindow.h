#ifndef ALGORITHMSMAINWINDOW_H
#define ALGORITHMSMAINWINDOW_H

#include <QMainWindow>

class GraphWidget;

namespace Ui
{
class AlgorithmsMainWindow;
}

class AlgorithmsMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AlgorithmsMainWindow(QWidget *parent = nullptr);
    ~AlgorithmsMainWindow();

private slots:
    void on_actionSave_triggered();
    void on_actionLoad_triggered();

private:
    Ui::AlgorithmsMainWindow *ui;
    GraphWidget* graphWidget;
};

#endif // ALGORITHMSMAINWINDOW_H
