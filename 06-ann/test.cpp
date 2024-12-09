// 1. Carregar os dados
#include <iostream>
#include <vector>
#include "objectTypes/Feature.hpp"
#include "data/loadFiles.hpp"
#include "objectTypes/Individual.hpp"
#include "indexing/ShiftSequentialSearcher.hpp"
#include "indexing/DistanceFunction.hpp"

typedef Feature<float> feature;
typedef Individual<float> individual;
typedef ShiftSequentialSearcher<feature, EuclideanDistance<feature>> ssseacher;

int main(){
    // 1. Carregar dados

    auto [galleryIndividuals, features] = loadIndividuals("C:/Users/jfcmp/Documentos/Griaule/data/teste2", true);

    // for (auto &ind : galleryIndividuals)
    // {
    //     ind.print();
    // }

    // For all features print the name of the individual and the id
    // for (auto &f : features)
    // {
    //     std::cout << f.id << " " << f.representative->name << std::endl;
    // }

    // Print f
    // for (auto &f : features) {
    //     f.print();
    // }

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

    std::cout << searcher.size() << std::endl;


    // 3. Create the modified structure for which every comparison will be made
    // with respect to the modified query: distance(meand + query * std, modifiedFeature)
    std::vector<feature> queries = loadNpy("C:/Users/jfcmp/Documentos/Griaule/data/teste1/query.npy", true);

    std::cout << searcher.knn(queries[0], 6);
    

    return 0;
}