#include "edge_list.h"

Edge::Edge(int inStart, int inEnd, bool directed)
{
    start = directed ? inStart : qMin(inStart, inEnd);
    end = directed ? inEnd : qMax(inStart, inEnd);
}

int Edge::getStart() const
{
    return start;
}

int Edge::getEnd() const
{
    return end;
}

EdgeList::EdgeList()
{

}

void EdgeList::add(int start, int end, bool directed)
{
    const Edge edge(start, end, directed);
    if(!helperSet.contains(edge))
    {
        edges.append(edge);
        helperSet.insert(edge);
    }
}

void EdgeList::reserve(qsizetype size)
{
    edges.reserve(size);
    helperSet.reserve(size);
}

Edge EdgeList::takeFirst()
{
    return edges.takeFirst();
}

bool EdgeList::empty() const
{
    return edges.empty();
}

const Edge& EdgeList::getLastEdge() const
{
    return edges.last();
}

bool EdgeList::isValidIndex(qsizetype index) const
{
    return edges.size() > index;
}

void EdgeList::clear()
{
    edges.clear();
    helperSet.clear();
}

const Edge &EdgeList::operator[](qsizetype index) const
{
    return edges[index];
}
