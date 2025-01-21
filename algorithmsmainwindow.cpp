#include "algorithmsmainwindow.h"
#include "ui_algorithmsmainwindow.h"
#include "graphwidget.h"

#include <QFile>
#include <QJsonDocument>

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

void AlgorithmsMainWindow::on_actionSave_triggered()
{
    if(saveGraph())
    {
        saveGraphNodeLocations();
    }
}

void AlgorithmsMainWindow::on_actionLoad_triggered()
{
    if(loadGraph())
    {
        loadGraphNodeLocations();
        graphWidget->update();
    }
}

void AlgorithmsMainWindow::on_actionClear_triggered()
{
    graphWidget->clearGraph();
}

bool AlgorithmsMainWindow::saveGraph()
{
    QFile saveFile("graph.txt");
    if(!saveFile.open(QIODevice::WriteOnly))
    {
        return false;
    }

    const QJsonObject graphAsJsonObject = graphWidget->getGraph().toJsonObject();
    const QJsonDocument jsonDoc(graphAsJsonObject);

    saveFile.write(jsonDoc.toJson());
    saveFile.close();

    return true;
}

bool AlgorithmsMainWindow::loadGraph()
{
    QFile loadFile("graph.txt");
    if(!loadFile.open(QIODevice::ReadOnly))
    {
        return false;
    }

    const QByteArray jsonData = loadFile.readAll();
    loadFile.close();

    const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    graphWidget->getGraphMutable().fromJsonObject(jsonDoc.object());

    return true;
}

bool AlgorithmsMainWindow::saveGraphNodeLocations()
{
    QFile saveFile("graph_node_locations.txt");
    if(!saveFile.open(QIODevice::WriteOnly))
    {
        return false;
    }

    QJsonObject locationsAsJsonObject;

    const auto& graphNodeLocations = graphWidget->getGraphNodeLocations();
    for(auto it = graphNodeLocations.constBegin(); it != graphNodeLocations.constEnd(); it++)
    {
        QJsonObject locationAsJsonObject;
        locationAsJsonObject["x"] = it.value().x();
        locationAsJsonObject["y"] = it.value().y();

        locationsAsJsonObject[QString::number(it.key())] = locationAsJsonObject;
    }

    const QJsonDocument jsonDoc(locationsAsJsonObject);

    saveFile.write(jsonDoc.toJson());
    saveFile.close();

    return true;
}

bool AlgorithmsMainWindow::loadGraphNodeLocations()
{
    QFile loadFile("graph_node_locations.txt");
    if(!loadFile.open(QIODevice::ReadOnly))
    {
        return false;
    }

    const QByteArray jsonData = loadFile.readAll();
    loadFile.close();

    const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    const QJsonObject locationsAsJsonObject = jsonDoc.object();

    auto& graphNodeLocations = graphWidget->getGraphNodeLocationsMutable();
    graphNodeLocations.clear();
    for (auto it = locationsAsJsonObject.constBegin(); it != locationsAsJsonObject.constEnd(); ++it)
    {
        const QJsonObject jsonObject = it.value().toObject();
        const QPoint graphNodeLocation(jsonObject["x"].toInt(), jsonObject["y"].toInt());

        graphNodeLocations[it.key().toInt()] = graphNodeLocation;
    }

    return true;
}
