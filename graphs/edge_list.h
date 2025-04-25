#ifndef EDGE_LIST_H
#define EDGE_LIST_H

#include <QList>
#include <QPair>
#include <QSet>

class EdgeList
{
public:
    using GraphEdge = QPair<int, int>;
    using const_iterator = QList<QPair<int, int>>::const_iterator;
    using iterator = QList<QPair<int, int>>::iterator;

    EdgeList();

    void add(int start, int end);
    void reserve(qsizetype size);

    const QPair<int, int>& getLastEdge() const;
    bool isValidIndex(qsizetype index) const;

    void clear();

private:
    QList<GraphEdge> edges;
    QSet<GraphEdge> helperSet;

public:
    const QPair<int, int>& operator[](qsizetype index) const;

    // to support range-based operation for loop
    inline QList<GraphEdge>::iterator        begin() { return edges.begin(); }
    inline QList<GraphEdge>::const_iterator  constBegin() const { return edges.constBegin(); }
    inline QList<GraphEdge>::iterator        end() { return edges.end(); }
    inline QList<GraphEdge>::const_iterator  constEnd() const { return edges.constEnd(); }
};

#endif // EDGE_LIST_H
