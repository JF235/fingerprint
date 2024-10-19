#ifndef RANDOMUNITVECTORS_HPP
#define RANDOMUNITVECTORS_HPP

#include <vector>
#include <random>
#include <cmath>

/**
 * @brief Generates a random d-dimensional unit vector.
 * 
 * @tparam T The type of vector to generate.
 * @tparam NumT The type of the elements in the vector.
 * @param d The dimension of the vector.
 * @param gen The random number generator.
 * @param dist The normal distribution.
 * @return A d-dimensional unit vector.
 */
template <typename T, typename NumT>
T generateUnitVector(int d, std::mt19937 &gen, std::normal_distribution<> &dist) {
    T vec(d);
    NumT norm = 0.0;

    // Generate random components
    for (int i = 0; i < d; ++i) {
        vec[i] = dist(gen);
        norm += vec[i] * vec[i];
    }

    // Normalize the vector
    norm = std::sqrt(norm);
    for (int i = 0; i < d; ++i) {
        vec[i] /= norm;
    }

    return vec;
}

/**
 * @brief Generates N random d-dimensional unit vectors.
 * 
 * @tparam T The type of the elements in the vectors.
 * @param N The number of vectors.
 * @param d The dimension of each vector.
 * @return A vector of N d-dimensional unit vectors.
 */
template <typename T, typename NumT>
std::vector<T> generateUnitVectors(int N, int d, int seed) {
    std::vector<T> vectors(N);
    std::mt19937 gen(seed);
    std::normal_distribution<> dist(0.0, 1.0);

    for (int i = 0; i < N; ++i) {
        vectors[i] = generateUnitVector<T, NumT>(d, gen, dist);
    }

    return vectors;
}

#endif // RANDOMUNITVECTORS_HPP