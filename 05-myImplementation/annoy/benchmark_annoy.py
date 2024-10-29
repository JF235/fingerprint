import time
import numpy as np
from annoy import AnnoyIndex
import argparse
import os

def generate_random_unit_vectors(num_vectors, dimension, seed=None):
    if seed is not None:
        np.random.seed(seed)
    vectors = np.random.randn(num_vectors, dimension).astype(np.float32)
    vectors /= np.linalg.norm(vectors, axis=1)[:, np.newaxis]
    # Save vectors to disk
    np.save(f"UNIT_{num_vectors}_{dimension}d.npy", vectors)
    return vectors

def generate_random_vectors(num_vectors, dimension, seed=None):
    if seed is not None:
        np.random.seed(seed)
    return np.random.rand(num_vectors, dimension).astype(np.float32)

def build_annoy_index(vectors, dimension, num_trees):
    # Check if index file exists
    if not os.path.exists("annoy_index.ann"):
        index = AnnoyIndex(dimension, 'angular')
        start_time = time.time()
        for i, vector in enumerate(vectors):
            index.add_item(i, vector)
            # Print for each 10%
            if i % (len(vectors) // 10) == 0:
                print(f"Added {i} vectors to index")
        index.build(num_trees)
        end_time = time.time()
        print(f"Built Annoy index with {len(vectors)} vectors in {end_time - start_time:.6f} s")
        # Save
        index.save("annoy_index.ann")
    else:
        index = AnnoyIndex(dimension, 'angular')
        index.load("annoy_index.ann")
    return index

def benchmark_annoy_knn(index, query_vectors, k, search_k):
    total_time = 0
    distance_calls = np.NaN
    start_time = time.time()
    for query in query_vectors:
        if search_k == -1:
            search_k = k * index.get_n_trees()
        result, distances = index.get_nns_by_vector(query, k, search_k=search_k, include_distances=True)
    end_time = time.time()
    total_time += (end_time - start_time)
    # distance_calls += len(result) * len(query_vectors)
    return total_time, result, distances

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
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="Benchmark Annoy KNN search.")
    parser.add_argument("-N", type=int, default=10000, help="Number of vectors")
    parser.add_argument("-Q", "--querySize", type=int, default=100, help="Number of queries")
    parser.add_argument("-K", "--k", type=int, default=10, help="Number of nearest neighbors")
    parser.add_argument("-S", "--seed", type=int, default=42, help="Random seed")
    parser.add_argument("-M", "--maxf", type=float, default=10.0, help="Max value for uniform distribution")
    parser.add_argument("-D", "--dimension", type=int, default=40, help="Dimension of vectors")
    parser.add_argument("-T", "--numTrees", type=int, default=10, help="Number of trees in Annoy index")
    parser.add_argument("-O", "--nodeSize", type=int, default=64, help="Node size (not used in Annoy)")
    parser.add_argument("-H", "--searchK", type=int, default=-1, help="Search K for annoy")
    parser.add_argument("--dataset", type=str, default="UNIT", help="Dataset type")
    args = parser.parse_args()

    # Generate random vectors or load from disk
    if args.dataset == "UNIFORM":
        data_vectors = generate_random_vectors(args.N, args.dimension, args.seed)
        query_vectors = generate_random_vectors(args.querySize, args.dimension, args.seed + 1)
    elif args.dataset == "UNIT":
        data_vectors = generate_random_unit_vectors(args.N, args.dimension, args.seed)
        query_vectors = generate_random_unit_vectors(args.querySize, args.dimension, args.seed + 1)
    elif args.dataset == "NPY":
        data_vectors = np.load("UNIT_10000_128d.npy")
        query_vectors = np.load("UNIT_10_128d.npy")
        data_vectors = data_vectors[:args.N]
        query_vectors = query_vectors[:args.querySize]
    
    # Build Annoy index
    index = build_annoy_index(data_vectors, args.dimension, args.numTrees)

    # Benchmark KNN search
    total_time, result, distances = benchmark_annoy_knn(index, query_vectors, args.k, args.searchK)
    avg_time_per_query = total_time / args.querySize

    # Format time total_time and get unit
    formatted_total_time = format_time(total_time)
    formatted_avg_time_per_query = format_time(avg_time_per_query)
    benchmark_message = (
        f"Dataset: {args.dataset}, Structure: ANNOY, N: {args.N}, querySize: {args.querySize}, K: {args.k}, S: {args.seed}, M: {args.maxf}, dimension: {args.dimension}, NS: {args.nodeSize}\n"
        f"Number of elements: {args.N}, Number of queries: {args.querySize}\n"
        f"Dimension: {args.dimension}, k: {args.k}\n\n"
        f"Total Time: {formatted_total_time}\n"
        f"Average Time: {formatted_avg_time_per_query}\n"
        f"Distance function calls: {np.NaN}\n"
    )
    print(benchmark_message)
    print(f"KNN[end] result: {[(id, f'{dist:.2f}') for id, dist in zip(result, distances)]}")

if __name__ == "__main__":
    main()