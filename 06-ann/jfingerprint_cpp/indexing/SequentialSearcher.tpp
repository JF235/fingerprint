#include "SequentialSearcher.hpp"

// Constructor
template <typename T, typename DistanceFunc>
SequentialSearcher<T, DistanceFunc>::SequentialSearcher(DistanceFunc &distFunc)
    : distanceFunc(distFunc) {}

// Method to perform k-nearest neighbors search
template <typename T, typename DistanceFunc>
NNList<T> SequentialSearcher<T, DistanceFunc>::knn(const T &query, size_t k) const
{
    NNList<T> nnList(k);

    // Sequentially calculate the distance between the query object and all objects in dataObjects
    // Takes O(n) distance calculations
    for (const auto &obj : dataObjects)
    {
        double dist = distanceFunc(query, obj);

        nnList.insert(obj, dist);
    }

    return nnList;
}

// Method to add a single object to the dataObjects
template <typename T, typename DistanceFunc>
void SequentialSearcher<T, DistanceFunc>::add(const T &obj)
{
    dataObjects.push_back(obj);
}

// Method to add all objects from a vector to the dataObjects
template <typename T, typename DistanceFunc>
void SequentialSearcher<T, DistanceFunc>::addAll(const std::vector<T> &objs)
{
    dataObjects.insert(dataObjects.end(), objs.begin(), objs.end());
}

// Method to get size of dataObjects
template <typename T, typename DistanceFunc>
size_t SequentialSearcher<T, DistanceFunc>::size() const
{
    return dataObjects.size();
}

//Overload of the << operator for printing the SequentialSearcher
template <typename T, typename DistanceFunc>
std::ostream &operator<<(std::ostream &os, const SequentialSearcher<T, DistanceFunc> &searcher)
{
    // os << "SequentialSearcher with objects of type: " << typeid(T).name() << "\n";
    // os << "Using distance function: " << typeid(DistanceFunc).name() << "\n";
    // os << "Data objects:\n";
    for (const auto &obj : searcher.dataObjects)
    {
        os << obj << "\n";
    }
    return os;
}