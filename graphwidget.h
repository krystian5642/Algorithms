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

struct GraphEdgeVisualData
{
    QColor color = Qt::black;
};

struct ScaleData
{
    qreal currentScale = 1.0;
    qreal minScale = 0.1;
    qreal maxScale = 15.0;
    qreal scaleMultiplier = 1.1;
};

struct RuntimeMouseDragData
{
    bool isDragging = false;
    QPointF dragDelta;
    QPointF lastMousePos;
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

    QJsonObject toJsonObject() const;
    void fromJsonObject(const QJsonObject& jsonObj);

    void setNodeColor(int value, const QColor& color, bool callUpdate = true);
    void setEdgeColor(int start, int end, const QColor& color, bool callUpdate = true);

    bool addNode(int value, const QPoint& location);
    void addEdge(int startValue, int endValue, float weight = 0.f);

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    QHash<int, GraphNodeVisualData> graphNodeVisualData;
    QHash<QPair<int, int>, GraphEdgeVisualData> graphEdgeVisualData;

private:
    void paintGraph(QPainter& painter);

    void paintEdges(QPainter& painter);
    void paintNodes(QPainter& painter);
    void paintNodeValues(QPainter& painter);

    Ui::GraphWidget *ui;
    Graph<int> graph;

    RuntimeMouseDragData dragData;

    ScaleData scaleData;

    QPointF scaleOffset;
};

#endif // GRAPHWIDGET_H
