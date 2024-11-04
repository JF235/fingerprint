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
        self, query: np.ndarray, k: int, **kwargs: Any
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

        start_time = time.perf_counter()  # Start timing

        # Ensure query is two-dimensional
        if query.ndim == 1:
            query = query.reshape(1, -1)

        distances, indices = self.kdtree.query(query, k=k, **kwargs)

        end_time = time.perf_counter()  # End timing
        elapsed_time = end_time - start_time  # Calculate elapsed time

        # If single query, flatten the results
        if indices.shape[0] == 1:
            nearest_indices = indices[0].tolist()
            nearest_distances = distances[0].tolist()
        else:
            # For multiple queries, store lists of lists
            nearest_indices = indices.tolist()
            nearest_distances = distances.tolist()

        # Store the results in last_result
        self._last_result = {
            "indices": nearest_indices,
            "distances": nearest_distances,
            "time_seconds": elapsed_time,
        }

        return nearest_indices, nearest_distances

    @property
    def last_result(self) -> Optional[Dict[str, Any]]:
        """
        Retrieves the last search result containing indices, distances, and search time.

        :return: Dictionary with keys 'indices', 'distances', and 'time_seconds'.
                 Returns None if no search has been performed yet.
        :rtype: Optional[Dict[str, Any]]
        """
        return self._last_result