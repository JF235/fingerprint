#include "indexing/SequentialSearcher.hpp"
#include "indexing/NNList.hpp"
#include "indexing/DistanceFunction.hpp"
#include "objectTypes/FloatNumber.hpp"
#include "objectTypes/Feature.hpp"
#include "MTree.hpp"
#include "includes/npy.hpp"
#include "data/randomUnitVectors.hpp"
#include "data/randomUniformVectors.hpp"
#include "data/randomFloats.hpp"
#include "data/loadFiles.hpp"
//#include "annoy/annoylib.h"
//#include "annoy/kissrandom.h"

#include <chrono>
#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <iomanip>

#if defined(UNIT) || defined(UNIFORM) || defined(NPY)
typedef Feature<float> Obj;
#endif

#ifdef FLOAT
typedef FloatNumber Obj;
#endif

std::string format_time(uint64_t microseconds) {
    if (microseconds >= 1e6) {
        return std::to_string(microseconds / 1e6) + " s";
    } else if (microseconds >= 1e3) {
        return std::to_string(microseconds / 1e3) + " ms";
    } else if (microseconds >= 1) {
        return std::to_string(microseconds) + " us";
    } else {
        return std::to_string(microseconds * 1e3) + " ns";
    }
}

typedef SequentialSearcher<Obj, DistanceFunction<Obj>> mySeqSearcher;

// Get time stamp in microseconds.
uint64_t micros()
{
    uint64_t us = std::chrono::duration_cast<std::chrono::microseconds>(
                      std::chrono::high_resolution_clock::now().time_since_epoch())
                      .count();
    return us;
}

#ifdef ANNOY
void testAnnoy(std::vector<Obj> &dataObjects, std::vector<Obj> &queryObjects, int k, int f)
{
    using namespace Annoy;

    // Cria o índice Annoy
    AnnoyIndex<int, float, Angular, Kiss32Random, AnnoyIndexSingleThreadedBuildPolicy> t(f);

    // Adiciona os itens ao índice
    for (size_t i = 0; i < dataObjects.size(); ++i)
    {
        t.add_item(dataObjects[i].getId(), dataObjects[i].values.data());
    }

    // Constrói o índice
    t.build(2 * f);

    // Salva o índice
    t.save("annoy_index.tree");

    // Realiza consultas

    // First convert std::vector<float> to float*

    std::vector<int> closest;
    std::vector<float> distances;
    auto startU = micros();
    for (const auto &query : queryObjects)
    {
        closest.clear();
        distances.clear();
        t.get_nns_by_vector(query.values.data(), k, -1, &closest, &distances);
        // Clear closest and distances
    }
    auto endU = micros();
    std::cout << "Time taken to make query: " << format_time(endU - startU) << std::endl;
    std::cout << "Annoy KNN: [";
    for (int i = 0; i < closest.size(); i++)
    {
        std::cout << "(id:" << closest[i] << ", " << distances[i] << "), ";
    }
    std::cout << "]" << std::endl;
}
#endif

#ifdef TREE
void testMTree(std::vector<Obj> &dataObjects, std::vector<Obj> &queryObjects, int k, DistanceFunction<Obj> &distanceFunction, int nodeSize)
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
    std::cout << "Time taken to build tree: " << duration.count() << " s" << "\n\n";

    distanceFunction.resetCounter();
    // Reset the distance function calls
    auto startU = micros();
    for (const auto &query : queryObjects)
    {
        nnList = mtree.knn(query, k);
    }
    auto endU = micros();

    std::cout << "Number of elements: " << mtree.size() << ", Number of queries: " << queryObjects.size() << std::endl;
    std::cout << "Dimension: " << dataObjects[0].size() << ", k: " << k << "\n\n";
    std::cout << "Total Time: " << format_time(endU - startU) << std::endl;
    std::cout << "Average Time: " << format_time((endU - startU) / queryObjects.size()) << "\n";
    std::cout << "Height: " << mtree.getHeight() << ", Nodes accessed: " << mtree.getNodesAccessed() << "/" << mtree.getTotalNodes() << "(= " << (double)mtree.getNodesAccessed() / mtree.getTotalNodes() * 100 << "%)" << "\n";
    std::cout << "Distance function calls: " << distanceFunction.distanceFunctionCalls << "\n\n";
    // Change fixed point to 4 places
    std::cout << std::fixed << std::setprecision(2);

    std::cout << "KNN[end] result: " << nnList << "\n\n";

}
#endif

#ifdef SEQUENTIAL
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
    std::cout << "Number of elements: " << seqSearcher.size() << ", Number of queries: " << queryObjects.size() << std::endl;
    std::cout << "Dimension: " << dataObjects[0].size() << ", k: " << k << "\n\n";
    std::cout << "Total Time: " << format_time(endU - startU) << std::endl;
    std::cout << "Average Time: " << format_time((endU - startU) / queryObjects.size()) << "\n";
    std::cout << "Distance function calls: " << distanceFunction.distanceFunctionCalls << "\n\n";
    // Change fixed point to 4 places
    std::cout << std::fixed << std::setprecision(2);

    std::cout << "KNN[end] result: " << nnList << "\n\n";
}
#endif

int main(int argc, char *argv[])
{
    int N = 20;
    int querySize = 10;
    int k = 4;
    int seed = 263;
    double maxf = 100.;
    int nodeSize = 64;
    int dimension = 10;

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
        else if (std::string(argv[i]) == "-dimension")
        {
            dimension = std::stoi(argv[i + 1]);
        }
        else if (std::string(argv[i]) == "-maxf")
        {
            maxf = std::stod(argv[i + 1]);
        }
        else if (std::string(argv[i]) == "-h")
        {
            std::cout << "Usage: " << argv[0] << " [-s seed] [-k k] [-N N] [-nodeSize nodeSize] [-querySize querySize] [-dimension dimension] [-maxf maxf]" << std::endl;
            return 0;
        }
    }

#ifdef UNIT
    std::vector<Obj> dataObjects = generateUnitVectors<Obj, float>(N, dimension, seed);
    std::vector<Obj> queryObjects = generateUnitVectors<Obj, float>(querySize, dimension, seed + 1);
#endif

#ifdef UNIFORM
    // generateUniformVectors(int N, int d, int seed, NumT lower_bound, NumT upper_bound)
    std::vector<Obj> dataObjects = generateUniformVectors<Obj, float>(N, dimension, seed, -maxf, maxf);
    std::vector<Obj> queryObjects = generateUniformVectors<Obj, float>(querySize, dimension, seed + 1, -maxf, maxf);
#endif

#ifdef FLOAT
    std::vector<Obj> dataObjects = generateRandomFloats<Obj, float>(N, -maxf, maxf, seed);
    std::vector<Obj> queryObjects = generateRandomFloats<Obj, float>(querySize, -maxf, maxf, seed + 1);
#endif

#ifdef NPY
    std::vector<Obj> dataObjects = loadData(N, "UNIT_100000_128d.npy");
    std::vector<Obj> queryObjects = loadData(querySize, "UNIT_100_128d.npy");
#endif

    EuclideanDistance<Obj> euclideanDistance;

#ifdef SEQUENTIAL
    testSequentialSearcher(dataObjects, queryObjects, k, euclideanDistance);
#endif

#ifdef TREE
    testMTree(dataObjects, queryObjects, k, euclideanDistance, nodeSize);
#endif

#ifdef ANNOY
    testAnnoy(dataObjects, queryObjects, k, dimension);
#endif

    return 0;
}