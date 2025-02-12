#ifndef EDGELIST_H
#define EDGELIST_H

#include <QList>
#include <QPair>

class EdgeList
{
public:
    using GraphEdge = QPair<int, int>;
    using const_iterator = QList<QPair<int, int>>::const_iterator;
    using iterator = QList<QPair<int, int>>::iterator;

    EdgeList();

    void insert(int start, int end);
    bool isValidIndex(qsizetype index);

private:
    QList<GraphEdge> edges;

public:
    QPair<int, int>& operator[](qsizetype index);

    // to support range-based operation for loop
    inline QList<GraphEdge>::iterator        begin() { return edges.begin(); }
    inline QList<GraphEdge>::const_iterator  constBegin() const { return edges.constBegin(); }
    inline QList<GraphEdge>::iterator        end() { return edges.end(); }
    inline QList<GraphEdge>::const_iterator  constEnd() const { return edges.constEnd(); }
};

#endif // EDGELIST_H
