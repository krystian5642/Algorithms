#ifndef GRAPH_ALGORITHM_VISUALIZERS_H
#define GRAPH_ALGORITHM_VISUALIZERS_H

#include "../core/algorithm_visualizer.h"
#include "edge_list.h"

#include <QStack>

class GraphWidget;
class Graph;

class GraphAlgorithmVisualizer : public AlgorithmVisualizer
{
    Q_OBJECT

    Q_PROPERTY(int start READ getStart WRITE setStart NOTIFY startChanged FINAL)
    Q_PROPERTY(bool randomStart READ getRandomStart WRITE setRandomStart NOTIFY randomStartChanged FINAL)
public:
    explicit GraphAlgorithmVisualizer(QObject *parent = nullptr);
    virtual ~GraphAlgorithmVisualizer() = 0;

    virtual bool supportsDirectedGraph() const;
    virtual bool supportsUndirectedGraph() const;

    void clear() override;

    int getStart() const;
    void setStart(int newStart);

    bool getRandomStart() const;
    void setRandomStart(bool newRandomStart);

    Graph *getGraph() const;
    void setGraph(Graph *newGraph);

signals:
    void startChanged();
    void randomStartChanged();

protected:
    void updateVisualization() override;

    void showInfo(const QString& info) const;

    int start;
    bool randomStart;

    Graph* graph;
    GraphWidget* graphWidget;

protected:
    EdgeList resultEdgeList;
};

class PathFindingAlgorithmVisualizer : public GraphAlgorithmVisualizer
{
    Q_OBJECT

    Q_PROPERTY(int end READ getEnd WRITE setEnd NOTIFY endChanged FINAL)
    Q_PROPERTY(bool randomEnd READ getRandomEnd WRITE setRandomEnd NOTIFY randomEndChanged FINAL)
public:
    explicit PathFindingAlgorithmVisualizer(QObject *parent = nullptr);
    virtual ~PathFindingAlgorithmVisualizer() = 0;

    void clear() override;

    int getEnd() const;
    void setEnd(int newEnd);

    bool getRandomEnd() const;
    void setRandomEnd(bool newRandomEnd);

    bool setStartAndEnd();

signals:
    void endChanged();
    void randomEndChanged();

protected:
    void updateVisualization() override;

    void startVisualization(QWidget* widget);
    void buildResultPath(const QList<int>& prev);

    int end;
    bool randomEnd;

    QList<int> resultPath;
};

class BFSVisualizer final : public GraphAlgorithmVisualizer
{
    Q_OBJECT
public:
    explicit BFSVisualizer(QObject *parent = nullptr);

    void run(QWidget* widget) override;
};

class BFSShortestPathVisualizer final : public PathFindingAlgorithmVisualizer
{
    Q_OBJECT
public:
    explicit BFSShortestPathVisualizer(QObject *parent = nullptr);

    void run(QWidget* widget) override;
};

class DFSVisualizer final : public GraphAlgorithmVisualizer
{
    Q_OBJECT
public:
    explicit DFSVisualizer(QObject *parent = nullptr);

    void run(QWidget* widget) override;

private:
    void DFSHelper(int begin, QList<bool>& visited);
};

class TreeCentersVisualizer final : public GraphAlgorithmVisualizer
{
    Q_OBJECT
public:
    explicit TreeCentersVisualizer(QObject *parent = nullptr);

    void run(QWidget* widget) override;
    void clear() override;
    QWidget* createPropertiesWidget(QWidget* parent) override;

protected:
    void updateVisualization() override;

private:
    QList<QList<int>> visitedLeafLayers;
    QList<int> centers;
};

class TopologicalSortVisualizer final : public GraphAlgorithmVisualizer
{
    Q_OBJECT
public:
    explicit TopologicalSortVisualizer(QObject *parent = nullptr);

    void run(QWidget* widget) override;
    void clear() override;

    bool supportsUndirectedGraph() const override;

protected:
    void updateVisualization() override;

private:
    void TopologicalSortHelper(int begin, QList<bool>& visited);

    QStack<int> topologicalOrder;
};

class KahnsAlgorithmVisualizer final : public GraphAlgorithmVisualizer
{
    Q_OBJECT
public:
    explicit KahnsAlgorithmVisualizer(QObject *parent = nullptr);

    void run(QWidget* widget) override;
    void clear() override;

    bool supportsUndirectedGraph() const override;

protected:
    void updateVisualization() override;

private:
    QList<int> topologicalOrder;
};

class SSSPonDAGVisualizer final : public PathFindingAlgorithmVisualizer
{
    Q_OBJECT
public:
    explicit SSSPonDAGVisualizer(QObject *parent = nullptr);

    void run(QWidget* widget) override;
    bool supportsUndirectedGraph() const override;

private:
    void topologicalSort(QList<int>& outTopologicalOrder) const;
};

class LazyDijkstraVisualizer final : public PathFindingAlgorithmVisualizer
{
    Q_OBJECT
public:
    explicit LazyDijkstraVisualizer(QObject *parent = nullptr);

    void run(QWidget* widget) override;
};

class BellmanFordVisualizer : public PathFindingAlgorithmVisualizer
{
    Q_OBJECT

    Q_PROPERTY(bool breakIfNoChange READ getBreakIfNoChange WRITE setBreakIfNoChange NOTIFY breakIfNoChangeChanged FINAL)
    Q_PROPERTY(bool paintNegativeCycleToBlue READ getPaintNegativeCycleToBlue WRITE setPaintNegativeCycleToBlue NOTIFY paintNegativeCycleToBlueChanged FINAL)
public:
    explicit BellmanFordVisualizer(QObject* parent = nullptr);

    void clear() override;

    bool getBreakIfNoChange() const;
    void setBreakIfNoChange(bool newBreakIfNoChange);

    bool getPaintNegativeCycleToBlue() const;
    void setPaintNegativeCycleToBlue(bool newPaintNegativeCycleToBlue);

signals:
    void breakIfNoChangeChanged();
    void paintNegativeCycleToBlueChanged();

protected:
    void run(QWidget* widget) override;
    void updateVisualization() override;

    bool breakIfNoChange;
    bool paintNegativeCycleToBlue;

    int updateStep;
    EdgeList negativeCycle;
};


#endif // GRAPHALGORITHMVISUALIZERS_H
