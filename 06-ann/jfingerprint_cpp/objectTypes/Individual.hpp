#ifndef INDIVIDUAL_HPP
#define INDIVIDUAL_HPP

#include <vector>
#include <cmath>
#include "Feature.hpp"

template <typename NumT>
class Individual {
public:
    /**
     * @brief Default constructor that initializes an empty Individual.
     */
    Individual() : mean(), std() {}

    /**
     * @brief Adds a feature to the Individual.
     * @param featureId ID of the feature to be added.
     */
    void addFeature(uint32_t featureId) {
        features.push_back(featureId);
    }

    /**
     * @brief Calculates the mean feature of the Individual.
     * @param features Vector containing all features.
     */
    void calculateMean(const std::vector<Feature<NumT>>& features) {
        if (features.empty()) return;

        size_t featureSize = features[0].size();
        std::vector<NumT> meanValues(featureSize, 0);

        for (const auto& feature : features) {
            for (size_t i = 0; i < featureSize; ++i) {
                meanValues[i] += feature[i];
            }
        }

        for (size_t i = 0; i < featureSize; ++i) {
            meanValues[i] /= features.size();
        }

        mean = Feature<NumT>(0, meanValues);
    }

    /**
     * @brief Calculates the standard deviation feature of the Individual.
     * @param features Vector containing all features.
     */
    void calculateStd(const std::vector<Feature<NumT>>& features) {
        if (features.empty()) return;

        size_t featureSize = features[0].size();
        std::vector<NumT> stdValues(featureSize, 0);

        for (const auto& feature : features) {
            for (size_t i = 0; i < featureSize; ++i) {
                NumT diff = feature[i] - mean[i];
                stdValues[i] += diff * diff;
            }
        }

        for (size_t i = 0; i < featureSize; ++i) {
            stdValues[i] = std::sqrt(stdValues[i] / features.size());
        }

        std = Feature<NumT>(0, stdValues);
    }

    void print() const {
        std::cout << "Individual: " << name << "\n";
        // std::cout << "Features: ";
        // for (uint32_t featureId : features) {
        //     std::cout << featureId << " ";
        // }
        // std::cout << "\n";

        // Calculate the mean of the means
        float meanOfMeans = 0;
        for (const auto& val : mean.values) {
            meanOfMeans += val;
        }
        meanOfMeans /= mean.values.size();

        // Calculate the mean of the standard deviations
        float meanOfStds = 0;
        for (const auto& val : std.values) {
            meanOfStds += val;
        }
        meanOfStds /= std.values.size();

        // Increase the precision of the output
        std::cout.precision(10);
        std::cout << "Mean of Means: " << meanOfMeans << "\n";
        std::cout << "Mean of Stds: " << meanOfStds << "\n";
    }

    void printInline() const {
        std::cout << name;

        // Calculate the mean of the means
        float meanOfMeans = 0;
        for (const auto& val : mean.values) {
            meanOfMeans += val;
        }
        meanOfMeans /= mean.values.size();

        // Calculate the mean of the standard deviations
        float meanOfStds = 0;
        for (const auto& val : std.values) {
            meanOfStds += val;
        }
        meanOfStds /= std.values.size();

        // Increase the precision of the output
        std::cout.precision(10);
        std::cout << meanOfMeans << "; ";
        std::cout << meanOfStds << "; ";
    }

    void printLong() const {
        // Print 8 decimal places
        std::cout.precision(9);
        std::cout << std::fixed;
        // Print all elements
        std::cout << "Individual: " << name << "\n";
        std::cout << "Features: ";
        for (uint32_t featureId : features) {
            std::cout << featureId << " ";
        }
        std::cout << "\n";

        std::cout << "Mean: ";
        for (const auto& val : mean.values) {
            std::cout << val << " ";
        }
        std::cout << "\n";

        std::cout << "Std: ";
        for (const auto& val : std.values) {
            std::cout << val << " ";
        }
        std::cout << "\n\n";
    }

    std::vector<uint32_t> features; ///< List of feature IDs associated with the Individual
    Feature<NumT> mean;             ///< Mean feature
    Feature<NumT> std;              ///< Standard deviation feature
    std::string name;               ///< Name of the Individual
};

#endif // INDIVIDUAL_HPP