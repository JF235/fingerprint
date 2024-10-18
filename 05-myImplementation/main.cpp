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
#include <string>

int seed;

typedef SequentialSearcher<FloatNumber, std::function<double(const FloatNumber &, const FloatNumber &)>> mySeqSearcher;

// Function to generate a list of random float numbers
std::vector<float> generateRandomFloats(size_t count, float min, float max)
{
    // Create a random device and seed the random number generator
    // Set seed to 42
    std::random_device rd;
    std::mt19937 gen(rd());
    gen.seed(seed);

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
    MTree<FloatNumber> mtree(32, manhattanDistance<FloatNumber>);

    int i = 0;
    for (const auto &element : floats)
    {
        mtree.insert(element);
        #ifdef INSDEBUG
        std::cout << mtree << "\n\n\n";
        #endif
        i++;
    }

    // Reset the distance function calls
    distanceFunctionCalls = 0;
    FloatNumber query(0.5);
    
    auto start = std::chrono::high_resolution_clock::now();
    NNList<FloatNumber> nnList = mtree.search(query, k);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = (end - start) * 1000;
    std::cout << "Time taken to make query: " << duration.count() << " ms" << std::endl;
    std::cout << "Number of elements in the tree: " << mtree.size() << std::endl;
    std::cout << "Distance function calls: " << distanceFunctionCalls << std::endl;
    std::cout << "MTree KNN: " << nnList << "\n\n\n";
}

void testSequentialSearcher(std::vector<FloatNumber> floats, int k)
{
    // Create a sequential searcher with the distance function manhattanDistance
    mySeqSearcher seqSearcher(manhattanDistance<FloatNumber>);

    seqSearcher.addAll(floats);

    // Reset the distance function calls
    distanceFunctionCalls = 0;
    FloatNumber query(0.5);

    auto start = std::chrono::high_resolution_clock::now();
    NNList<FloatNumber> nnList = seqSearcher.knn(query, k);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = (end - start) * 1000;
    std::cout << "Time taken to make query: " << duration.count() << " ms" << std::endl;
    std::cout << "Number of elements in the search structure: " << seqSearcher.size() << std::endl;
    std::cout << "Distance function calls: " << distanceFunctionCalls << std::endl;
    std::cout << "SeqSearch KNN: " << nnList << std::endl;
}


int main(int argc, char *argv[])
{
    seed = 263;
    int k = 4;
    int N = 20;

    for (int i = 1; i < argc; i+=2)
    {
        if (std::string(argv[i]) == "-s")
        {
            seed = std::stoi(argv[i + 1]);
        }
        else if (std::string(argv[i]) == "-k")
        {
            k = std::stoi(argv[i + 1]);
        }
        else if (std::string(argv[i]) == "-N")
        {
            N = std::stoi(argv[i + 1]);
        }
    }
    


    std::vector<FloatNumber> floats;

    std::vector<float> randomFloats = generateRandomFloats(N, 0.0f, 1000.0f);
    //std::vector<float> randomFloats = generateRandomFloats(1000, 0.0f, 100.0f);

    // Reserve space for the FloatNumber objects to avoid multiple reallocations
    floats.reserve(randomFloats.size());
    for (const auto &element : randomFloats)
    {
        floats.emplace_back(element);
    }

    testMTree(floats, k);
    testSequentialSearcher(floats, k);
    return 0;
}
