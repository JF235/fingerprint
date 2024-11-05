import sys
import os
import numpy as np

# Add the parent directory to sys.path to locate the jfingerprint package
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

import jfingerprint as jf
from jfingerprint.indexing import BallTreeSearcher
from jfingerprint.printing import format_time

N = 100000
Q = 40
d = 3
k = 3

def main_balltree(N, Q, d, k):
    np.random.seed(42)

    data = jf.data.generate_unit_vectors(N, d)
    queries = jf.data.generate_unit_vectors(Q, d)

    if os.path.exists(f'build/ball_tree_euclidean_{d}.pkl'):
        # Load the Ball-Tree from the binary file
        ball_tree_euclidean = BallTreeSearcher()
        ball_tree_euclidean.load(f'build/ball_tree_euclidean_{d}.pkl')
        print("Ball-Tree loaded successfully.")
    else:
        # Instantiate the BallTreeSearcher with Euclidean distance
        ball_tree_euclidean = BallTreeSearcher(distance_metric='euclidean')
        ball_tree_euclidean.build(data, leaf_size=30, log_time=True)
        ball_tree_euclidean.save(f'build/ball_tree_euclidean_{d}.pkl')
        print("Ball-Tree built and saved successfully.")

    # Perform the search
    nearest_indices_euclidean, euclidean_distances = ball_tree_euclidean.search(queries, k)
    
    if Q == 1:
        nearest_indices_euclidean = [nearest_indices_euclidean]
        euclidean_distances = [euclidean_distances]

    print("Ball-Tree Searcher (Euclidean)\nNearest Indices:", nearest_indices_euclidean[-1])
    print("Distances:", np.round(euclidean_distances[-1], 3))
    print("Time Taken:", format_time(ball_tree_euclidean.last_result["time_seconds"]),
          "Time Average:", format_time(ball_tree_euclidean.last_result["average_time_seconds"]))
    print("Distance calculations:", ball_tree_euclidean.last_result["distance_calculations"],
          "Average distance calculations:", ball_tree_euclidean.last_result["average_distance_calculations"], "\n")


if __name__ == "__main__":
    main_balltree(N, Q, d, k)