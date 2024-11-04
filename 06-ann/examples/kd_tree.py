import sys
import os
import numpy as np
import sklearn

# Add the parent directory to sys.path to locate the jfingerprint package
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

import jfingerprint as jf
from jfingerprint.indexing import KdTree
from jfingerprint.printing import format_time

def main():
    np.random.seed(42)
    N = 100000
    Q = 40
    d = 3
    k = 3
    
    data = jf.data.generate_unit_vectors(N, d)
    queries = jf.data.generate_unit_vectors(Q, d)

    # Check if kd_tree_euclidean.pkl exists
    if os.path.exists(f'build/kdtree_euclidean_{d}.pkl'):
        # Load the KD-Tree from the binary file
        kd_tree_euclidean = KdTree()
        kd_tree_euclidean.load(f'build/kdtree_euclidean_{d}.pkl')
        print("KD-Tree loaded successfully.")
    else:
        # Instantiate the KdTreeSearcher with Euclidean distance
        kd_tree_euclidean = KdTree(distance_metric='euclidean')
        kd_tree_euclidean.build(data, leaf_size=30)
        kd_tree_euclidean.save(f'build/kdtree_euclidean_{d}.pkl')
        print("KD-Tree built and saved successfully.")
    
    # Perform the search
    nearest_indices_euclidean, euclidean_distances = kd_tree_euclidean.search(queries, k)
    
    print("Naive Searcher (Euclidean)\nNearest Indices:", nearest_indices_euclidean[-1])
    print("Distances:", np.round(euclidean_distances[-1], 3))
    print("Time Taken:", format_time(kd_tree_euclidean.last_result["time_seconds"]), "Time Average:", format_time(kd_tree_euclidean.last_result["average_time_seconds"]))
    print("Distance calculations:", kd_tree_euclidean.last_result["distance_calculations"], "Average distance calculations:", kd_tree_euclidean.last_result["average_distance_calculations"], "\n")
    

if __name__ == "__main__":
    main()
    #print(sklearn.neighbors.VALID_METRICS['kd_tree'])