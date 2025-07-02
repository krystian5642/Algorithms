#ifndef UTILS_H
#define UTILS_H

#include <queue>

using KeyValuePair = std::pair<int, int>;

struct CompareByValue
{
    bool operator()(const KeyValuePair& a, const KeyValuePair& b) const
    {
        return a.second > b.second;
    }
};

using KeyValuePriorityQueue = std::priority_queue<KeyValuePair, std::vector<KeyValuePair>, CompareByValue>;

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
