#include "algorithm_visualization_window.h"

#include "../core/widgets/data_structure_widget.h"
#include "../core/algorithm_visualizer.h"
#include "../graphs/widgets/graph_widget.h"

#include <QMessageBox>
#include <QStandardItemModel>
#include <QToolBar>
#include <QTreeView>
#include <QVBoxLayout>
#include <QStatusBar>

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
    else
    {
        showNothingSelectedInfo();
    }
}

void AlgorithmVisualizationWindow::onActionLoadTriggered()
{
    DataStructureWidget* dataStructureWidget = getDataStructureWidget();
    if(dataStructureWidget)
    {
        dataStructureWidget->loadAction();
    }
    else
    {
        showNothingSelectedInfo();
    }
}

void AlgorithmVisualizationWindow::onActionClearTriggered()
{
    DataStructureWidget* dataStructureWidget = getDataStructureWidget();
    if(dataStructureWidget)
    {
        actionRunAlgorithm->setChecked(false);
        dataStructureWidget->clearAction();
    }
    else
    {
        showNothingSelectedInfo();
    }
}

void AlgorithmVisualizationWindow::onActionGenerateRandomDataStructureTriggered()
{
    DataStructureWidget* dataStructureWidget = getDataStructureWidget();
    if(dataStructureWidget)
    {
        dataStructureWidget->generateRandomDataStructureAction();
    }
    else
    {
        showNothingSelectedInfo();
    }
}

