#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include "graph.h"

class GraphNodePropertiesWidget;

QT_BEGIN_NAMESPACE
namespace Ui
{
class GraphWidget;
}
QT_END_NAMESPACE

struct GraphNodeVisualData
{
    QPoint location;
    QColor color = Qt::black;
};

class GraphWidget : public QWidget
{
    Q_OBJECT

public:
    GraphWidget(QWidget *parent = nullptr);
    ~GraphWidget();

    void clearGraph();

    const Graph<int>& getGraph() const { return graph; };
    Graph<int>& getGraphMutable() { return graph; };

    QHash<int, GraphNodeVisualData>& getGraphNodeVisualData() { return graphNodeVisualData; };

    //const QHash<int, QPoint>& getGraphNodeLocations() const { return graphNodeLocations; }
    //QHash<int, QPoint>& getGraphNodeLocationsMutable() { return graphNodeLocations; }

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

    QHash<int, GraphNodeVisualData> graphNodeVisualData;

    void addNode(int value, const QPoint& location);
    void addEdge(int startValue, int endValue, float weight = 0.f);
private:
    void paintGraph(QPainter& painter);

    Ui::GraphWidget *ui;
    Graph<int> graph;
};

#endif // GRAPHWIDGET_H
