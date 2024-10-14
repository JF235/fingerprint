#ifndef SEQUENTIAL_SEARCHER_HPP
#define SEQUENTIAL_SEARCHER_HPP

#include <vector>
#include <functional> // For std::function
#include <typeinfo> // For typeid
#include "NNList.hpp"

/**
 * @brief A class for performing sequential k-nearest neighbors search.
 * 
 * @tparam T The type of the objects stored in dataObjects.
 * @tparam DistanceFunc The type of the distance function.
 */
template <typename T, typename DistanceFunc>
class SequentialSearcher {
public:
    /**
     * @brief Constructs a SequentialSearcher with the given distance function.
     * 
     * @param distFunc The distance function to evaluate distance between objects.
     */
    SequentialSearcher(DistanceFunc distFunc);

    /**
     * @brief Performs k-nearest neighbors search.
     * 
     * @param query The query object.
     * @param k The number of nearest neighbors to find.
     * @return NNList<T> The list of k-nearest neighbors.
     */
    NNList<T> knn(const T& query, size_t k) const;

    /**
     * @brief Adds a single object to the dataObjects.
     * 
     * @param obj The object to add.
     */
    void add(const T& obj);

    /**
     * @brief Adds all objects from a vector to the dataObjects.
     * 
     * @param objs The vector of objects to add.
     */
    void addAll(const std::vector<T>& objs);

    /**
     * @brief Returns the number of objects in the search structure.
     * 
     * @return size_t The number of objects in the search structure.
     */
    size_t size() const;

    /**
     * @brief Overloads the << operator for printing the SequentialSearcher.
     * 
     * @param os The output stream.
     * @param searcher The SequentialSearcher to print.
     * @return The output stream.
     */
    template <typename U, typename V>
    friend std::ostream& operator<<(std::ostream& os, const SequentialSearcher<U, V>& searcher);

private:
    std::vector<T> dataObjects; ///< The data objects to be searched.
    DistanceFunc distanceFunc; ///< The distance function to evaluate distance between objects.
};

#include "SequentialSearcher.tpp"

#endif // SEQUENTIAL_SEARCHER_HPP