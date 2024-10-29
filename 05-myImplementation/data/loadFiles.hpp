#ifndef LOADFILE_HPP
#define LOADFILE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <ostream>
#include <chrono>
#include "../objectTypes/Feature.hpp"
#include "../includes/npy.hpp"

typedef Feature<float> feature;

std::vector<feature> loadQueries(size_t N, std::string filename)
{
    std::vector<feature> queryFeatureects;

    // auto start = std::chrono::high_resolution_clock::now();
    std::vector<float> data;
    std::vector<unsigned long> shape;

    // Load the query objects from the .npy file
    npy::npy_data<float> d = npy::read_npy<float>(filename);
    // Use std::move to avoid copying
    data = std::move(d.data);
    shape = std::move(d.shape);

    // std::cout << "Loading query objects with shape: " << shape[0] << "x" << shape[1] << "\n";

    // Reserve space for queryFeatureects to avoid multiple reallocations
    queryFeatureects.reserve(shape[0]);

    // Go through the lines of matrix
    for (uint32_t i = 0; i < shape[0]; i++)
    {
        // Use iterators to avoid copying data to a new vector
        // Iterator is vector<float>::iterator
        auto startIt = data.begin() + i * shape[1];
        auto endIt = startIt + shape[1];
        queryFeatureects.emplace_back(i, std::vector<float>(startIt, endIt));
    }

    // Select only the first N elements
    if (N < queryFeatureects.size())
    {
        queryFeatureects.resize(N);
    }
    else
    {
        std::cout << "Warning: N is greater than the number of data objects\n";
    }

    // auto end = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> duration = (end - start) * 1000;
    // std::cout << "Added " << queryFeatureects.size() << " query objects\n";
    // std::cout << "Time taken to load queries: " << duration.count() << " ms\n\n";
    return queryFeatureects;
}

std::vector<feature> loadData(size_t N, std::string filename)
{
    std::vector<feature> dataFeatures;

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<float> data;
    std::vector<unsigned long> shape;

    // Load the data objects from the .npy file
    npy::npy_data<float> d = npy::read_npy<float>(filename);
    data = std::move(d.data);   // Use std::move to avoid copying
    shape = std::move(d.shape); // Use std::move to avoid copying

    std::cout << "Loading data objects with shape: " << shape[0] << "x" << shape[1] << "\n";

    // Reserve space for dataFeatures to avoid multiple reallocations
    dataFeatures.reserve(shape[0]);
    // Go through the lines of matrix
    for (uint32_t i = 0; i < shape[0]; i++)
    {
        // Use iterators to avoid copying data to a new vector
        auto startIt = data.begin() + i * shape[1];
        auto endIt = startIt + shape[1];

        dataFeatures.emplace_back(std::vector<float>(startIt, endIt));
    }

    // Select only the first N elements
    if (N <= dataFeatures.size())
    {
        dataFeatures.resize(N);
    }
    else
    {
        std::cout << "Warning: N is greater than the number of data objects\n";
    }

    // auto end = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> duration = (end - start) * 1000;
    // std::cout << "Added " << dataFeatures.size() << " data objects\n";
    // std::cout << "Time taken to load data: " << duration.count() << " ms\n\n";

    return dataFeatures;
}

#endif // LOADFILE_HPP