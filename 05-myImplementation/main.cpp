#include "SequentialSearcher.hpp"
#include "MTree.hpp"
#include "NNList.hpp"
#include "distances.hpp"
#include "FloatNumber.hpp"
#include "debug_msg.hpp"

#include <chrono>
#include <iostream>
#include <vector>
#include <random>

typedef SequentialSearcher<FloatNumber, std::function<double(const FloatNumber &, const FloatNumber &)>> mySeqSearcher;

// Function to generate a list of random float numbers
std::vector<float> generateRandomFloats(size_t count, float min, float max)
{
    // Create a random device and seed the random number generator
    // Set seed to 42
    std::random_device rd;
    std::mt19937 gen(rd());
    gen.seed(2);

    // Create a uniform distribution for the float numbers
    std::uniform_real_distribution<float> dis(min, max);

    // Generate the random float numbers
    std::vector<float> randomFloats;
    randomFloats.reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
        // Approximate to first decimal place and add
        randomFloats.push_back(dis(gen));
    }

    return randomFloats;
}

void testMTree(std::vector<FloatNumber> &floats, int k)
{
    // Create a tree which element is float and distance function is manhattanDistance
    MTree<FloatNumber> mtree(3, manhattanDistance<FloatNumber>);

    int i = 0;
    for (const auto &element : floats)
    {
        if (i == 6)
        {
            std::cout << "Breakpoint\n";
        }
        mtree.insert(element);
        #ifdef KNNDEBUG
        std::cout << mtree << "\n\n\n";
        #endif
        i++;
    }

    // Reset the distance function calls
    distanceFunctionCalls = 0;
    FloatNumber query(0.5);
    NNList<FloatNumber> nnList = mtree.search(query, k);

    std::cout << "Number of elements in the tree: " << mtree.size() << std::endl;
    std::cout << "Distance function calls: " << distanceFunctionCalls << std::endl;
    std::cout << "MTree KNN: " << nnList << std::endl;
}

void testSequentialSearcher(std::vector<FloatNumber> floats, int k)
{
    // Create a sequential searcher with the distance function manhattanDistance
    mySeqSearcher seqSearcher(manhattanDistance<FloatNumber>);

    seqSearcher.addAll(floats);

    // Reset the distance function calls
    distanceFunctionCalls = 0;
    FloatNumber query(0.5);
    NNList<FloatNumber> nnList = seqSearcher.knn(query, k);

    std::cout << "Number of elements in the search structure: " << seqSearcher.size() << std::endl;
    std::cout << "Distance function calls: " << distanceFunctionCalls << std::endl;
    std::cout << "SeqSearch KNN: " << nnList << std::endl;
}

int main()
{

    std::vector<FloatNumber> floats;

    std::vector<float> randomFloats = generateRandomFloats(20, 0.0f, 100.0f);
    //std::vector<float> randomFloats = generateRandomFloats(1000, 0.0f, 100.0f);

    // Reserve space for the FloatNumber objects to avoid multiple reallocations
    floats.reserve(randomFloats.size());
    for (const auto &element : randomFloats)
    {
        floats.emplace_back(element);
    }

    int k = 2;
    testMTree(floats, k);
    testSequentialSearcher(floats, k);
    return 0;
}
