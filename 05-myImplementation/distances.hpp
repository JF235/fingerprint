#ifndef DISTANCES_HPP
#define DISTANCES_HPP

#include <iostream> // For std::cerr
#include <vector>
#include <cmath>
#include <numeric> // For std::inner_product
#include <stdexcept> // For std::invalid_argument

unsigned long int distanceFunctionCalls = 0;

/**
 * @brief Computes the Euclidean distance between two vectors.
 * 
 * @tparam T The type of the elements in the vectors.
 * @param a The first vector.
 * @param b The second vector.
 * @return The Euclidean distance.
 * @throws std::invalid_argument if the vectors are not of the same size.
 */
template <typename T>
inline float euclideanDistance(const T& a, const T& b) {
    distanceFunctionCalls++;
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

/**
 * @brief Computes the Manhattan distance between two vectors.
 * 
 * @tparam T The type of the elements in the vectors.
 * @param a The first vector.
 * @param b The second vector.
 * @return The Manhattan distance.
 * @throws std::invalid_argument if the vectors are not of the same size.
 */
template <typename T>
inline float manhattanDistance(const T& a, const T& b) {
    distanceFunctionCalls++;
    if (a.size() != b.size()) {
        throw std::invalid_argument("Vectors must be of the same size");
    }

    float sum = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        sum += std::abs(a[i] - b[i]);
    }
    return sum;
}

/**
 * @brief Computes the Chebyshev distance between two vectors.
 * 
 * @tparam T The type of the elements in the vectors.
 * @param a The first vector.
 * @param b The second vector.
 * @return The Chebyshev distance.
 * @throws std::invalid_argument if the vectors are not of the same size.
 */
template <typename T>
inline float chebyshevDistance(const T& a, const T& b) {
    distanceFunctionCalls++;
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

/**
 * @brief Computes the Cosine distance between two vectors.
 * 
 * @tparam T The type of the elements in the vectors.
 * @param a The first vector.
 * @param b The second vector.
 * @return The Cosine distance.
 * @throws std::invalid_argument if the vectors are not of the same size.
 */
template <typename T>
inline float cosineDistance(const T& a, const T& b) {
    distanceFunctionCalls++;
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

/**
 * @brief Computes the Cosine distance between two normalized vectors.
 * 
 * @tparam T The type of the elements in the vectors.
 * @param a The first normalized vector.
 * @param b The second normalized vector.
 * @return The Cosine distance.
 * @throws std::invalid_argument if the vectors are not of the same size.
 */
template <typename T>
inline float normalizedCosineDistance(const T& a, const T& b) {
    distanceFunctionCalls++;
    if (a.size() != b.size()) {
        throw std::invalid_argument("Vectors must be of the same size");
    }

    float dotProduct = std::inner_product(a.begin(), a.end(), b.begin(), 0.0f);
    return 1.0f - dotProduct; // Norms are 1, so we only need the dot product
}

#endif // DISTANCES_HPP