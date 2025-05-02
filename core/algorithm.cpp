#include "algorithm.h"

Algorithm::Algorithm(QObject* parent)
    : QObject(parent)
    , requestedEnd(false)
{
    setAutoDelete(false);
}

void Algorithm::requestEnd()
{
    requestedEnd = true;
}

Algorithm::~Algorithm()
{

}
