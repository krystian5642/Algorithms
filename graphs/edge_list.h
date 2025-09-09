#ifndef EDGE_LIST_H
#define EDGE_LIST_H

#include <QList>
#include <QPair>
#include <QSet>

struct Edge
{
public:
    explicit Edge(int inStart, int inEnd, bool directed);

    int getStart() const;
    int getEnd() const;

public:
    bool operator==(const Edge& other) const = default;

private:
    int start;
    int end;
};

inline uint qHash(const Edge& edge, uint seed = 0)
{
    return qHash(QPair<int, int>(edge.getStart(), edge.getEnd()), seed);
}

class EdgeList
{
public:
    EdgeList(bool inAllowDuplicates = false);

    void add(int start, int end, bool directed);
    void add(const Edge& edge);
    void reserve(qsizetype size);
    const Edge& first() const;
    Edge takeFirst();
    bool empty() const;
    qsizetype size() const;
    bool contains(const Edge& edge) const;

    const Edge& getLastEdge() const;
    bool isValidIndex(qsizetype index) const;

    void clear();

    void setAllowDuplicates(bool inAllowDuplicates);

private:
    QList<Edge> edges;
    QSet<Edge> helperSet;

    bool allowDuplicates;

public:
    const Edge& operator[](qsizetype index) const;

    // to support range-based operation for loop
    inline QList<Edge>::iterator        begin() { return edges.begin(); }
    inline QList<Edge>::const_iterator  constBegin() const { return edges.constBegin(); }
    inline QList<Edge>::iterator        end() { return edges.end(); }
    inline QList<Edge>::const_iterator  constEnd() const { return edges.constEnd(); }
};

#endif // EDGE_LIST_H
