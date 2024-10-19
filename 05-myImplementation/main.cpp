#include "indexing/SequentialSearcher.hpp"
#include "indexing/NNList.hpp"
#include "indexing/DistanceFunction.hpp"
#include "objectTypes/FloatNumber.hpp"
#include "objectTypes/Feature.hpp"
#include "MTree.hpp"
#include "includes/npy.hpp"
#include "data/randomUnitVectors.hpp"
#include "data/randomFloats.hpp"

#include <chrono>
#include <iostream>
#include <vector>
#include <random>
#include <string>

int seed;
size_t nodeSize;

#ifdef FEATURE
typedef Feature<float> Obj;
#endif

#ifdef FLOAT
typedef FloatNumber Obj;
#endif

typedef SequentialSearcher<Obj, DistanceFunction<Obj>> mySeqSearcher;


// Get time stamp in microseconds.
uint64_t micros()
{
    uint64_t us = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
            .count();
    return us; 
}


void testMTree(std::vector<Obj> &dataObjects, std::vector<Obj> &queryObjects, int k, DistanceFunction<Obj> &distanceFunction)
{
    // Create a tree which element is float and distance function is manhattanDistance
    MTree<Obj> mtree(nodeSize, distanceFunction);
    NNList<Obj> nnList(0);

    int i = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto &element : dataObjects)
    {
        mtree.insert(element);
        i++;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = (end - start);
    std::cout << "Time taken to build tree: " << duration.count() << " s" << std::endl;

    distanceFunction.resetCounter();
    // Reset the distance function calls
    auto startU = micros();
    for (const auto &query : queryObjects)
    {
        nnList = mtree.knn(query, k);
    }
    auto endU = micros();
    std::cout << "Time taken to make query: " << endU - startU << " us" << std::endl;
    std::cout << "Number of elements in the tree: " << mtree.size() << ", Height: " << mtree.getHeight() << ", Nodes accessed: " << mtree.getNodesAccessed() << std::endl;
    std::cout << "Distance function calls: " << distanceFunction.distanceFunctionCalls << std::endl;
    std::cout << "MTree KNN: " << nnList << "\n\n\n";
}

void testSequentialSearcher(std::vector<Obj> &dataObjects, std::vector<Obj> &queryObjects, int k, DistanceFunction<Obj> &distanceFunction)
{
    NNList<Obj> nnList(0);
    // Create a sequential searcher with the distance function manhattanDistance
    mySeqSearcher seqSearcher(distanceFunction);

    seqSearcher.addAll(dataObjects);

    // Reset the distance function calls
    distanceFunction.resetCounter();

    auto startU = micros();
    // Go through all the query objects
    for (const auto &query : queryObjects)
    {
        nnList = seqSearcher.knn(query, k);
    }
    auto endU = micros();
    std::cout << "Time taken to make query: " << endU - startU << " us" << std::endl;
    std::cout << "Number of elements in the search structure: " << seqSearcher.size() << std::endl;
    std::cout << "Distance function calls: " << distanceFunction.distanceFunctionCalls << std::endl;
    std::cout << "SeqSearch KNN: " << nnList << std::endl;
}


int main(int argc, char *argv[])
{
    seed = 263;
    nodeSize = 64;
    int k = 4;
    int N = 20;
    int querySize = 10;
    int dimension = 10;
    double maxf = 100.;

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
        #ifdef FLOAT
        else if (std::string(argv[i]) == "-maxf")
        {
            maxf = std::stod(argv[i + 1]);
        }
        #endif
    }

    #ifdef FEATURE
    std::vector<Obj> dataObjects = generateUnitVectors<Obj, float>(N, dimension, seed);
    std::vector<Obj> queryObjects = generateUnitVectors<Obj, float>(querySize, dimension, seed + 1);
    #endif

    #ifdef FLOAT
    std::vector<Obj> dataObjects = generateRandomFloats<Obj, float>(N, -maxf, maxf, seed);
    std::vector<Obj> queryObjects = generateRandomFloats<Obj, float>(querySize, -maxf, maxf, seed + 1);
    #endif

    EuclideanDistance<Obj> euclideanDistance;

    testSequentialSearcher(dataObjects, queryObjects, k, euclideanDistance);
    testMTree(dataObjects, queryObjects, k, euclideanDistance);
    
    return 0;
}