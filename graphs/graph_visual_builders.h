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
    Q_PROPERTY(int minWeight READ getMinWeight WRITE setMinWeight NOTIFY minWeightChanged FINAL)
    Q_PROPERTY(int maxWeight READ getMaxWeight WRITE setMaxWeight NOTIFY maxWeightChanged FINAL)
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

    int getMinWeight() const;
    void setMinWeight(int newMinWeight);

    int getMaxWeight() const;
    void setMaxWeight(int newMaxWeight);

signals:
    void nodesNumberChanged();

    void minXLocationChanged();
    void maxXLocationChanged();

    void minYLocationChanged();
    void maxYLocationChanged();

    void addEdgePropabilityChanged();

    void minWeightChanged();
    void maxWeightChanged();

protected:
    int nodesNumber;

    int minXLocation;
    int maxXLocation;

    int minYLocation;
    int maxYLocation;

    double addEdgePropability;

    int minWeight;
    int maxWeight;
};

class GridGraphVisualBuilder : public GraphVisualBuilder
{
    Q_OBJECT

    Q_PROPERTY(int columns READ getColumns WRITE setColumns NOTIFY columnsChanged FINAL)
    Q_PROPERTY(int rows READ getRows WRITE setRows NOTIFY rowsChanged FINAL)
    Q_PROPERTY(int nodeSpace READ getNodeSpace WRITE setNodeSpace NOTIFY nodeSpaceChanged FINAL)
    Q_PROPERTY(int startXLocation READ getStartXLocation WRITE setStartXLocation NOTIFY startXLocationChanged FINAL)
    Q_PROPERTY(int startYLocation READ getStartYLocation WRITE setStartYLocation NOTIFY startYLocationChanged FINAL)
public:
    explicit GridGraphVisualBuilder(QObject *parent = nullptr);

    void buildDataStructureVisualization(DataStructureWidget* dataStructureWidget) override;

    int getColumns() const;
    void setColumns(int newColumns);

    int getRows() const;
    void setRows(int newRows);

    int getNodeSpace() const;
    void setNodeSpace(int newNodeSpace);

    int getStartXLocation() const;
    void setStartXLocation(int newStartXLocation);

    int getStartYLocation() const;
    void setStartYLocation(int newStartYLocation);

signals:
    void columnsChanged();
    void rowsChanged();

    void nodeSpaceChanged();

    void startXLocationChanged();
    void startYLocationChanged();

protected:
    int columns;
    int rows;

    int nodeSpace;

    int startXLocation;
    int startYLocation;
};


#endif // GRAPH_VISUAL_BUILDER_H
