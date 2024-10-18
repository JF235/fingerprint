#include "SequentialSearcher.hpp"
#include "MTree.hpp"
#include "NNList.hpp"
#include "distances.hpp"
#include "FloatNumber.hpp"
#include "debug_msg.hpp"
#include "npy.hpp"
#include "feature.hpp"

#include <chrono>
#include <iostream>
#include <vector>
#include <random>
#include <string>

int seed;
size_t nodeSize;

#ifdef FEATURE
typedef feature Obj;
int dimension = 8;
#endif

#ifdef FLOAT
typedef FloatNumber Obj;
double maxf = 1000000.0;
#endif

typedef SequentialSearcher<Obj, std::function<double(const Obj &, const Obj &)>> mySeqSearcher;


// Get time stamp in microseconds.
uint64_t micros()
{
    uint64_t us = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
            .count();
    return us; 
}

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

void testMTree(std::vector<Obj> &dataObjects, std::vector<Obj> &queryObjects, int k, std::function<double(const Obj &, const Obj &)> distanceFunction)
{
    // Create a tree which element is float and distance function is manhattanDistance
    MTree<Obj> mtree(nodeSize, distanceFunction);
    NNList<Obj> nnList(0);

    int i = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto &element : dataObjects)
    {
        mtree.insert(element);
#ifdef INSDEBUG
        std::cout << mtree << "\n\n\n";
#endif
        i++;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = (end - start);
    std::cout << "Time taken to build tree: " << duration.count() << " s" << std::endl;


    // Reset the distance function calls
    distanceFunctionCalls = 0;

    auto startU = micros();
    for (const auto &query : queryObjects)
    {
        nnList = mtree.search(query, k);
    }
    auto endU = micros();
    std::cout << "Time taken to make query: " << endU - startU << " us" << std::endl;
    std::cout << "Number of elements in the tree: " << mtree.size() << ", Height: " << mtree.getHeight() << ", Nodes accessed: " << mtree.getNodesAccessed() << std::endl;
    std::cout << "Distance function calls: " << distanceFunctionCalls << std::endl;
    std::cout << "MTree KNN: " << nnList << "\n\n\n";
}

void testSequentialSearcher(std::vector<Obj> &dataObjects, std::vector<Obj> &queryObjects, int k, std::function<double(const Obj &, const Obj &)> distanceFunction)
{
    NNList<Obj> nnList(0);
    // Create a sequential searcher with the distance function manhattanDistance
    mySeqSearcher seqSearcher(distanceFunction);

    seqSearcher.addAll(dataObjects);

    // Reset the distance function calls
    distanceFunctionCalls = 0;

    auto startU = micros();
    // Go through all the query objects
    for (const auto &query : queryObjects)
    {
        nnList = seqSearcher.knn(query, k);
    }
    auto endU = micros();
    std::cout << "Time taken to make query: " << endU - startU << " us" << std::endl;
    std::cout << "Number of elements in the search structure: " << seqSearcher.size() << std::endl;
    std::cout << "Distance function calls: " << distanceFunctionCalls << std::endl;
    std::cout << "SeqSearch KNN: " << nnList << std::endl;
}

