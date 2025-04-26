#include "algorithm.h"

Algorithm::Algorithm(QObject* parent)
    : QObject(parent)
{
    setAutoDelete(false);
}

Algorithm::~Algorithm()
{

}
