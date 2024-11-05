import sys
import os
import numpy as np

# Add the parent directory to sys.path to locate the jfingerprint package
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

import jfingerprint as jf
from jfingerprint.indexing import AnnoySearcher
from jfingerprint.printing import format_time

N = 10000
Q = 40
d = 128
k = 3

def main_annoy(N, Q, d, k):
    np.random.seed(42)
    
    data = jf.data.generate_unit_vectors(N, d)
    queries = jf.data.generate_unit_vectors(Q, d)

    n_trees = 10 * d
    distance_metric = 'euclidean'
    search_k =  20000

    if os.path.exists(f'build/annoy_index_{d}.ann') and os.path.exists(f'build/annoy_index_{d}.pkl'):
        # Load the Annoy index from the binary file
        annoy_searcher = AnnoySearcher(f=d)
        annoy_searcher.load(f'build/annoy_index_{d}.ann')
        print("Annoy index loaded successfully.")
    else:
        # Instantiate the AnnoySearcher
        annoy_searcher = AnnoySearcher(f=d, distance_metric=distance_metric)

        # Build the Annoy index with timing enabled
        annoy_searcher.build(data, n_trees=n_trees, log_time=True)
    
        # Save the Annoy index to a binary file
        annoy_searcher.save(f'build/annoy_index_{d}.ann')
        print("Annoy index built and saved successfully.")

    nearest_indices, nearest_distances = annoy_searcher.search(queries, k, search_k=search_k, include_distances=True)
    
    print(f"Annoy Searcher ({distance_metric.capitalize()})\nNearest Indices:", nearest_indices[-1])
    print("Distances:", np.round(nearest_distances[-1], 3))
    print("Time Taken:", format_time(annoy_searcher.last_result["time_seconds"]), "Time Average:", format_time(annoy_searcher.last_result["average_time_seconds"]))
    #print("Distance calculations:", annoy_searcher.last_result["distance_calculations"], "Average distance calculations:", annoy_searcher.last_result["average_distance_calculations"])
    print()


if __name__ == "__main__":
    main_annoy(N, Q, d, k)