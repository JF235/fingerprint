import numpy as np
from annoy import AnnoyIndex
import time
import os

def random_unit_vectors(num_vectors, n):
    # Gera uma matriz de vetores com distribuição normal (gaussiana)
    vectors = np.random.normal(0, 1, (num_vectors, n))
    norm = np.linalg.norm(vectors, axis = 1)
    for i in np.where(norm != 1.0)[0]:
        vectors[i] = vectors[i] / np.linalg.norm(vectors[i])
    vectors = vectors.astype(np.float32)
    norm = np.linalg.norm(vectors, axis = 1)
    for i in np.where(norm != 1.0)[0]:
        vectors[i] = vectors[i] / np.linalg.norm(vectors[i])
    
    return vectors

def L2(query, features):
    # Compute L2 distance between query and features
    return np.sqrt(np.sum((query - features)**2, axis=1))

def knn(query, k, features, distFunc = None, tie = 0):
    """
    Realiza uma busca k-NN (k-Nearest Neighbors) utilizando uma função de distância personalizada.

    Parameters
    ----------
    query : np.ndarray
        O vetor de consulta para o qual os vizinhos mais próximos serão encontrados.
    k : int
        O número de vizinhos mais próximos a serem retornados.
    features : np.ndarray
        Um array 2D onde cada linha é um vetor de características.
    distFunc : callable, optional
        A função de distância a ser utilizada. Se None, a função L2 (distância Euclidiana) será utilizada (default é None).
    tie : int, optional
        Se 1, verifica se há mais características com a mesma distância que o k-ésimo vizinho mais próximo e as inclui no resultado (default é 0).

    Returns
    -------
    list of tuple
        Uma lista de tuplas onde cada tupla contém o índice e a distância do vizinho mais próximo no array de características.
        A lista é ordenada pela distância em ordem crescente.

    Notes
    -----
    Em caso de empate (distâncias iguais) e se `tie` for 1, todos os vizinhos com a mesma distância que o k-ésimo vizinho mais próximo são incluídos no resultado.
    """
    if distFunc is None:
        distFunc = L2
    
    # Vetor com distâncias
    distances = distFunc(query, features)
    k_nearest = []
     
    for i in range(k):
        min_idx = np.argmin(distances)
        k_nearest.append((min_idx, distances[min_idx]))
        distances[min_idx] = np.inf
        
    # Check if there are more features with the same distance
    if tie:
        min_idx = np.argmin(distances)
        while distances[min_idx] == k_nearest[-1][1]:
            k_nearest.append((min_idx, distances[min_idx]))
            distances[min_idx] = np.inf
            min_idx = np.argmin(distances)
    
    return k_nearest

for n in [4, 8, 16, 32, 64, 128]:
    # Gerar vetores
    num_vectors = 100000
    np.random.seed(42)
    random_vectors = random_unit_vectors(num_vectors, n)

    # Gerar queries
    np.random.seed(235)
    list_of_queries = random_unit_vectors(80, n)

    annoy = AnnoyIndex(n, 'angular')

    if os.path.exists(f'datasets/annoy{n}.ann'):
        annoy.load(f'datasets/annoy{n}.ann')
    else:
        print(f"Building {n}...")
        for i in range(random_vectors.shape[0]):
            annoy.add_item(i, random_vectors[i, :])
        annoy.build(n_trees=500)
        annoy.save(f'datasets/annoy{n}.ann')

    t0 = time.time()
    for query in list_of_queries:
        indices = annoy.get_nns_by_vector(query, 32,include_distances=False)
    t1 = time.time()  
    dt = (t1 - t0) * 1000
    print(f"Annoy{n}: {dt:.2f} ms")
    indices, distances = annoy.get_nns_by_vector(list_of_queries[0], 32,include_distances=True)
    print([(i, d) for i, d in zip(indices, distances)])
    print("\n")