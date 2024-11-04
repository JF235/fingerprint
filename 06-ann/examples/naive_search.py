import sys
import os
import numpy as np

# Add the parent directory to sys.path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

import jfingerprint as jf
from jfingerprint.indexing import NaiveSearcher
from jfingerprint.printing import format_time

def main():
    np.random.seed(42)
    N = 100000
    Q = 40
    d = 3
    k = 3

    data = jf.data.generate_unit_vectors(N, d)
    queries = jf.data.generate_unit_vectors(Q, d)

    ns_euclidean = NaiveSearcher(jf.math.euclidean_distance, data=data)
    
    nearest_indices_euclidean, euclidean_distances = ns_euclidean.search(queries, k)
    

    print("Naive Searcher (Euclidean)\nNearest Indices:", nearest_indices_euclidean[-1])
    print("Distances:", np.round(euclidean_distances[-1], 3))
    print("Time Taken:", format_time(ns_euclidean.last_result["time_seconds"]), "Time Average:", format_time(ns_euclidean.last_result["average_time_seconds"]))
    print("Distance calculations:", ns_euclidean.last_result["distance_calculations"], "Average distance calculations:", ns_euclidean.last_result["average_distance_calculations"], "\n")
    
    #==========================================================================
    
    ns_cosine = NaiveSearcher(jf.math.cosine_distanceU, data=data)
    nearest_indices_cosine, cosine_distances = ns_cosine.search(queries, k)
    
    result = ns_cosine.last_result
    print("\nNaive Searcher (Cosine)\nNearest Indices:", result["indices"][-1])
    print("Distances:", np.round(result["distances"][-1], 3))
    print("Time Taken:", format_time(result["time_seconds"]), "Time Average:", format_time(result["average_time_seconds"]))
    print("Distance calculations:", result["distance_calculations"], "Average distance calculations:", result["average_distance_calculations"], "\n")
    
    
if __name__ == "__main__":
    main()