void AlgorithmVisualizationWindow::onActionRunAlgorithmTriggered(bool isOn)
{
    DataStructureWidget* dataStructureWidget = getDataStructureWidget();
    if (!dataStructureWidget)
    {
        actionRunAlgorithm->setChecked(false);
        showNothingSelectedInfo();
        return;
    }

    if(isOn)
    {
        AlgorithmVisualizer* algorithmVisualizer = getSelectedAlgorithmVisualizer();
        if(algorithmVisualizer)
        {
            if(!dataStructureWidget->visualizeAlgorithmAction(getSelectedAlgorithmVisualizer(), false))
            {
                onAlgorithmVisualizerFinished();
            }
        }
        else
        {
            actionRunAlgorithm->setChecked(false);

            QMessageBox::information(this, "Info", "No algorithm selected");
        }
    }
    else
    {
        actionRunAlgorithm->setChecked(false);
        dataStructureWidget->visualizeAlgorithmAction(nullptr, true);
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

    DataStructureWidget* newWidget = nullptr;

    AlgorithmVisualizer* algorithmVisualizer = qvariant_cast<AlgorithmVisualizer*>(index.data(Qt::UserRole));
    if(algorithmVisualizer)
    {
        QWidget* propertyWidget = algorithmVisualizer->createPropertiesWidget();
        verticalLayout->addWidget(propertyWidget, 1);

        newWidget = qobject_cast<DataStructureWidget*>(algorithmVisualizer->parent());
    }
    else
    {
        newWidget = qvariant_cast<DataStructureWidget*>(index.data(Qt::UserRole));
    }

    DataStructureWidget* previousWidget = getDataStructureWidget();

    if(newWidget != previousWidget)
    {
        if(previousWidget)
        {
            previousWidget->hide();

            removeActions(previousWidget->getAdditionalActions());

            removeActions(temporaryActions);
            temporaryActions.clear();
        }

        deleteFirstLayoutItem(horizontalLayout, false);

        if(newWidget)
        {
            newWidget->show();

            horizontalLayout->addWidget(newWidget, 1);

            toolBar->addActions(newWidget->getAdditionalActions());

            addWidgetsToToolBar(newWidget->getAdditionalToolBarWidget());
        }
    }
}

void AlgorithmVisualizationWindow::onAlgorithmVisualizerFinished()
{
    actionRunAlgorithm->setChecked(false);
}

void AlgorithmVisualizationWindow::mousePressEvent(QMouseEvent *event)
{
    const DataStructureWidget* dataStructureWidget = getDataStructureWidget();
    if(!dataStructureWidget)
    {
        showNothingSelectedInfo();
    }
}

void AlgorithmVisualizationWindow::paintEvent(QPaintEvent *event)
{
    const DataStructureWidget* dataStructureWidget = getDataStructureWidget();
    statusBar()->showMessage(dataStructureWidget ? QString("Last paint time : %1 ms").arg(dataStructureWidget->getLastPaintTime())
                                                     : QString());
}

void AlgorithmVisualizationWindow::setupUi()
{
    setupActionsAndToolBar();

    setIconSize(QSize(40, 40));
    setWindowTitle("Algorithm Visualization Window");

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    algorithmsTreeView = new QTreeView(centralWidget);
    algorithmsTreeView->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    connect(algorithmsTreeView, &QTreeView::clicked, this, &AlgorithmVisualizationWindow::onAlgorithmVisualizerTreeItemClicked);

    QWidget* layoutHolder = new QWidget(this);
    layoutHolder->setFixedWidth(260);

    verticalLayout = new QVBoxLayout(layoutHolder);
    verticalLayout->addWidget(algorithmsTreeView, 1);

    horizontalLayout = new QHBoxLayout(centralWidget);
    horizontalLayout->addWidget(layoutHolder);
    horizontalLayout->addStretch(1);

    QStandardItemModel* model = new QStandardItemModel;
    model->setHorizontalHeaderLabels({"All algorithms"});

    for(auto* widget : dataStructureWidgets)
    {
        QStandardItem* categoryItem = new QStandardItem(widget->getCategory());
        categoryItem->setData(QVariant::fromValue(widget), Qt::UserRole);

        const QList<AlgorithmVisualizer*>& algorithmVisualizers = widget->getAlgorithmVisualizers();
        for(auto* algorithmVisualizer : algorithmVisualizers)
        {
            QStandardItem* algorithmItem = new QStandardItem(algorithmVisualizer->objectName());
            algorithmItem->setData(QVariant::fromValue(algorithmVisualizer), Qt::UserRole);
            categoryItem->appendRow(algorithmItem);

            connect(algorithmVisualizer, &AlgorithmVisualizer::finished, this, &AlgorithmVisualizationWindow::onAlgorithmVisualizerFinished);
        }

        model->appendRow(categoryItem);
    }

    algorithmsTreeView->setModel(model);
    algorithmsTreeView->expandAll();
}

void AlgorithmVisualizationWindow::setupActionsAndToolBar()
{
    actionSave = new QAction(this);
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/icons/save.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    actionSave->setIcon(icon);
    connect(actionSave, &QAction::triggered, this, &AlgorithmVisualizationWindow::onActionSaveTriggered);

    actionLoad = new QAction(this);
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/icons/load.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    actionLoad->setIcon(icon1);
    connect(actionLoad, &QAction::triggered, this, &AlgorithmVisualizationWindow::onActionLoadTriggered);

    actionClear = new QAction(this);
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/icons/clear.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    actionClear->setIcon(icon2);
    connect(actionClear, &QAction::triggered, this, &AlgorithmVisualizationWindow::onActionClearTriggered);

    actionGenerateRandomStructure = new QAction(this);
    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/icons/random.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    actionGenerateRandomStructure->setIcon(icon3);
    actionGenerateRandomStructure->setMenuRole(QAction::MenuRole::NoRole);
    connect(actionGenerateRandomStructure, &QAction::triggered, this, &AlgorithmVisualizationWindow::onActionGenerateRandomDataStructureTriggered);

    actionRunAlgorithm = new QAction(this);
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
    toolBar = new QToolBar(this);
    toolBar->setMovable(false);
    toolBar->setFloatable(false);
    addToolBar(Qt::ToolBarArea::TopToolBarArea, toolBar);

    toolBar->addAction(actionSave);
    toolBar->addAction(actionLoad);
    toolBar->addAction(actionClear);
    toolBar->addSeparator();
    toolBar->addAction(actionGenerateRandomStructure);
    toolBar->addAction(actionRunAlgorithm);

    QTimer::singleShot(0, this, [this]()
    {
        toolBar->setFixedHeight(70);
    });
}

void AlgorithmVisualizationWindow::removeActions(const QList<QAction *> &actions)
{
    for(auto* action : actions)
    {
        toolBar->removeAction(action);
    }
}

void AlgorithmVisualizationWindow::addWidgetsToToolBar(const QList<QWidget *> &widgets)
{
    temporaryActions.reserve(widgets.size());
    for(auto* widget : widgets)
    {
        temporaryActions.push_back(toolBar->addWidget(widget));
    }
}

void AlgorithmVisualizationWindow::registerDataStructureWidgets()
{
    dataStructureWidgets.append(new GraphWidget(this));

    for(auto* widget : dataStructureWidgets)
    {
        widget->hide();
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

void AlgorithmVisualizationWindow::showNothingSelectedInfo()
{
    QMessageBox::information(this, "Info", "No algorithm/data structure selected");
}
