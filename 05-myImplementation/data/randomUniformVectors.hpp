#ifndef RANDOMUNIFORMVECTORS_HPP
#define RANDOMUNIFORMVECTORS_HPP

#include <vector>
#include <random>

/**
 * @brief Generates a random d-dimensional vector uniformly distributed inside a hypervolume.
 * 
 * @tparam T The type of vector to generate.
 * @tparam NumT The type of the elements in the vector.
 * @param d The dimension of the vector.
 * @param gen The random number generator.
 * @param dist The uniform distribution.
 * @return A d-dimensional vector.
 */
template <typename T, typename NumT>
T generateUniformVector(int d, std::mt19937 &gen, std::uniform_real_distribution<NumT> &dist) {
    T vec(d);

    // Generate random components
    for (int i = 0; i < d; ++i) {
        vec[i] = dist(gen);
    }

    return vec;
}

/**
 * @brief Generates N random d-dimensional vectors uniformly distributed inside a hypervolume.
 * 
 * @tparam T The type of the elements in the vectors.
 * @param N The number of vectors.
 * @param d The dimension of each vector.
 * @param seed The seed for the random number generator.
 * @param lower_bound The lower bound of the uniform distribution.
 * @param upper_bound The upper bound of the uniform distribution.
 * @return A vector of N d-dimensional vectors.
 */
template <typename T, typename NumT>
std::vector<T> generateUniformVectors(int N, int d, int seed, NumT lower_bound, NumT upper_bound) {
    std::vector<T> vectors(N);
    std::mt19937 gen(seed);
    std::uniform_real_distribution<NumT> dist(lower_bound, upper_bound);

    for (int i = 0; i < N; ++i) {
        vectors[i] = generateUniformVector<T, NumT>(d, gen, dist);
    }

    return vectors;
}

#endif // RANDOMUNIFORMVECTORS_HPP