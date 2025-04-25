#include "edge_list.h"

EdgeList::EdgeList()
{

}

void EdgeList::add(int start, int end)
{
    const QPair<int, int> edge(qMin(start, end), qMax(start, end));
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

const QPair<int, int> &EdgeList::getLastEdge() const
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

const QPair<int, int> &EdgeList::operator[](qsizetype index) const
{
    return edges[index];
}
