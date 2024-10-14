#include <iostream>
#include <chrono>
#include "distances.hpp"
#include "feature.hpp"
#include "NNList.hpp"
#include "npy.hpp"
#include "SequentialSearcher.hpp"

typedef SequentialSearcher<feature, std::function<float(const feature&, const feature&)>> mySeqSearcher;

std::vector<feature> dataObjects;
std::vector<feature> queryObjects;

void loadQueries(const std::string& fileName)
{
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<float> data;
    std::vector<unsigned long> shape;

    // Load the query objects from the .npy file
    npy::npy_data<float> d = npy::read_npy<float>(fileName);
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

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = (end - start) * 1000;
    std::cout << "Added " << queryObjects.size() << " query objects\n";
    std::cout << "Time taken to load queries: " << duration.count() << " ms\n\n";
}

void loadData(const std::string& fileName)
{
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<float> data;
    std::vector<unsigned long> shape;


    // Load the data objects from the .npy file
    npy::npy_data<float> d = npy::read_npy<float>(fileName);
    data = std::move(d.data); // Use std::move to avoid copying
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

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = (end - start) * 1000;
    std::cout << "Added " << dataObjects.size() << " data objects\n";
    std::cout << "Time taken to load data: " << duration.count() << " ms\n\n";
}

int main() {
    std::string queries = "C:\\Users\\jfcmp\\Documentos\\fingerprint\\04-comparingIndexing\\datasets\\random_unit_vectors_16d_queries.npy";
    std::string data = "C:\\Users\\jfcmp\\Documentos\\fingerprint\\04-comparingIndexing\\datasets\\random_unit_vectors_16d.npy";

    NNList<feature> nn;

    loadQueries(queries);
    loadData(data);

    mySeqSearcher seqsearcher(euclideanDistance<feature>);

    seqsearcher.addAll(dataObjects);

    std::cout << seqsearcher.size() << "\n";

    auto start = std::chrono::high_resolution_clock::now();
    // For all queries
    for (const auto& query : queryObjects)
    {
        nn = seqsearcher.knn(query, 32);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = (end - start) * 1000;
    std::cout << "Time taken to single knn: " << duration.count() << " ms\n\n";

    std::cout << nn << "\n";

    // feature f1(1, {0.f, 0.f, 0.f});
    // feature f2(2, {3.0f, 4.0f, 0.0f});

    // std::cout << f1 << std::endl;
    // std::cout << f2 << std::endl;

    // std::cout << "Euclidean Distance: " << euclideanDistance(f1, f2) << std::endl;
    // std::cout << "Manhattan Distance: " << manhattanDistance(f1, f2) << std::endl;
    // std::cout << "Chebyshev Distance: " << chebyshevDistance(f1, f2) << std::endl;
    // std::cout << "Cosine Distance: " << cosineDistance(f1, f2) << std::endl;

    // size_t k = 2;

    // NNList<feature> nnList(k);

    // nnList.insert(f1, 1.0);
    // nnList.insert(f2, -1.0);

    // std::cout << nnList << std::endl;

    

    // // Create vector with features
    // std::vector<feature> features;
    // features.push_back(f1);
    // features.push_back(f2);

    // // Add features to the SequentialSearcher
    // seqsearcher.addAll(features);

    // std::cout << seqsearcher << std::endl;

    // feature f3(3, {1.0f, 1.0f, 0.0f});

    // seqsearcher.add(f3);

    // std::cout << seqsearcher << std::endl;


    return 0;
}