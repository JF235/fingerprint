#ifndef NNLIST_HPP
#define NNLIST_HPP

#include <vector>
#include <limits>

/**
 * @brief Represents an entry in the nearest neighbors list.
 * 
 * @tparam T The type of the elements in the list.
 */
template <typename T>
struct NNEntry {
    T element;
    double distance;

    NNEntry(const T& elem, double dist) : element(elem), distance(dist) {}

    /**
     * @brief Compares two NNEntry objects by their distance.
     * 
     * @param other The other NNEntry object to compare with.
     * @return True if this object's distance is less than the other's distance, false otherwise.
     */
    bool operator<(const NNEntry& other) const {
        return distance < other.distance;
    }

    /**
     * @brief Overload of the output operator to format the output as (element, distance).
     * @param os Output stream.
     * @param entry NNEntry object to be printed.
     * @return Output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const NNEntry& entry) {
        os << "(" << entry.element << ", " << entry.distance << ")";
        return os;
    }
};

/**
 * @brief A class to manage a list of nearest neighbors.
 * 
 * @tparam T The type of the elements in the list.
 */
template <typename T>
class NNList {
public:
    /**
     * @brief Constructs an NNList with the given number of empty elements, with the default distance set to infinity.
     * 
     * @param k The number of empty elements to initialize the list with. Default is 0.
     */
    NNList(size_t k = 0);

    /**
     * @brief Destructor for NNList.
     */
    ~NNList();

    /**
     * @brief Inserts an entry into the nearest neighbors list in a smart way.
     * 
     * This function inserts an entry into the list if there is available capacity
     * or if the list is full but the new entry is closer than the farthest entry in the list.
     * In the latter case, the farthest entry is removed to maintain the correct size of the list.
     * 
     * @tparam T The type of the elements in the list.
     * @param value The element to insert.
     * @param distance The distance associated with the element.
     */
    void insert(const T& element, double distance);

    /**
     * @brief Forces the insertion of an entry into the list, even if it is full.
     * 
     * @param entry The entry to insert.
     */
    void forceInsert(const NNEntry<T>& entry);

    /**
     * @brief Returns the maximum distance in the nearest neighbors list.
     * @return The maximum distance in the nearest neighbors list.
     */
    float getMaxDistance() const;

    /**
     * @brief Returns the number of elements in the nearest neighbors list.
     * @return The number of elements in the nearest neighbors list.
     */
    size_t size() const;

    /**
     * @brief Returns iterator to the beginning of the nearest neighbors list.
     * @return Iterator to the beginning of the nearest neighbors list.
     */
    typename std::vector<NNEntry<T>>::const_iterator begin() const;

    /**
     * @brief Returns iterator to the end of the nearest neighbors list.
     * @return Iterator to the end of the nearest neighbors list.
     */
    typename std::vector<NNEntry<T>>::const_iterator end() const;

    /**
     * @brief Overload of the output operator to format the output as [NNEntry1, NNEntry2, ...].
     * @param os Output stream.
     * @param list NNList object to be printed.
     * @return Output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const NNList& list) {
        os << "[";
        for (size_t i = 0; i < list.entries.size(); ++i) {
            os << list.entries[i];
            if (i < list.entries.size() - 1) {
                os << ", ";
            }
        }
        os << "]";
        return os;
    }

    /**
     * @brief Overload of the [] operator to access entries by index.
     * @param index Index of the entry to access.
     * @return Reference to the NNEntry at the specified index.
     */
    NNEntry<T>& operator[](size_t index) {
        return entries[index];
    }

    /**
     * @brief Overload of the [] operator to access entries by index (const version).
     * @param index Index of the entry to access.
     * @return Const reference to the NNEntry at the specified index.
     */
    const NNEntry<T>& operator[](size_t index) const {
        return entries[index];
    }

    std::vector<NNEntry<T>> entries;
private:
    size_t currentSize;
    size_t maxSize;
};

#include "NNList.tpp"

#endif // NNLIST_HPP