#include "algorithm_visualization_window.h"

#include "../core/widgets/data_structure_widget.h"
#include "../core/algorithm_visualizer.h"
#include "../graphs/widgets/graph_widget.h"

#include <QStandardItemModel>
#include <QToolBar>
#include <QTreeView>
#include <QVBoxLayout>

AlgorithmVisualizationWindow::AlgorithmVisualizationWindow(QWidget *parent)
    : QMainWindow{parent}
{
    registerDataStructureWidgets();
    setupUi();
}

AlgorithmVisualizationWindow::~AlgorithmVisualizationWindow()
{

}

void AlgorithmVisualizationWindow::onActionSaveTriggered()
{
    DataStructureWidget* dataStructureWidget = getDataStructureWidget();
    if(dataStructureWidget)
    {
        dataStructureWidget->saveAction();
    }
}

void AlgorithmVisualizationWindow::onActionLoadTriggered()
{
    DataStructureWidget* dataStructureWidget = getDataStructureWidget();
    if(dataStructureWidget)
    {
        dataStructureWidget->loadAction();
    }
}

void AlgorithmVisualizationWindow::onActionClearTriggered()
{
    DataStructureWidget* dataStructureWidget = getDataStructureWidget();
    if(dataStructureWidget)
    {
        dataStructureWidget->clearAction();
    }
}

void AlgorithmVisualizationWindow::onActionGenerateRandomDataStructureTriggered()
{
    DataStructureWidget* dataStructureWidget = getDataStructureWidget();
    if(dataStructureWidget)
    {
        dataStructureWidget->generateRandomDataStructureAction();
    }
}

void AlgorithmVisualizationWindow::onActionRunAlgorithmTriggered(bool isOn)
{
    DataStructureWidget* dataStructureWidget = getDataStructureWidget();
    if(dataStructureWidget)
    {
        dataStructureWidget->visualizeAlgorithmAction(getSelectedAlgorithmVisualizer(), !isOn);
    }
}

void AlgorithmVisualizationWindow::onAlgorithmVisualizerTreeItemClicked(const QModelIndex &index)
{
    auto deleteFirstLayoutItem = [](QBoxLayout* layout, bool deleteWidget)
    {
        QLayoutItem* layoutItem = layout->takeAt(1);
        if(layoutItem)
        {
            if(deleteWidget)
            {
                QWidget* widget = layoutItem->widget();
                if(widget)
                {
                    widget->deleteLater();
                }
            }
            delete layoutItem;
        }
    };

    deleteFirstLayoutItem(verticalLayout, true);
    deleteFirstLayoutItem(horizontalLayout, false);

    AlgorithmVisualizer* algorithmVisualizer = qvariant_cast<AlgorithmVisualizer*>(index.data(Qt::UserRole));
    if(algorithmVisualizer)
    {
        QWidget* propertyWidget = algorithmVisualizer->createPropertiesWidget();
        verticalLayout->addWidget(propertyWidget, 1);

        horizontalLayout->addWidget(qobject_cast<QWidget*>(algorithmVisualizer->parent()), 1);
    }
    else
    {
        verticalLayout->addStretch(1);
        horizontalLayout->addStretch(1);
    }
}

void AlgorithmVisualizationWindow::setupUi()
{
    setupActionsAndToolBar();

    setIconSize(QSize(35, 35));
    setWindowTitle("Algorithm Visualization Window");

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    algorithmsTreeView = new QTreeView(centralWidget);
    algorithmsTreeView->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    connect(algorithmsTreeView, &QTreeView::clicked, this, &AlgorithmVisualizationWindow::onAlgorithmVisualizerTreeItemClicked);

    QWidget* layoutHolder = new QWidget(this);
    layoutHolder->setFixedWidth(240);

    verticalLayout = new QVBoxLayout(layoutHolder);
    verticalLayout->addWidget(algorithmsTreeView, 1);
    verticalLayout->addStretch(1);

    horizontalLayout = new QHBoxLayout(centralWidget);
    horizontalLayout->addWidget(layoutHolder);
    horizontalLayout->addStretch(1);

    QStandardItemModel* model = new QStandardItemModel;
    model->setHorizontalHeaderLabels({"All algorithms"});

    for(auto* widget : dataStructureWidgets)
    {
        QStandardItem* categoryItem = new QStandardItem(widget->getCategory());

        const QList<AlgorithmVisualizer*>& algorithmVisualizers = widget->getAlgorithmVisualizers();
        for(auto* algorithmVisualizer : algorithmVisualizers)
        {
            QStandardItem* algorithmItem = new QStandardItem(algorithmVisualizer->objectName());
            algorithmItem->setData(QVariant::fromValue(algorithmVisualizer), Qt::UserRole);
            categoryItem->appendRow(algorithmItem);
        }

        model->appendRow(categoryItem);
    }

    algorithmsTreeView->setModel(model);
    algorithmsTreeView->expandAll();
}

