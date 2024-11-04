from .searcher import Searcher
from typing import Any, List, Tuple, Dict, Optional
import numpy as np
from sklearn.neighbors import BallTree
import pickle
import time

# Import jfingerprint.printing.format_time
import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from jfingerprint.printing import format_time


class BallTreeSearcher(Searcher):
    """
    Ball-Tree searcher implementation using scikit-learn's BallTree.

    This searcher leverages the Ball-Tree data structure for efficient k-nearest neighbors (k-NN) searches.
    """

    def __init__(
        self,
        distance_metric: str = "euclidean",
        data: Optional[np.ndarray] = None,
        **kwargs: Any
    ) -> None:
        """
        Initializes the BallTreeSearcher.

        :param distance_metric: The distance metric to use. Supported metrics include
                                'euclidean', 'manhattan', 'chebyshev', etc., as supported by scikit-learn's BallTree.
                                Defaults to 'euclidean'.
        :type distance_metric: str, optional
        :param data: NumPy array containing the data to be indexed. Defaults to None.
        :type data: Optional[np.ndarray], optional
        :param kwargs: Additional keyword arguments specific to the BallTree.
        :type kwargs: Any
        """
        self.distance_metric: str = distance_metric
        self.ball_tree: Optional[BallTree] = None
        self.data: Optional[np.ndarray] = None

        self._last_result: Optional[Dict[str, Any]] = None  # Initialize last_result attribute

        if data is not None:
            self.build(data, **kwargs)

    def build(self, data: np.ndarray, **kwargs: Any) -> None:
        """
        Builds the Ball-Tree from the provided data.

        :param data: NumPy array containing the data to be indexed.
        :type data: np.ndarray
        :param kwargs: Optional keyword arguments specific to scikit-learn's BallTree.
                      Includes 'log_time' (bool) to enable timing.
        :type kwargs: Any
        """
        log_time = kwargs.pop('log_time', False)

        if log_time:
            start_time = time.perf_counter()

        self.data = data
        self.ball_tree = BallTree(data, metric=self.distance_metric, **kwargs)

        if log_time:
            end_time = time.perf_counter()
            elapsed_time = end_time - start_time
            print(f"Ball-Tree built in {format_time(elapsed_time)}")

    def load(self, filename: str) -> None:
        """
        Loads the Ball-Tree from a binary file on disk.

        :param filename: Path to the binary file to be loaded.
        :type filename: str
        """
        with open(filename, 'rb') as file:
            loaded_data = pickle.load(file)
            self.data = loaded_data['data']
            self.ball_tree = loaded_data['ball_tree']
            self.distance_metric = loaded_data['distance_metric']

    def save(self, filename: str) -> None:
        """
        Saves the current Ball-Tree to a binary file on disk.

        :param filename: Path to the binary file where the structure will be saved.
        :type filename: str
        """
        if self.ball_tree is None or self.data is None:
            raise ValueError("Ball-Tree has not been built.")

        with open(filename, 'wb') as file:
            pickle.dump({
                'data': self.data,
                'ball_tree': self.ball_tree,
                'distance_metric': self.distance_metric
            }, file)

    def search(
        self, queries: np.ndarray, k: int, **kwargs: Any
    ) -> Tuple[List[List[int]], List[List[float]]]:
        """
        Performs a k-nearest neighbors search using the Ball-Tree.

        Supports multiple query vectors and records total time, average time, and distance calculations.

        :param queries: Query vector or array of query vectors.
        :type queries: np.ndarray
        :param k: Number of nearest neighbors to retrieve.
        :type k: int
        :param kwargs: Additional keyword arguments passed to scikit-learn's BallTree query method.
        :type kwargs: Any
        :return: A tuple containing a list of lists of indices and a list of lists of corresponding distances.
        :rtype: Tuple[List[List[int]], List[List[float]]]
        """
        if self.ball_tree is None or self.data is None:
            raise ValueError("Ball-Tree has not been built. Use the 'build' method to load data.")

        num_queries = queries.shape[0] if queries.ndim > 1 else 1
        distance_calculations = self.ball_tree.reset_n_calls()

        start_time = time.perf_counter()  # Start timing

        # Perform the search with the BallTree
        distances, indices = self.ball_tree.query(queries, k=k, **kwargs)

        end_time = time.perf_counter()  # End timing
        elapsed_time = end_time - start_time  # Total time
        average_time = elapsed_time / num_queries if num_queries > 0 else elapsed_time  # Average time

        distance_calculations = self.ball_tree.get_n_calls()
        
        # Convert numpy arrays to lists
        if num_queries == 1:
            nearest_indices = indices[0].tolist()
            nearest_distances = distances[0].tolist()
        else:
            nearest_indices = indices.tolist()
            nearest_distances = distances.tolist()

        self._last_result = {
            "indices": nearest_indices,
            "distances": nearest_distances,
            "time_seconds": elapsed_time,
            "average_time_seconds": average_time,
            "distance_calculations": distance_calculations,
            "average_distance_calculations": distance_calculations / num_queries if num_queries > 0 else distance_calculations,
        }

        return nearest_indices, nearest_distances

    @property
    def last_result(self) -> Optional[Dict[str, Any]]:
        """
        Retrieves the last search result containing indices, distances, timing information, and distance calculations (total and average).

        :return: Dictionary with keys "indices", "distances", "time_seconds", "average_time_seconds", "distance_calculations", "average_distance_calculations".
                 Returns None if no search has been performed yet.
        :rtype: Optional[Dict[str, Any]]
        """
        return self._last_result