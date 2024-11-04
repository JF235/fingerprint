from .searcher import Searcher
from typing import Any, List, Tuple, Dict, Optional
import numpy as np
from sklearn.neighbors import KDTree
import pickle
import time

# Import jfingerprint.printing.format_time
import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from jfingerprint.printing import format_time


class KdTree(Searcher):
    """
    KD-Tree searcher implementation using scikit-learn's KDTree.

    This searcher leverages the KD-Tree data structure for efficient k-nearest neighbors (k-NN) searches.
    """

    def __init__(
        self,
        distance_metric: str = "euclidean",
        data: Optional[np.ndarray] = None,
        **kwargs: Any
    ) -> None:
        """
        Initializes the KdTreeSearcher.

        :param distance_metric: The distance metric to use. Supported metrics include
                                'euclidean', 'manhattan', 'chebyshev', etc., as supported by scikit-learn's KDTree.
                                Defaults to 'euclidean'.
        :type distance_metric: str, optional
        :param data: NumPy array containing the data to be indexed. Defaults to None.
        :type data: Optional[np.ndarray], optional
        :param kwargs: Additional keyword arguments specific to the KDTree.
        :type kwargs: Any
        """
        self.distance_metric: str = distance_metric
        self.kdtree: Optional[KDTree] = None
        self.data: Optional[np.ndarray] = None

        self._last_result: Optional[Dict[str, Any]] = None  # Initialize last_result attribute

        if data is not None:
            self.build(data, **kwargs)

    def build(self, data: np.ndarray, **kwargs: Any) -> None:
        """
        Builds the KD-Tree from the provided data.

        :param data: NumPy array containing the data to be indexed.
        :type data: np.ndarray
        :param kwargs: Optional keyword arguments specific to scikit-learn's KDTree.
                      Includes 'log_time' (bool) to enable timing.
        :type kwargs: Any
        """
        log_time = kwargs.pop('log_time', False)
        
        if log_time:
            start_time = time.perf_counter()
        
        self.data = data
        self.kdtree = KDTree(data, metric=self.distance_metric, **kwargs)
        
        if log_time:
            end_time = time.perf_counter()
            elapsed_time = end_time - start_time
            print(f"KD-Tree built in {format_time(elapsed_time)}")

    def load(self, filename: str) -> None:
        """
        Loads the KD-Tree from a binary file on disk.

        :param filename: Path to the binary file to be loaded.
        :type filename: str
        """
        with open(filename, 'rb') as file:
            loaded_data = pickle.load(file)
            self.data = loaded_data['data']
            self.kdtree = loaded_data['kdtree']
            self.distance_metric = loaded_data['distance_metric']

    def save(self, filename: str) -> None:
        """
        Saves the current KD-Tree to a binary file on disk.

        :param filename: Path to the binary file where the structure will be saved.
        :type filename: str
        """
        if self.kdtree is None or self.data is None:
            raise ValueError("KD-Tree has not been built.")

        with open(filename, 'wb') as file:
            pickle.dump({
                'data': self.data,
                'kdtree': self.kdtree,
                'distance_metric': self.distance_metric
            }, file)

    def search(
        self, queries: np.ndarray, k: int, **kwargs: Any
    ) -> Tuple[List[int], List[float]]:
        """
        Performs a k-nearest neighbors search using the KD-Tree.

        :param query: Query vector or array of query vectors.
        :type query: np.ndarray
        :param k: Number of nearest neighbors to retrieve.
        :type k: int
        :param kwargs: Additional keyword arguments passed to scikit-learn's KDTree query method.
        :type kwargs: Any
        :return: A tuple containing a list of indices and a list of corresponding distances.
        :rtype: Tuple[List[int], List[float]]
        """
        if self.kdtree is None or self.data is None:
            raise ValueError("KD-Tree has not been built. Use the 'build' method to load data.")

        num_queries = queries.shape[0] if queries.ndim > 1 else 1
        
        self.kdtree.reset_n_calls()
        start_time = time.perf_counter()  # Start timing

        # Ensure query is two-dimensional
        if queries.ndim == 1:
            query = queries.reshape(1, -1)
            distances, indices = self.kdtree.query(query, k=k, **kwargs)
        elif num_queries > 1:
            distances, indices = self.kdtree.query(queries, k=k, **kwargs)

        end_time = time.perf_counter()
        elapsed_time = end_time - start_time
        average_time = elapsed_time / num_queries
        
        # Get the number of distance calculations
        distance_calculations = self.kdtree.get_n_calls()
        average_distance_calculations = distance_calculations / num_queries

        # If single query, flatten the results
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
            "average_distance_calculations": average_distance_calculations
        }

        return nearest_indices, nearest_distances

    @property
    def last_result(self) -> Optional[Dict[str, Any]]:
        """
        Retrieves the last search result containing indices, distances, timing information, and distance calculations (total and average).

        :return: Dictionary with keys "indices", "distances", "time_seconds", "average_time_seconds", and "distance_calculations", "average_distance_calculations".
                 Returns None if no search has been performed yet.
        :rtype: Optional[Dict[str, Any]]
        """
        return self._last_result