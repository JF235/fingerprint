import sys
import os
import numpy as np

# Add the parent directory to sys.path to locate the jfingerprint package
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

import jfingerprint as jf
from jfingerprint.indexing import VoyagerSearcher
from jfingerprint.printing import format_time

from voyager import Space, StorageDataType, Index

N = 100
Q = 1
d = 5
k = 3

def main_voyager(N, Q, d, k):
    np.random.seed(42)
    
    data = jf.data.generate_unit_vectors(N, d)
    queries = jf.data.generate_unit_vectors(Q, d)
    
    space: Space = Space.Euclidean
    M: int = 64
    ef_construction: int = 20000
    query_ef: int = 200
    storage_data_type: StorageDataType = StorageDataType.Float32
    
    voyager_searcher = VoyagerSearcher(space=space, num_dimensions=d, M=M, ef_construction=ef_construction, storage_data_type=storage_data_type)
    if os.path.exists(f'build/voyager_index_{d}.voy'):
        voyager_searcher.load(f'build/voyager_index_{d}.voy')
        print("Voyager index loaded successfully.")
    else:    
        voyager_searcher.build(data, log_time=True)
        voyager_searcher.save(f'build/voyager_index_{d}.voy')
        print("Voyager index built and saved successfully.")
    
    nearest_indices, nearest_distances = voyager_searcher.search(queries, k, query_ef=query_ef)
    
    nearest_distances = np.sqrt(nearest_distances)
    
    print("Voyager Searcher (Euclidean)\nNearest Indices:", nearest_indices[-1])
    print("Distances:", np.round(nearest_distances[-1], 3))
    print("Time Taken:", format_time(voyager_searcher.last_result["time_seconds"]), "Time Average:", format_time(voyager_searcher.last_result["average_time_seconds"]))
    #print("Distance calculations:", annoy_searcher.last_result["distance_calculations"], "Average distance calculations:", annoy_searcher.last_result["average_distance_calculations"], "\n")


if __name__ == "__main__":
    main_voyager(N, Q, d, k)