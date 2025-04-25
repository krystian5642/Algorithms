#ifndef ALGORITHM_VISUALIZATION_WINDOW_H
#define ALGORITHM_VISUALIZATION_WINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>

class QTreeView;
class AlgorithmVisualizer;
class DataStructureWidget;

class AlgorithmVisualizationWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit AlgorithmVisualizationWindow(QWidget *parent = nullptr);
    ~AlgorithmVisualizationWindow();

private slots:
    void onActionSaveTriggered();
    void onActionLoadTriggered();
    void onActionClearTriggered();
    void onActionGenerateRandomDataStructureTriggered();
    void onActionRunAlgorithmTriggered(bool isOn);

    void onAlgorithmVisualizerTreeItemClicked(const QModelIndex& index);

private:
    void setupUi();
    void setupActions();

    void registerDataStructureWidgets();

    DataStructureWidget* getDataStructureWidget() const;

    AlgorithmVisualizer* getSelectedAlgorithmVisualizer() const;

    QTreeView* algorithmsTreeView;
    QVBoxLayout* verticalLayout;
    QHBoxLayout* horizontalLayout;

    QList<DataStructureWidget*> dataStructureWidgets;
};

#endif // ALGORITHM_VISUALIZATION_WINDOW_H
