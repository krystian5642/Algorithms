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
    GraphNodeVisualData() = default;
    GraphNodeVisualData(const QPoint& inLocation)
        : location(inLocation)
    {
    }

    QPoint location;
};

class GraphWidget : public QWidget
{
    Q_OBJECT

public:
    GraphWidget(QWidget *parent = nullptr);
    ~GraphWidget();

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

    QHash<int, GraphNodeVisualData> graphVisualData;

    void addNode(int value, const QPoint& location);
    void addEdge(int startValue, int endValue, float weight = 0.f);

private:
    void paintGraph(QPainter& painter);

    Ui::GraphWidget *ui;
    Graph<int> graph;
};

#endif // GRAPHWIDGET_H
