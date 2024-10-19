#ifndef RANDOMFLOAT_HPP
#define RANDOMFLOAT_HPP

#include <vector>
#include <random>

// Function to generate a list of random float numbers
template <typename T, typename NumT>
std::vector<T> generateRandomFloats(size_t count, float min, float max, int seed)
{
    // Create a random device and seed the random number generator
    // Set seed to 42
    std::random_device rd;
    std::mt19937 gen(rd());
    gen.seed(seed);

    // Create a uniform distribution for the float numbers
    std::uniform_real_distribution<NumT> dis(min, max);

    // Generate the random float numbers
    std::vector<T> randomFloats;
    randomFloats.reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
        // Approximate to first decimal place and add
        randomFloats.push_back(dis(gen));
    }

    return randomFloats;
}

#endif // RANDOMFLOAT_HPP