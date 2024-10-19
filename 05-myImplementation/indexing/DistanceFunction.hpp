#ifndef DISTANCES_HPP
#define DISTANCES_HPP

#include <iostream> // For std::cerr
#include <vector> // For std::vector
#include <cmath> // For std::sqrt, std::abs
#include <numeric> // For std::inner_product
#include <stdexcept> // For std::invalid_argument

/**
 * @brief Base class for distance functions.
 * 
 * @tparam T The type of the elements in the vectors.
 */
template <typename T>
class DistanceFunction {
public:
    static unsigned long int distanceFunctionCalls;

    /**
     * @brief Computes the distance between two vectors.
     * 
     * @param a The first vector.
     * @param b The second vector.
     * @return The distance.
     * @throws std::invalid_argument if the vectors are not of the same size.
     */
    virtual float operator()(const T& a, const T& b) const = 0;

    /**
     * @brief Resets the distance function call counter.
     */
    static void resetCounter() {
        distanceFunctionCalls = 0;
    }
};


/**
 * @brief Static member initialization for distance function calls.
 * 
 * @tparam T The type of the elements in the vectors.
 */
template <typename T>
unsigned long int DistanceFunction<T>::distanceFunctionCalls = 0;

/**
 * @brief Class for computing Euclidean distance.
 * 
 * @tparam T The type of the elements in the vectors.
 * 
 * The Euclidean distance between two vectors a and b is defined as:
 * \f[
 * d(a, b) = \sqrt{\sum_{i=1}^{n} (a_i - b_i)^2}
 * \f]
 */
template <typename T>
class EuclideanDistance : public DistanceFunction<T> {
public:
    float operator()(const T& a, const T& b) const override {
        DistanceFunction<T>::distanceFunctionCalls++;
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must be of the same size");
        }

        float sum = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            float diff = a[i] - b[i];
            sum += diff * diff;
        }
        return std::sqrt(sum);
    }
};

/**
 * @brief Class for computing Manhattan distance.
 * 
 * @tparam T The type of the elements in the vectors.
 * 
 * The Manhattan distance between two vectors a and b is defined as:
 * \f[
 * d(a, b) = \sum_{i=1}^{n} |a_i - b_i|
 * \f]
 */
template <typename T>
class ManhattanDistance : public DistanceFunction<T> {
public:
    float operator()(const T& a, const T& b) const override {
        DistanceFunction<T>::distanceFunctionCalls++;
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must be of the same size");
        }

        float sum = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            sum += std::abs(a[i] - b[i]);
        }
        return sum;
    }
};

/**
 * @brief Class for computing Chebyshev distance.
 * 
 * @tparam T The type of the elements in the vectors.
 * 
 * The Chebyshev distance between two vectors a and b is defined as:
 * \f[
 * d(a, b) = \max_{i=1}^{n} |a_i - b_i|
 * \f]
 */
template <typename T>
class ChebyshevDistance : public DistanceFunction<T> {
public:
    float operator()(const T& a, const T& b) const override {
        DistanceFunction<T>::distanceFunctionCalls++;
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must be of the same size");
        }

        float maxDiff = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            float diff = std::abs(a[i] - b[i]);
            if (diff > maxDiff) {
                maxDiff = diff;
            }
        }
        return maxDiff;
    }
};

/**
 * @brief Class for computing Cosine distance.
 * 
 * @tparam T The type of the elements in the vectors.
 * 
 * The Cosine distance between two vectors a and b is defined as:
 * \f[
 * d(a, b) = 1 - \frac{\sum_{i=1}^{n} a_i b_i}{\sqrt{\sum_{i=1}^{n} a_i^2} \sqrt{\sum_{i=1}^{n} b_i^2}}
 * \f]
 */
template <typename T>
class CosineDistance : public DistanceFunction<T> {
public:
    float operator()(const T& a, const T& b) const override {
        DistanceFunction<T>::distanceFunctionCalls++;
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must be of the same size");
        }

        float dotProduct = std::inner_product(a.begin(), a.end(), b.begin(), 0.0f);
        float normA = std::sqrt(std::inner_product(a.begin(), a.end(), a.begin(), 0.0f));
        float normB = std::sqrt(std::inner_product(b.begin(), b.end(), b.begin(), 0.0f));

        // Division by 0 check
        if (normA == 0.0f || normB == 0.0f) {
            std::cerr << "Warning: Division by zero in cosineDistance. Returning maximum distance (1.0)." << std::endl;
            return 1.0f;
        }
        return 1.0f - (dotProduct / (normA * normB));
    }
};

/**
 * @brief Class for computing Normalized Cosine distance.
 * 
 * @tparam T The type of the elements in the vectors.
 * 
 * The Normalized Cosine distance between two normalized vectors a and b is defined as:
 * \f[
 * d(a, b) = 1 - \sum_{i=1}^{n} a_i b_i
 * \f]
 */
template <typename T>
class NormalizedCosineDistance : public DistanceFunction<T> {
public:
    float operator()(const T& a, const T& b) const override {
        DistanceFunction<T>::distanceFunctionCalls++;
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must be of the same size");
        }

        float dotProduct = std::inner_product(a.begin(), a.end(), b.begin(), 0.0f);
        return 1.0f - dotProduct; // Norms are 1, so we only need the dot product
    }
};

#endif // DISTANCES_HPP