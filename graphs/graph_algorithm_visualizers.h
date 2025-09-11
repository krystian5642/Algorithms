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
    void startVisualization(QWidget* widget);
    void updateVisualization() override;

    void showInfo(const QString& info) const;

    void finish();

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

    bool isDataStructureSupported(const DataStructure* dataStructure, QString& outInfo) const override;

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

    bool isDataStructureSupported(const DataStructure* dataStructure, QString& outInfo) const override;

protected:
    void updateVisualization() override;

private:
    QList<int> topologicalOrder;
};

// Single Source Shortest Path (SSSP on DAG)
class SSSPonDAGVisualizer final : public PathFindingAlgorithmVisualizer
{
    Q_OBJECT
public:
    explicit SSSPonDAGVisualizer(QObject *parent = nullptr);

    void run(QWidget* widget) override;
    bool isDataStructureSupported(const DataStructure* dataStructure, QString& outInfo) const override;

private:
    void topologicalSort(QList<int>& outTopologicalOrder) const;
};

class LazyDijkstraVisualizer final : public PathFindingAlgorithmVisualizer
{
    Q_OBJECT
public:
    explicit LazyDijkstraVisualizer(QObject *parent = nullptr);

    bool isDataStructureSupported(const DataStructure* dataStructure, QString& outInfo) const override;

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

class FloydWarshallVisualizer : public PathFindingAlgorithmVisualizer
{
    Q_OBJECT

    Q_PROPERTY(bool paintNegativeCycleToBlue READ getPaintNegativeCycleToBlue WRITE setPaintNegativeCycleToBlue NOTIFY paintNegativeCycleToBlueChanged FINAL)
public:
    explicit FloydWarshallVisualizer(QObject* parent = nullptr);

    void clear() override;

    bool getPaintNegativeCycleToBlue() const;
    void setPaintNegativeCycleToBlue(bool newPaintNegativeCycleToBlue);

signals:
    void paintNegativeCycleToBlueChanged();

protected:
    void run(QWidget* widget) override;
    void updateVisualization() override;

    bool paintNegativeCycleToBlue;

    EdgeList negativeCycle;
};

// Tarjan's Strongly Connected Component
class SCCsVisualizer : public GraphAlgorithmVisualizer
{
    Q_OBJECT
public:
    explicit SCCsVisualizer(QObject* parent = nullptr);

    void clear() override;

protected:
    void run(QWidget* widget) override;
    void SCCsHelper(int begin, QList<int> &visitTime, QStack<int> &stack, QList<int> &low);

    void updateVisualization() override;

    int sccCount;
    int time;

    QList<EdgeList> SCCs;

    QStack<int> sccStart;

private:
    static inline QList<QColor> sccColors =
    {
        Qt::green,
        Qt::blue,
        Qt::cyan,
        Qt::magenta,
        Qt::gray
    };
};

// Traveling Salesman Problem ( Held-Karp )
class TravelingSalesmanProblemVisualizer : public GraphAlgorithmVisualizer
{
    Q_OBJECT
public:
    explicit TravelingSalesmanProblemVisualizer(QObject* parent = nullptr);

protected:
    void run(QWidget* widget) override;
    bool isNotInCombination(int i, int combination) const;
    void buildResultPath(const QList<QList<int>> &memo);

    void generateCombinations(int r, QList<int> &combinations) const;
    void generateCombinations(int subSet, int pos, int r, QList<int> &combinations) const;
};

class EulerianPathAlgorithmVisualizer : public GraphAlgorithmVisualizer
{
    Q_OBJECT
public:
    explicit EulerianPathAlgorithmVisualizer(QObject* parent = nullptr);

protected:
    void run(QWidget* widget) override;

    void DFSHelper(int begin, QList<int>& outDegrees, QList<int>& eulerianPath, EdgeList &visitedUndirectedEdges) const;
    bool doesEulerianPathExist(const QList<int> &inDegrees, const QList<int> &outDegrees) const;
    int findStart(const QList<int> &inDegrees, const QList<int> &outDegrees) const;
};

class PrimMinimumSpanningTreeAlgorithmVisualizer : public GraphAlgorithmVisualizer
{
    Q_OBJECT
public:
    explicit PrimMinimumSpanningTreeAlgorithmVisualizer(QObject* parent = nullptr);

    bool isDataStructureSupported(const DataStructure* dataStructure, QString& outInfo) const override;

protected:
    void run(QWidget* widget) override;

    struct CompareByMinWeight
    {
        bool operator()(const std::pair<int, Edge>& a, const std::pair<int, Edge>& b)
        {
            return a.first > b.first;
        }
    };
};

#endif // GRAPHALGORITHMVISUALIZERS_H
