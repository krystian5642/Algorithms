#ifndef ALGORITHM_H
#define ALGORITHM_H

class Algorithm
{
public:
    Algorithm();
    virtual ~Algorithm() = 0;

    virtual void run() = 0;
};

#endif // ALGORITHM_H
