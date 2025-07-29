#ifndef GRAPH_WIDGET_H
#define GRAPH_WIDGET_H

#include "../../core/widgets/data_structure_widget.h"
#include "../graph.h"

#include <QComboBox>
#include <QDialog>

class GraphVisualizationSettings;
class GraphNodePropertiesWidget;
class GraphAlgorithmVisualizer;
class Edge;

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
    QColor getNodeColor(int value) const;
    void setEdgeColor(int start, int end, const QColor& color, bool callUpdate = true);
    QColor getEdgeColor(int start, int end) const;

    void setNodesAndEdgesToBlack();

    void generateRandomEdges(const double addEdgePropability, const int minWeight, const int maxWeight);

    bool addNode(const QPointF& location);
    void addEdge(int start, int end, int weight = 1);
    void removeEdge(int start, int end);

    // DataStructureWidget interface
    void saveAction() override;
    void loadAction() override;
    void clearAction() override;
    void generateRandomDataStructureAction() override;
    bool visualizeAlgorithmAction(AlgorithmVisualizer* algorithmVisualizer, bool pause) override;
    void registerAlgorithmVisualizers() override;

private slots:
    void onActionGenerateRandomEdgesTriggered();
    void onActionAddEdgeTriggered();
    void onActionRemoveEdgeTriggered();
    void onActionResetGraphColorTriggered();

    void onAlgorithmVisualizerFinished();

    void onGraphEdgeAdded();
    void onGraphEdgeRemoved();

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;

    QList<GraphNodeVisualData> graphNodeVisualData;
    QHash<Edge, GraphEdgeVisualData> graphEdgeVisualData;

private:
    void paintDataStructure(QPainter& painter) override;

    void paintEdges(QPainter& painter);
    void paintNodes(QPainter& painter);
    void paintNodeValues(QPainter& painter);
    void paintWeights(QPainter& painter);

    void setAllowDirectedGraph(bool allow);

    void clearVisualization();

    bool saveGraph();
    bool loadGraph();

    bool saveGraphNodeLocations();
    bool loadGraphNodeLocations();

    qreal nodeRadius;
    qreal arrowSize;
    qreal arrowAngle;

    GraphVisualizationSettings* graphVisualizationSettings;
    QWidget* graphPropertiesWidget;

    Graph* graph;

    bool allowDirectedGraph;
};

#endif // GRAPH_WIDGET_H
