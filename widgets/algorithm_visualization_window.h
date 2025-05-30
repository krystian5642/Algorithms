#ifndef ALGORITHM_VISUALIZATION_WINDOW_H
#define ALGORITHM_VISUALIZATION_WINDOW_H

#include <QHBoxLayout>
#include <QMainWindow>

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

    void onAlgorithmVisualizerFinished();

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void setupUi();
    void setupActionsAndToolBar();

    void removeActions(const QList<QAction*>& actions);

    void addWidgetsToToolBar(const QList<QWidget*>& widgets);

    void registerDataStructureWidgets();

    DataStructureWidget* getDataStructureWidget() const;

    AlgorithmVisualizer* getSelectedAlgorithmVisualizer() const;

    void showNothingSelectedInfo();

    QTreeView* algorithmsTreeView;
    QVBoxLayout* verticalLayout;
    QHBoxLayout* horizontalLayout;
    QToolBar* toolBar;

    // Actions
    QAction* actionSave;
    QAction* actionLoad;
    QAction* actionClear;
    QAction* actionGenerateRandomStructure;
    QAction* actionRunAlgorithm;

    QList<DataStructureWidget*> dataStructureWidgets;

    QList<QAction*> temporaryActions;
};

#endif // ALGORITHM_VISUALIZATION_WINDOW_H
