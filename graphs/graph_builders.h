#ifndef GRAPH_BUILDERS_H
#define GRAPH_BUILDERS_H

#include "../core/data_structure_builder.h"

class Graph;

class GraphBuilder : public DataStructureBuilder
{
    Q_OBJECT
public:
    explicit GraphBuilder(QObject *parent = nullptr);
    virtual ~GraphBuilder() = 0;

    QWidget* createPropertiesWidget(QWidget* parent = nullptr) override;
    void appendPropertiesInfo(QString& infoText) override;

    int buildIterations;

protected:
    Graph* createGraph() const;

    QString selectedImplementation;
};

class GeneralGraphBuilder : public GraphBuilder
{
    Q_OBJECT

    Q_PROPERTY(double addEdgePropability READ getAddEdgePropability WRITE setAddEdgePropability NOTIFY addEdgePropabilityChanged FINAL)
public:
    explicit GeneralGraphBuilder(QObject *parent = nullptr);

    DataStructure* createDataStructure() override;

    double getAddEdgePropability() const;
    void setAddEdgePropability(double newAddEdgePropability);

signals:
    void addEdgePropabilityChanged();

protected:
    double addEdgePropability;
};

class GridGraphBuilder : public GraphBuilder
{
    Q_OBJECT
public:
    explicit GridGraphBuilder(QObject *parent = nullptr);

    DataStructure* createDataStructure() override;
};

#endif // GENERAL_GRAPH_BUILDER_H
