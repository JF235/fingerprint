#include "NNList.hpp"

template <typename T>
NNList<T>::NNList(size_t k) : currentSize(0), maxSize(k) {
    entries.reserve(k);
}

template <typename T>
NNList<T>::~NNList() {}

template <typename T>
void NNList<T>::forceInsert(const NNEntry<T>& entry) {
    auto it = std::lower_bound(entries.begin(), entries.end(), entry);
    entries.insert(it, entry);
    ++currentSize;
}

template <typename T>
void NNList<T>::insert(const T& value, double distance) {
    if (currentSize < maxSize || distance < getMaxDistance()) {
        forceInsert(NNEntry<T>{value, distance});
        // If the inserted item exceeds the maximum size, remove the last element
        if (currentSize > maxSize) {
            entries.pop_back();
        }
    }
}

template <typename T>
float NNList<T>::getMaxDistance() const {
    if (entries.empty()) {
        // Return infinity if the list is empty
        return std::numeric_limits<float>::infinity();
    }
    return entries.back().distance;
}

template <typename T>
size_t NNList<T>::size() const {
    return currentSize;
}

template <typename T>
typename std::vector<NNEntry<T>>::const_iterator NNList<T>::begin() const {
    return entries.begin();
}

template <typename T>
typename std::vector<NNEntry<T>>::const_iterator NNList<T>::end() const {
    return entries.end();
}