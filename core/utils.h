#ifndef UTILS_H
#define UTILS_H

namespace Utils
{

template<class Container, class Predicate>
void eraseIf(Container& container, Predicate pred)
{
    for(auto it = container.begin(); it != container.end(); )
    {
        if(pred(*it))
        {
            it = container.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

}

#endif // UTILS_H
