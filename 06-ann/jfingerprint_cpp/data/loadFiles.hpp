#ifndef LOADFILE_HPP
#define LOADFILE_HPP

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <ostream>
#include <chrono>
#include <memory>
#include "../objectTypes/Feature.hpp"
#include "../includes/npy.hpp"
#include "../objectTypes/Individual.hpp"

namespace fs = std::filesystem;

typedef Feature<float> feature;

std::vector<feature> loadNpy(std::string filename, bool log_info)
{
    std::vector<feature> dataFeatures;

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<float> data;
    std::vector<unsigned long> shape;

    // Load the data objects from the .npy file
    npy::npy_data<float> d = npy::read_npy<float>(filename);

    data = std::move(d.data);   // Use std::move to avoid copying
    shape = std::move(d.shape); // Use std::move to avoid copying

    if (log_info)
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

    if (log_info)
    {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = (end - start) * 1000;
        std::cout << "Added " << dataFeatures.size() << " data objects\n";
        std::cout << "Time taken to load data: " << duration.count() << " ms\n\n";
    }

    return dataFeatures;
}

std::vector<feature> loadData(const std::string &directoryPath, bool log_info)
{
    std::vector<feature> dataFeatures;
    auto start = std::chrono::high_resolution_clock::now();

    for (const auto &entry : fs::directory_iterator(directoryPath))
    {
        if (entry.path().extension() == ".npy")
        {
            std::vector<feature> fileFeatures = loadNpy(entry.path().string(), false);

            // Adiciona os dados carregados ao vetor principal
            dataFeatures.insert(dataFeatures.end(), fileFeatures.begin(), fileFeatures.end());
        }
    }

    if (log_info){
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = (end - start) * 1000;
        std::cout << "Added " << dataFeatures.size() << " data objects\n";
        std::cout << "Time taken to load data: " << duration.count() << " ms\n\n";
    }

    return dataFeatures;
}

std::pair<std::vector<std::shared_ptr<Individual<float>>>, std::vector<feature>> loadIndividuals(const std::string &directoryPath, bool log_info)
{
    std::vector<std::shared_ptr<Individual<float>>> individuals;
    std::vector<feature> allFeatures;
    auto start = std::chrono::high_resolution_clock::now();

    for (const auto &entry : fs::directory_iterator(directoryPath))
    {
        if (entry.path().extension() == ".npy")
        {
            // For each file, create an individual
            auto individual = std::make_shared<Individual<float>>();
            individual->name = entry.path().filename().string();

            std::vector<feature> fileFeatures = loadNpy(entry.path().string(), false);

            for (auto &f : fileFeatures)
            {
                // Add all the features for the individual
                f.representative = individual.get();
                individual->addFeature(f.getId());
                allFeatures.push_back(f);
            }

            // Calculate the mean and std for the individual
            individual->calculateMean(fileFeatures);
            individual->calculateStd(fileFeatures);
    
            individuals.push_back(individual);
        }
    }

    if (log_info)
    {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = (end - start) * 1000;
        std::cout << "Loaded " << individuals.size() << " individuals\n";
        std::cout << "Added " << allFeatures.size() << " features\n";
        std::cout << "Time taken to load individuals: " << duration.count() << " ms\n\n";
    }

    return std::make_pair(individuals, allFeatures);
}

#endif // LOADFILE_HPP