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
    d = 64
    data = jf.data.generate_unit_vectors(N, d)

    # Choose a query vector randomly
    query = jf.data.generate_unit_vectors(1, d)[0]

    # Define k
    k = 5

    # Instantiate the NaiveSearcher with data and Euclidean distance
    ns_euclidean = NaiveSearcher(jf.math.euclidean_distance, data=data)
    
    # Perform the search
    nearest_indices_euclidean, euclidean_distances = ns_euclidean.search(query, k)
    
    

    print("Naive Searcher (Euclidean)\nNearest Indices:", nearest_indices_euclidean)
    print("Distances:", np.round(euclidean_distances, 3))
    print("Time Taken:", format_time(ns_euclidean.last_result["time_seconds"]), "\n")
    
    # Instantiate the NaiveSearcher with data and cosine distance (unit vectors) and repeat
    ns_cosine = NaiveSearcher(jf.math.cosine_distanceU, data=data)
    nearest_indices_cosine, cosine_distances = ns_cosine.search(query, k)
    
    result = ns_cosine.last_result
    print("\nNaive Searcher (Cosine)\nNearest Indices:", result["indices"])
    print("Distances:", np.round(result["distances"], 3))
    print("Time Taken:", format_time(result["time_seconds"]), "\n")
    
    
if __name__ == "__main__":
    main()