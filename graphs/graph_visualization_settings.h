#ifndef GRAPH_VISUALIZATION_SETTINGS_H
#define GRAPH_VISUALIZATION_SETTINGS_H

#include <QObject>

class GraphVisualizationSettings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool showWeights READ getShowWeights WRITE setShowWeights NOTIFY showWeightsChanged FINAL)
    Q_PROPERTY(bool graphDirected READ getGraphDirected WRITE setGraphDirected NOTIFY graphDirectedChanged FINAL)
public:
    explicit GraphVisualizationSettings(QObject *parent);

    bool getShowWeights() const;
    void setShowWeights(bool newShowWeights);

    bool getGraphDirected() const;
    void setGraphDirected(bool newGraphDirected);

signals:
    void showWeightsChanged(bool showWeights);
    void graphDirectedChanged(bool graphDirected);

private:
    bool showWeights;
    bool graphDirected;
};

#endif // GRAPH_VISUALIZATION_SETTINGS_H