void AlgorithmVisualizationWindow::setupActionsAndToolBar()
{
    QAction* actionSave = new QAction(this);
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/icons/save.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    actionSave->setIcon(icon);
    connect(actionSave, &QAction::triggered, this, &AlgorithmVisualizationWindow::onActionSaveTriggered);

    QAction* actionLoad = new QAction(this);
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/icons/load.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    actionLoad->setIcon(icon1);
    connect(actionLoad, &QAction::triggered, this, &AlgorithmVisualizationWindow::onActionLoadTriggered);

    QAction* actionClear = new QAction(this);
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/icons/clear.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    actionClear->setIcon(icon2);
    connect(actionClear, &QAction::triggered, this, &AlgorithmVisualizationWindow::onActionClearTriggered);

    QAction* actionGenerateRandomStructure = new QAction(this);
    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/icons/random.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    actionGenerateRandomStructure->setIcon(icon3);
    actionGenerateRandomStructure->setMenuRole(QAction::MenuRole::NoRole);
    connect(actionGenerateRandomStructure, &QAction::triggered, this, &AlgorithmVisualizationWindow::onActionGenerateRandomDataStructureTriggered);

    QAction* actionRunAlgorithm = new QAction(this);
    actionRunAlgorithm->setCheckable(true);
    QIcon icon4;
    icon4.addFile(QString::fromUtf8(":/icons/play.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    icon4.addFile(QString::fromUtf8(":/icons/pause.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
    actionRunAlgorithm->setIcon(icon4);
    connect(actionRunAlgorithm, &QAction::triggered, this, &AlgorithmVisualizationWindow::onActionRunAlgorithmTriggered);

#if QT_CONFIG(tooltip)
    actionSave->setToolTip("Save to file");
    actionLoad->setToolTip("Load from file");
    actionClear->setToolTip("Clear");
    actionGenerateRandomStructure->setToolTip("Generate random structure");
    actionRunAlgorithm->setToolTip("Run algorithm");
#endif

    // tool bar
    QToolBar* toolBar = new QToolBar(this);
    toolBar->setMovable(false);
    toolBar->setFloatable(false);
    addToolBar(Qt::ToolBarArea::TopToolBarArea, toolBar);

    toolBar->addAction(actionSave);
    toolBar->addAction(actionLoad);
    toolBar->addAction(actionClear);
    toolBar->addSeparator();
    toolBar->addAction(actionGenerateRandomStructure);
    toolBar->addAction(actionRunAlgorithm);
}

void AlgorithmVisualizationWindow::registerDataStructureWidgets()
{
    dataStructureWidgets.append(new GraphWidget(this));

    for(auto* widget : dataStructureWidgets)
    {
        widget->registerAlgorithmVisualizers();
    }
}

DataStructureWidget *AlgorithmVisualizationWindow::getDataStructureWidget() const
{
    QLayoutItem* layoutItem = horizontalLayout->itemAt(1);
    return qobject_cast<DataStructureWidget*>(layoutItem->widget());
}

AlgorithmVisualizer* AlgorithmVisualizationWindow::getSelectedAlgorithmVisualizer() const
{
    const QModelIndex modelIndex = algorithmsTreeView->selectionModel()->currentIndex();
    return modelIndex.isValid() ? qvariant_cast<AlgorithmVisualizer*>(modelIndex.data(Qt::UserRole)) : nullptr;
}
