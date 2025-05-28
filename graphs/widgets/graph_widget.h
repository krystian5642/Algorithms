#ifndef GRAPH_WIDGET_H
#define GRAPH_WIDGET_H

#include "../../core/widgets/data_structure_widget.h"
#include "../graph.h"

#include <QComboBox>
#include <QDialog>

class GraphNodePropertiesWidget;
class GraphAlgorithmVisualizer;

struct GraphNodeVisualData
{
    QPointF location;
    QColor color = Qt::black;
};

struct GraphEdgeVisualData
{
    QColor color = Qt::black;
};

class GraphWidget : public DataStructureWidget
{
    Q_OBJECT

public:
    explicit GraphWidget(QWidget *parent = nullptr);
    ~GraphWidget();

    void clear();

    // graph serialization
    QJsonObject toJsonObject() const;
    void fromJsonObject(const QJsonObject& jsonObj);

    void setNodeColor(int value, const QColor& color, bool callUpdate = true);
    void setEdgeColor(int start, int end, const QColor& color, bool callUpdate = true);
    void setNodesAndEdgesToBlack();

    void generateRandomEdges(const double addEdgePropability);

    bool addNode(const QPoint& location);
    void addEdge(int start, int end, int weight = 1);

    // DataStructureWidget interface
    void saveAction() override;
    void loadAction() override;
    void clearAction() override;
    void generateRandomDataStructureAction() override;
    void visualizeAlgorithmAction(AlgorithmVisualizer* algorithmVisualizer, bool pause) override;
    void registerAlgorithmVisualizers() override;

private slots:
    void onActionGenerateRandomEdgesTriggered();
    void onActionAddEdgeTriggered();

    void onAlgorithmVisualizerFinished();

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;

    QList<GraphNodeVisualData> graphNodeVisualData;
    QHash<QPair<int, int>, GraphEdgeVisualData> graphEdgeVisualData;

private:
    void paintDataStructure(QPainter& painter) override;

    void paintEdges(QPainter& painter);
    void paintNodes(QPainter& painter);
    void paintNodeValues(QPainter& painter);

    void clearVisualization();

    bool saveGraph();
    bool loadGraph();

    bool saveGraphNodeLocations();
    bool loadGraphNodeLocations();

    qreal nodeRadius;
    qreal arrowSize;
    qreal arrowAngle;

    std::unique_ptr<Graph> graph;
};

#endif // GRAPH_WIDGET_H
