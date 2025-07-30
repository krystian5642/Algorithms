#ifndef GRAPH_TEXTS_H
#define GRAPH_TEXTS_H

#include <QString>

namespace GraphTexts
{
    inline const QString DirectedGraphIsNotSupported = "Directed graph is not supported by this algorithm.";
    inline const QString UndirectedGraphIsNotSupported = "Undirected graph is not supported by this algorithm.";
    inline const QString EmptyGraph = "Graph is empty.";
    inline const QString StartNodeIsInvalid = "Start node is invalid.";
    inline const QString EndNodeIsInvalid = "End node is invalid.";
    inline const QString StartOrEndNodesAreInvalid = "Start or end nodes are invalid.";
}

#endif // GRAPH_TEXTS_H
