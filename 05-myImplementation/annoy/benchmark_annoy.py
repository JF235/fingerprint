import time
import numpy as np
from annoy import AnnoyIndex
import os

def generate_random_vectors(num_vectors, dimension, seed=None):
    if seed is not None:
        np.random.seed(seed)
    return np.random.rand(num_vectors, dimension).astype(np.float32)

def build_annoy_index(vectors, dimension, num_trees=10):
    index = AnnoyIndex(dimension, 'euclidean')
    if not os.path.exists(f'annoy_index{dimension}.ann'):
        start_time = time.time()
        for i, vector in enumerate(vectors):
            index.add_item(i, vector)
        end_time = time.time()
        total_time = (end_time - start_time)
        # Save on disk
        index.build(num_trees)
        index.save(f'annoy_index{dimension}.ann')
    else:
        start_time = time.time()
        index.load(f'annoy_index{dimension}.ann')
        end_time = time.time()
    total_time = (end_time - start_time)
    print(f"Time to build index: {total_time} seconds")
    return index

def benchmark_annoy_knn(index, query_vectors, k):
    results = []
    start_time = time.time()
    for query in query_vectors:
        results.append(
            index.get_nns_by_vector(query, k, include_distances=True)
        )
    end_time = time.time()
    total_time = (end_time - start_time)
    return total_time, results


def format_time(seconds):
    if seconds >= 1:
        return f"{seconds:.6f} s"
    elif seconds >= 1e-3:
        return f"{seconds * 1e3:.6f} ms"
    elif seconds >= 1e-6:
        return f"{seconds * 1e6:.6f} µs"
    else:
        return f"{seconds * 1e9:.6f} ns"
    

def main():
    num_vectors = 1000000
    dimension = 128
    num_queries = 1
    k = 64
    num_trees = 10
    seed = 42

    # Generate random vectors
    data_vectors = generate_random_vectors(num_vectors, dimension, seed)
    query_vectors = generate_random_vectors(num_queries, dimension, seed + 1)

    # Build Annoy index
    index = build_annoy_index(data_vectors, dimension, num_trees)

    # Benchmark KNN search
    total_time, results = benchmark_annoy_knn(index, query_vectors, k)
    avg_time_per_query = total_time / num_queries

    # Format time total_time and get unit
    
    print(f"Total time for {num_queries} queries: {format_time(total_time)}")
    print(f"Average time per query: {format_time(avg_time_per_query)}")
    
    # Perform brute force knn for first query vector
    query = query_vectors[0]
    distances = np.linalg.norm(data_vectors - query, axis = 1)
    brute_force_results = np.argsort(distances)[:k], np.sort(distances)[:k]
    print(np.array(results[0][0]))
    print(brute_force_results[0])

if __name__ == "__main__":
    main()