#ifndef GRAPH_ALGORITHM_VISUALIZERS_H
#define GRAPH_ALGORITHM_VISUALIZERS_H

#include "../core/algorithm_visualizer.h"
#include "edge_list.h"

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
    void updateVisualization() override;

    void showStartIsInvalidInfo();

    int start;
    bool randomStart;

    Graph* graph;
    GraphWidget* graphWidget;

protected:
    EdgeList resultEdgeList;
    qsizetype resultIndex;
};

class BFSVisualizer : public GraphAlgorithmVisualizer
{
    Q_OBJECT
public:
    explicit BFSVisualizer(QObject *parent = nullptr);

    void run(QWidget* widget) override;
};

class BFSShortestPathVisualizer : public GraphAlgorithmVisualizer
{
    Q_OBJECT

    Q_PROPERTY(int end READ getEnd WRITE setEnd NOTIFY endChanged FINAL)
    Q_PROPERTY(bool randomEnd READ getRandomEnd WRITE setRandomEnd NOTIFY randomEndChanged FINAL)
public:
    explicit BFSShortestPathVisualizer(QObject *parent = nullptr);

    void run(QWidget* widget) override;
    void clear() override;

    int getEnd() const;
    void setEnd(int newEnd);

    bool getRandomEnd() const;
    void setRandomEnd(bool newRandomEnd);

signals:
    void endChanged();
    void randomEndChanged();

protected:
    void updateVisualization() override;

    int end;
    bool randomEnd;

    QList<int> resultPath;
};

class DFSVisualizer : public GraphAlgorithmVisualizer
{
    Q_OBJECT
public:
    explicit DFSVisualizer(QObject *parent = nullptr);

    void run(QWidget* widget) override;

private:
    void DFSHelper(int start, QList<int>& visited);
};

#endif // GRAPHALGORITHMVISUALIZERS_H
