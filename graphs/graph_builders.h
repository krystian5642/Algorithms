#ifndef GRAPH_BUILDERS_H
#define GRAPH_BUILDERS_H

#include "../core/data_structure_builder.h"

class GraphBuilder : public DataStructureBuilder
{
    Q_OBJECT
public:
    explicit GraphBuilder(QObject *parent = nullptr);
    virtual ~GraphBuilder() = 0;

    QWidget* createPropertiesWidget(QWidget* parent = nullptr) override;

    int buildIterations;

protected:
    QString selectedImplementation;
};

class GeneralGraphBuilder : public GraphBuilder
{
    Q_OBJECT

    Q_PROPERTY(double addEdgePropability READ getAddEdgePropability WRITE setAddEdgePropability NOTIFY addEdgePropabilityChanged FINAL)
public:
    explicit GeneralGraphBuilder(QObject *parent = nullptr);

    DataStructure* buildDataStructure() override;

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

    Q_PROPERTY(int rows READ getRows WRITE setRows NOTIFY rowsChanged FINAL)
    Q_PROPERTY(int columns READ getColumns WRITE setColumns NOTIFY columnsChanged FINAL)
public:
    explicit GridGraphBuilder(QObject *parent = nullptr);

    DataStructure* buildDataStructure() override;

    int getRows() const;
    void setRows(int newRows);

    int getColumns() const;
    void setColumns(int newColumns);

signals:
    void rowsChanged();
    void columnsChanged();

protected:
    int rows;
    int columns;
};

#endif // GENERAL_GRAPH_BUILDER_H