std::vector<Obj> loadQueries(size_t N)
{
    std::vector<Obj> queryObjects;

    #ifdef FLOAT
        std::vector<float> randomFloats = generateRandomFloats(N, 0.0f, maxf);

        queryObjects.reserve(randomFloats.size());
        for (const auto &element : randomFloats)
        {
            queryObjects.emplace_back(element);
        }
    #endif
    #ifdef FEATURE
        std::string filename = "C:\\Users\\jfcmp\\Documentos\\fingerprint\\04-comparingIndexing\\datasets\\random_unit_vectors_" + std::to_string(dimension) + "d_queries.npy";

        auto start = std::chrono::high_resolution_clock::now();
        std::vector<float> data;
        std::vector<unsigned long> shape;

        // Load the query objects from the .npy file
        npy::npy_data<float> d = npy::read_npy<float>(filename);
        // Use std::move to avoid copying
        data = std::move(d.data);
        shape = std::move(d.shape);

        std::cout << "Loading query objects with shape: " << shape[0] << "x" << shape[1] << "\n";

        // Reserve space for queryObjects to avoid multiple reallocations
        queryObjects.reserve(shape[0]);

        // Go through the lines of matrix
        for (uint32_t i = 0; i < shape[0]; i++)
        {
            // Use iterators to avoid copying data to a new vector
            // Iterator is vector<float>::iterator
            auto startIt = data.begin() + i * shape[1];
            auto endIt = startIt + shape[1];
            queryObjects.emplace_back(i, std::vector<float>(startIt, endIt));
        }

        // Select only the first N elements
        if (N < queryObjects.size())
        {
            queryObjects.resize(N);
        } else {
            std::cout << "Warning: N is greater than the number of data objects\n";
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = (end - start) * 1000;
        std::cout << "Added " << queryObjects.size() << " query objects\n";
        std::cout << "Time taken to load queries: " << duration.count() << " ms\n\n";
    #endif
    return queryObjects;
}

std::vector<Obj> loadData(size_t N)
{
    std::vector<Obj> dataObjects;

    #ifdef FLOAT
        std::vector<float> randomFloats = generateRandomFloats(N, 0.0f, maxf);

        dataObjects.reserve(randomFloats.size());
        for (const auto &element : randomFloats)
        {
            dataObjects.emplace_back(element);
        }
    #endif
    #ifdef FEATURE
        std::string filename = "C:\\Users\\jfcmp\\Documentos\\fingerprint\\04-comparingIndexing\\datasets\\random_unit_vectors_" + std::to_string(dimension) + "d.npy";
        auto start = std::chrono::high_resolution_clock::now();
        std::vector<float> data;
        std::vector<unsigned long> shape;

        // Load the data objects from the .npy file
        npy::npy_data<float> d = npy::read_npy<float>(filename);
        data = std::move(d.data);   // Use std::move to avoid copying
        shape = std::move(d.shape); // Use std::move to avoid copying

        std::cout << "Loading data objects with shape: " << shape[0] << "x" << shape[1] << "\n";

        // Reserve space for dataObjects to avoid multiple reallocations
        dataObjects.reserve(shape[0]);

        // Go through the lines of matrix
        for (uint32_t i = 0; i < shape[0]; i++)
        {
            // Use iterators to avoid copying data to a new vector
            auto startIt = data.begin() + i * shape[1];
            auto endIt = startIt + shape[1];
            dataObjects.emplace_back(i, std::vector<float>(startIt, endIt));
        }

        // Select only the first N elements
        if (N < dataObjects.size())
        {
            dataObjects.resize(N);
        } else {
            std::cout << "Warning: N is greater than the number of data objects\n";
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = (end - start) * 1000;
        std::cout << "Added " << dataObjects.size() << " data objects\n";
        std::cout << "Time taken to load data: " << duration.count() << " ms\n\n";
    #endif

    return dataObjects;
}

int main(int argc, char *argv[])
{
    seed = 263;
    nodeSize = 64;
    int k = 4;
    int N = 20;
    int querySize = 10;

    for (int i = 1; i < argc; i += 2)
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
        else if (std::string(argv[i]) == "-nodeSize")
        {
            nodeSize = std::stoi(argv[i + 1]);
        }
        else if (std::string(argv[i]) == "-querySize")
        {
            querySize = std::stoi(argv[i + 1]);
        }
        #ifdef FEATURE
        else if (std::string(argv[i]) == "-dimension")
        {
            dimension = std::stoi(argv[i + 1]);
        }
        #endif
    }

    std::vector<Obj> dataObjects;
    std::vector<Obj> queryObjects;

    queryObjects = loadQueries(querySize); // type 0 for float 1 for feature
    dataObjects = loadData(N);     // type 0 for float 1 for feature

    testMTree(dataObjects, queryObjects, k, euclideanDistance<Obj>);
    testSequentialSearcher(dataObjects, queryObjects, k, euclideanDistance<Obj>);
    return 0;
}

// TODO: Arrumar as funções de split