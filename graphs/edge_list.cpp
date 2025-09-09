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

EdgeList::EdgeList(bool inAllowDuplicates)
    : allowDuplicates(inAllowDuplicates)
{

}

void EdgeList::add(int start, int end, bool directed)
{
    const Edge edge(start, end, directed);
    add(edge);
}

void EdgeList::add(const Edge &edge)
{
    if(allowDuplicates)
    {
        edges.append(edge);
    }
    else if(!helperSet.contains(edge))
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

const Edge &EdgeList::first() const
{
    return edges.first();
}

Edge EdgeList::takeFirst()
{
    return edges.takeFirst();
}

bool EdgeList::empty() const
{
    return edges.empty();
}

qsizetype EdgeList::size() const
{
    return edges.size();
}

bool EdgeList::contains(const Edge &edge) const
{
    return helperSet.contains(edge);
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

void EdgeList::setAllowDuplicates(bool inAllowDuplicates)
{
    allowDuplicates = inAllowDuplicates;
}

const Edge &EdgeList::operator[](qsizetype index) const
{
    return edges[index];
}
