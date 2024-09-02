import numpy as np
import os
from collections import namedtuple
from scipy.spatial import distance
from mtree import MTree

def L2(a, b):
    return distance.euclidean(a.vec, b.vec)

Feature = namedtuple('Feature', ['vec', 'id_finger', 'id_minutia'])

def find_gallery_associate(j:int, QDS: list[str], GDS: list[str]) -> int:
    # Busca o elemento QDS[j] em GDS e retorna o índice
    for i in range(len(GDS)):
        if QDS[j].split("-")[0] == GDS[i].split("-")[0]:
            break
    return i

def get_features(data:dict) -> np.ndarray:
    features = np.load(data["path"] + data["features_path"] + data["individual"] + ".npy")
    return features

def load_dataset_names(dataset:dict) -> list:
    # Carrega nome de todas as amostras em um dataset 
    # (considerando que todas as amostras tem um arquivo .npy correspondente na pasta features)
    directory_path = dataset["path"] + dataset["features_path"]
    file_names = [f.rstrip('.npy') for f in os.listdir(directory_path) if os.path.isfile(os.path.join(directory_path, f))]
    return file_names

def build_mtree(dataset:dict, distance_function:callable = L2, max_node_size:int = 8, tree_max_size:int = 5000) -> MTree:
    gallery_individuals = load_dataset_names(dataset)
    
    # Controi a MTree
    mtree = MTree(d=distance_function,max_node_size=max_node_size)

    # Para todos individuos da galeria
    for i in range(len(gallery_individuals[:tree_max_size])):
        features = get_features({**dataset, "individual": gallery_individuals[i]})
        
        # Para todas as features do individuo
        for j in range(len(features)):
            mtree.add(Feature(vec=features[j, :], id_finger=i, id_minutia=j))
    
    return mtree

def calculate_score(D:dict, score:callable = None) -> dict:
    if score is None:
        score = lambda item: len(item[1])**(3/4)/(np.mean(item[1]) + .15) 

    Dscore = {}
    for k in D:
        Dscore[k] = {"mean": np.mean(D[k]), "score": score((k, D[k])), "noMatches": len(D[k])} 

    Scores = {}
    for d in Dscore.items():
        finger_id = d[0][0]
        Scores[finger_id] = Scores.get(finger_id, 0) + d[1]['score']
    
    Scores = {k: v for k, v in sorted(Scores.items(), key=lambda item: item[1], reverse=True)}
    return Scores

def gallery_search(mtree:MTree, data:dict, k:int = 8) -> dict:
    # Vai usar todas as features de data para construir um dicionário com sumário de resultados.
    # Constrói o dicionário associando para galeria: (id.finger, id.minutia) -> distância

    features = get_features(data)

    # Fazer queries para todas as features do indivíduo
    D = {}
    for j in range(len(features)):
        query = Feature(vec=features[j, :], id_finger=data["individual"], id_minutia=j)
        result = list(mtree.search(query, k))
        
        # Atribuir a lista com score
        for r in result:
            key = (r.id_finger, r.id_minutia)
            lst = D.get(key, [])
            lst.append(L2(r, query))
            D[key] = lst
    
    return D

def full_search(QDS:list[str], mtree:MTree, queries_dataset:dict, no_queries:int = 10) -> dict:
    assert no_queries <= len(QDS), "Number of queries is greater than the number of queries in the dataset"

    result = {}
    for j in range(no_queries):
        data_q = {**queries_dataset, "individual": QDS[j]}
        D = gallery_search(mtree, data_q)
        Score = calculate_score(D)
        rank = list(Score.keys())
        result[j] = rank[:10]
    
        # Escreve o rank 10 no arquivo, data/full_search.txt
        with open('data/full_search.txt', 'a') as f:
            f.write(f"Query: {j}\n")
            for idx, key in enumerate(rank[:10]):
                f.write(f"\tSample: {key} | Score: {Score[key]:.3f}\n")
            f.write("\n")
    return result

if __name__ == "__main__":
    pass