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
    BFSVisualizer(QObject *parent = nullptr);

    void run(QWidget* widget) override;
};

class DFSVisualizer : public GraphAlgorithmVisualizer
{
    Q_OBJECT
public:
    DFSVisualizer(QObject *parent = nullptr);

    void run(QWidget* widget) override;
    void clear() override;

private:
    QSet<int> visited;

    void DFSHelper(int start);
};

#endif // GRAPHALGORITHMVISUALIZERS_H
