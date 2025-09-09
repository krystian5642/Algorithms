#ifndef UTILS_H
#define UTILS_H

#include <QHash>
#include <QList>

#define INF 1e9

namespace Utils
{
    template <typename Container, typename Predicate>
    int find_index_if(const Container& container, Predicate pred)
    {
        auto it = std::find_if(container.begin(), container.end(), pred);
        if (it == container.end())
        {
            return -1;
        }

        return static_cast<int>(std::distance(container.begin(), it));
    }

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

struct CompareByMinValue
{
    bool operator()(int a, int b) const
    {
        return a > b;
    }
};

struct CompareByMaxValue
{
    bool operator()(int a, int b) const
    {
        return a < b;
    }
};

namespace HeapHelpers
{
    inline int getLeftChild(const int index)
    {
        return 2 * index + 1;
    }

    inline int getRightChild(const int index)
    {
        return 2 * index + 2;
    }

    inline int getParent(const int index)
    {
        return (index - 1) / 2;
    }
}

using IntPair = QPair<int, int>;

template<typename Compare = CompareByMinValue>
class PriorityQueue
{
public:
    void insert(const int key, const int value)
    {
        pairs.push_back({key, value});
        heapifyUp(pairs.size() - 1);
    }

    IntPair extract()
    {
        Q_ASSERT(!pairs.isEmpty());
        const IntPair pair = pairs[0];

        pairs[0] = pairs.back();
        pairs.pop_back();

        if(pairs.size() > 1)
        {
            heapifyDown(0);
        }

        return pair;
    }

    const IntPair& peek() const
    {
        Q_ASSERT(!pairs.isEmpty());
        return pairs.first();
    }

    void deleteKey(const int key)
    {
        const auto index = Utils::find_index_if(pairs, [key](const IntPair& pair)
        {
            return key == pair.first;
        });

        if(index != -1)
        {
            if(index < pairs.size() - 1)
            {
                std::swap(pairs[index], pairs.back());
                pairs.pop_back();

                heapifyDown(index);
            }
            else
            {
                pairs.pop_back();
            }
        }
    }

    bool contains(const int key) const
    {
        return std::any_of(pairs.begin(), pairs.end(), [key](const IntPair& pair)
        {
            return pair.first == key;
        });
    }

    bool empty() const
    {
        return pairs.empty();
    }

    qsizetype size() const
    {
        return pairs.size();
    }

    void reserve(const qsizetype size)
    {
        pairs.reserve(size);
    }

private:
    void heapifyDown(int index)
    {
        const int leftChild = HeapHelpers::getLeftChild(index);
        const int rightChild = HeapHelpers::getRightChild(index);
        int smallest = index;

        const Compare compare;
        if(leftChild < pairs.size() && compare(pairs[smallest].second, pairs[leftChild].second))
        {
            smallest = leftChild;
        }
        if(rightChild < pairs.size() && compare(pairs[smallest].second, pairs[rightChild].second))
        {
            smallest = rightChild;
        }

        if(smallest != index)
        {
            std::swap(pairs[index], pairs[smallest]);
            heapifyDown(smallest);
        }
    }

    void heapifyUp(int index)
    {
        const Compare compare;
        int parent = HeapHelpers::getParent(index);
        if(index > 0 && compare(pairs[parent].second, pairs[index].second))
        {
            std::swap(pairs[index], pairs[parent]);
            heapifyUp(parent);
        }
    }

private:
    QList<IntPair> pairs;
};

template<typename Compare = CompareByMinValue>
class IndexedPriorityQueue
{
public:
    void insert(int key, int value)
    {
        const int oldSize = pairs.size();
        pairs.push_back({key, value});
        keyIndices[key] = oldSize;

        heapifyUp(oldSize);
    }

    IntPair extract()
    {
        Q_ASSERT(!pairs.isEmpty());

        const IntPair pair = pairs[0];
        keyIndices.remove(pairs[0].first);

        if(pairs.size() > 1)
        {
            pairs[0] = pairs.back();
            pairs.pop_back();
            keyIndices[pairs[0].first] = 0;
            heapifyDown(0);
        }
        else
        {
            pairs.pop_back();
        }

        return pair;
    }

    const IntPair& peek() const
    {
        Q_ASSERT(!pairs.isEmpty());
        return pairs.first();
    }

    void deleteKey(int key)
    {
        const auto it = keyIndices.find(key);
        if(it != keyIndices.end())
        {
            int index = it.value();
            keyIndices.erase(it);

            if(index < pairs.size() - 1)
            {
                std::swap(pairs[index], pairs.back());
                keyIndices[pairs[index].first] = index;
                pairs.pop_back();

                heapifyDown(index);
            }
            else
            {
                pairs.pop_back();
            }
        }
    }

    void setValue(int key, int value)
    {
        const auto it = keyIndices.find(key);
        if(it != keyIndices.end())
        {
            const int index = it.value();

            const int oldValue = pairs[index].second;
            if(oldValue != value)
            {
                pairs[index].second = value;

                const Compare compare;
                if(compare(oldValue, value))
                {
                    heapifyUp(index);
                }
                else
                {
                    heapifyDown(index);
                }
            }
        }
        else
        {
            insert(key, value);
        }
    }

    bool contains(int key) const
    {
        return keyIndices.contains(key);
    }

    bool empty() const
    {
        return pairs.empty();
    }

    qsizetype size() const
    {
        return pairs.size();
    }

private:
    void heapifyDown(int index)
    {
        const int leftChild = HeapHelpers::getLeftChild(index);
        const int rightChild = HeapHelpers::getRightChild(index);
        int smallest = index;

        const Compare compare;
        if(leftChild < pairs.size() && compare(pairs[smallest].second, pairs[leftChild].second))
        {
            smallest = leftChild;
        }
        if(rightChild < pairs.size() && compare(pairs[smallest].second, pairs[rightChild].second))
        {
            smallest = rightChild;
        }

        if(smallest != index)
        {
            std::swap(pairs[index], pairs[smallest]);
            std::swap(keyIndices[pairs[index].first], keyIndices[pairs[smallest].first]);
            heapifyDown(smallest);
        }
    }

    void heapifyUp(int index)
    {
        const Compare compare;
        int parent = HeapHelpers::getParent(index);
        if(index > 0 && compare(pairs[parent].second, pairs[index].second))
        {
            std::swap(pairs[index], pairs[parent]);
            std::swap(keyIndices[pairs[index].first], keyIndices[pairs[parent].first]);

            heapifyUp(parent);
        }
    }

private:
    QList<IntPair> pairs;
    QHash<int, int> keyIndices;
};

#endif // UTILS_H
