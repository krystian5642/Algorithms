#ifndef GRAPH_AlGORITHMS_H
#define GRAPH_AlGORITHMS_H

#include "../core/algorithm.h"

#include <QQueue>
#include <QSet>
#include <QWeakPointer>

class QComboBox;
class DataStructure;
class GraphNode;
class Graph;

class GraphAlgorithm : public Algorithm
{
    Q_OBJECT
public:
    explicit GraphAlgorithm(QObject* parent = nullptr);
    virtual ~GraphAlgorithm() = 0;

    QWidget* createPropertiesWidget(QWidget* parent = nullptr) override;
    void appendPropertiesInfo(QString& infoText) override;

    void run() override;
    void init(const DataStructure* dataStructure) override;
    void clear() override;

#ifdef QT_DEBUG
    void debugRun() override;
#endif

    DataStructureBuilder* getSelectedBuilder() const;

protected:
    const Graph* graph;

private:
    QComboBox* builderComboBox;
    QWidget* builderPropertiesWidget;
};

class BFSIterative : public GraphAlgorithm
{
    Q_OBJECT
public:
    explicit BFSIterative(QObject* parent = nullptr);

    void init(const DataStructure* dataStructure) override;
    void clear() override;

protected:
    void execute() override;

    QQueue<int> nodeQueue;
    QList<bool> visited;
};

class BFSRecursive : public GraphAlgorithm
{
    Q_OBJECT
public:
    explicit BFSRecursive(QObject* parent = nullptr);

    void init(const DataStructure* dataStructure) override;
    void clear() override;

protected:
    void execute() override;
    void BFSRecursiveHelper(std::function<void(int, int, int)> forEachNeighbourFunc);

    QQueue<int> nodeQueue;
    QList<bool> visited;
};

class DFSRecursive : public GraphAlgorithm
{
    Q_OBJECT
public:
    explicit DFSRecursive(QObject* parent = nullptr);

    void init(const DataStructure* dataStructure) override;
    void clear() override;

protected:
    void execute() override;
    void DFSHelper(int begin);

    QList<bool> visited;
};

#endif // GRAPH_AlGORITHMS_H
