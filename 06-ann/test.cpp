// 1. Carregar os dados
#include <iostream>
#include <vector>
#include "objectTypes/Feature.hpp"
#include "data/loadFiles.hpp"
#include "objectTypes/Individual.hpp"

typedef Feature<float> feature;
typedef Individual<float> individual;

int main(){
    // 1. Carregar dados
    // std::vector<feature> galleryObjects = loadData("C:/Users/jfcmp/Documentos/Griaule/data/g1", true);
    // std::vector<feature> queryObjects = loadData("C:/Users/jfcmp/Documentos/Griaule/data/g2", true);

    auto [galleryIndividuals, features] = loadIndividuals("C:/Users/jfcmp/Documentos/Griaule/data/teste1", true);

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
    std::cout << std::endl;

    // 1. Add the modified features to the Indexing structure
    // 2. Create the modified structure for which every comparison will be made
    // with respect to the modified query: distance(meand + query * std, modifiedFeature)

    return 0;
}