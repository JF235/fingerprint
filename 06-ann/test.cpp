// 1. Carregar os dados
#include <iostream>
#include <vector>
#include "objectTypes/Feature.hpp"
#include "data/loadFiles.hpp"
#include "objectTypes/Individual.hpp"
#include "indexing/ShiftSequentialSearcher.hpp"
#include "indexing/DistanceFunction.hpp"
#include "indexing/KNNResults.hpp"

typedef Feature<float> feature;
typedef Individual<float> individual;
typedef ShiftSequentialSearcher<feature, EuclideanDistance<feature>> ssseacher;

int main(){
    // 1. Carregar dados

    auto [galleryIndividuals, features] = loadIndividuals("C:/Users/jfcmp/Documentos/Griaule/data/teste2", true);

    // For all features apply the formula: mean + val[i] * std
    for (auto &f : features){
        feature mean = f.representative->mean;
        feature std = f.representative->std;
        for (size_t i = 0; i < f.size(); i++){
            f[i] = mean[i] + f[i] * std[i];
        }
    }

    // 2. Add the modified features to the Indexing structure
    EuclideanDistance<feature> distanceFunc;
    ssseacher searcher(distanceFunc);

    searcher.addAll(features);

    // 4. Perform the queries
    std::vector<feature> queries = loadNpy("C:/Users/jfcmp/Documentos/Griaule/data/teste1/queries.npy", true);

    std::vector<NNList<feature>> results;
    for (auto &q : queries){
        NNList<feature> nnList = searcher.knn(q, 3);
        results.push_back(nnList);

        std::cout << "Query: " << q << "\n";
        std::cout << "Results: " << nnList << "\n\n";
    }

    // 4. Create a class to store the results of each query.
    // This class should be able to go through all the KNN results and aggregate the results in a dict {individual: count/score} where the count is the number of times the individual appears in the KNN results.
    
    KNNResult<feature> knnResult(results);

    std::cout << knnResult << "\n\n";

    auto best = knnResult.pickBest(2, "frequency");
    std::cout << "Best: ";
    for (auto &b : best){
        std::cout << b.first << " " << b.second << "; ";
    }
    std::cout << "\n\n";

    auto best2 = knnResult.pickBest(2, "distance");
    std::cout << "Best: ";
    for (auto &b : best2){
        std::cout << b.first << " " << b.second << "; ";
    }
    std::cout << "\n\n";

    return 0;
}