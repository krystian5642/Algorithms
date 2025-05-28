#ifndef GRAPH_VISUAL_BUILDERS_H
#define GRAPH_VISUAL_BUILDERS_H

#include "../core/data_structure_visual_builder.h"

class GraphVisualBuilder : public DataStructureVisualBuilder
{
    Q_OBJECT
public:
    explicit GraphVisualBuilder(QObject *parent = nullptr);
    virtual ~GraphVisualBuilder() = 0;
};

class GeneralGraphVisualBuilder : public GraphVisualBuilder
{
    Q_OBJECT

    Q_PROPERTY(int nodesNumber READ getNodesNumber WRITE setNodesNumber NOTIFY nodesNumberChanged FINAL)
    Q_PROPERTY(int minXLocation READ getMinXLocation WRITE setMinXLocation NOTIFY minXLocationChanged FINAL)
    Q_PROPERTY(int maxXLocation READ getMaxXLocation WRITE setMaxXLocation NOTIFY maxXLocationChanged FINAL)
    Q_PROPERTY(int minYLocation READ getMinYLocation WRITE setMinYLocation NOTIFY minYLocationChanged FINAL)
    Q_PROPERTY(int maxYLocation READ getMaxYLocation WRITE setMaxYLocation NOTIFY maxYLocationChanged FINAL)
    Q_PROPERTY(double addEdgePropability READ getAddEdgePropability WRITE setAddEdgePropability NOTIFY addEdgePropabilityChanged FINAL)
public:
    explicit GeneralGraphVisualBuilder(QObject *parent = nullptr);

    void buildDataStructureVisualization(DataStructureWidget* dataStructureWidget) override;

    int getNodesNumber() const;
    void setNodesNumber(int newNodesNumber);

    int getMinXLocation() const;
    void setMinXLocation(int newMinXLocation);

    int getMaxXLocation() const;
    void setMaxXLocation(int newMaxXLocation);

    int getMinYLocation() const;
    void setMinYLocation(int newMinYLocation);

    int getMaxYLocation() const;
    void setMaxYLocation(int newMaxYLocation);

    double getAddEdgePropability() const;
    void setAddEdgePropability(double newAddEdgePropability);

signals:
    void nodesNumberChanged();
    void minXLocationChanged();
    void maxXLocationChanged();
    void minYLocationChanged();
    void maxYLocationChanged();
    void addEdgePropabilityChanged();

protected:
    int nodesNumber;

    int minXLocation;
    int maxXLocation;

    int minYLocation;
    int maxYLocation;

    double addEdgePropability;
};

class GridGraphVisualBuilder : public GraphVisualBuilder
{
    Q_OBJECT
public:
    explicit GridGraphVisualBuilder(QObject *parent = nullptr);

    void buildDataStructureVisualization(DataStructureWidget* dataStructureWidget) override;
};


#endif // GRAPH_VISUAL_BUILDER_H
