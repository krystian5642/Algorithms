#ifndef GRAPH_BUILDERS_H
#define GRAPH_BUILDERS_H

#include "../core/data_structure_builder.h"

class Graph;

class GraphBuilder : public DataStructureBuilder
{
    Q_OBJECT

    Q_PROPERTY(bool isGraphDirected READ getIsGraphDirected WRITE setIsGraphDirected NOTIFY isGraphDirectedChanged FINAL)
public:
    explicit GraphBuilder(QObject *parent = nullptr);
    virtual ~GraphBuilder() = 0;

    QWidget* createPropertiesWidget(QWidget* parent = nullptr) override;
    void appendPropertiesInfo(QString& infoText) override;

    int buildIterations;

    bool getIsGraphDirected() const;
    void setIsGraphDirected(bool newIsGraphDirected);

signals:
    void isGraphDirectedChanged();

protected:
    Graph* createGraph() const;

    bool isGraphDirected;

    QString selectedImplementation;
};

class GeneralGraphBuilder : public GraphBuilder
{
    Q_OBJECT

    Q_PROPERTY(double addEdgePropability READ getAddEdgePropability WRITE setAddEdgePropability NOTIFY addEdgePropabilityChanged FINAL)
    Q_PROPERTY(int minWeight READ getMinWeight WRITE setMinWeight NOTIFY minWeightChanged FINAL)
    Q_PROPERTY(int maxWeight READ getMaxWeight WRITE setMaxWeight NOTIFY maxWeightChanged FINAL)
public:
    explicit GeneralGraphBuilder(QObject *parent = nullptr);

    DataStructure* createDataStructure() override;

    double getAddEdgePropability() const;
    void setAddEdgePropability(double newAddEdgePropability);

    int getMinWeight() const;
    void setMinWeight(int newMinWeight);

    int getMaxWeight() const;
    void setMaxWeight(int newMaxWeight);

signals:
    void addEdgePropabilityChanged();
    void minWeightChanged();

    void maxWeightChanged();

protected:
    double addEdgePropability;
    int minWeight;
    int maxWeight;

};

class GridGraphBuilder : public GraphBuilder
{
    Q_OBJECT
public:
    explicit GridGraphBuilder(QObject *parent = nullptr);

    DataStructure* createDataStructure() override;
};

class TreeGraphBuilder : public GraphBuilder
{
    Q_OBJECT
public:
    explicit TreeGraphBuilder(QObject *parent = nullptr);

    DataStructure* createDataStructure() override;
};

#endif // GENERAL_GRAPH_BUILDER_H
