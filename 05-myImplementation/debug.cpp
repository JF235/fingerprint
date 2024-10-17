#include "MTree.hpp"
#include "distances.hpp"
#include "FloatNumber.hpp"
#include "debug_msg.hpp"

#include <iostream>
#include <vector>
#include <random>

// Function to generate a list of random float numbers
std::vector<float> generateRandomFloats(size_t count, float min, float max) {
    // Create a random device and seed the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);

    // Generate the random float numbers
    std::vector<float> randomFloats;
    randomFloats.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        randomFloats.push_back(dis(gen));
    }

    return randomFloats;
}

int main(){
    // Create vector of FloatNumber objects
    std::vector<FloatNumber> floats;

     std::vector<float> randomFloats = generateRandomFloats(1000, 0.0f, 100.0f);
    for (const auto &element : randomFloats)
    {
        floats.emplace_back(element);
    }

    // Create a tree which element is float and distance function is manhattanDistance
    MTree<FloatNumber> mtree(3, manhattanDistance<FloatNumber>);

    for (const auto &element : floats)
    {
        mtree.insert(element);
    }

    //std::cout << mtree << "\n\n\n";

    // Reset the distance function calls
    distanceFunctionCalls = 0;
    FloatNumber query(0.5);
    NNList<FloatNumber> nnList = mtree.search(query, 3);

    std::cout << nnList << std::endl;
    std::cout << "Number of elements in the tree: " << mtree.size() << std::endl;
    std::cout << "Distance function calls: " << distanceFunctionCalls << std::endl; 

    return 0;
}