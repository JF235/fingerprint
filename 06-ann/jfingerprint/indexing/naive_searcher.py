from .searcher import Searcher
from typing import Any, List, Callable, Optional, Tuple, Dict
import numpy as np
import jfingerprint.math as jfmath
import time


class NaiveSearcher(Searcher):
    """
    Exhaustive searcher implementation (Naive Searcher).

    This searcher performs a brute-force search to find the k-nearest neighbors.
    """

    def __init__(
        self,
        distance_func: Callable[[np.ndarray, np.ndarray], float],
        data: Optional[np.ndarray] = None,
        **kwargs: Any
    ) -> None:
        """
        Initializes the NaiveSearcher.

        :param distance_func: Function to compute the distance between two vectors.
        :type distance_func: Callable[[np.ndarray, np.ndarray], float]
        :param data: NumPy array containing the data to be indexed. Defaults to None.
        :type data: Optional[np.ndarray], optional
        :param kwargs: Additional keyword arguments specific to the searcher.
        :type kwargs: Any
        """
        self.data: np.ndarray = data if data is not None else np.array([])
        self.distance_func: Callable[[np.ndarray, np.ndarray], float] = distance_func

        self._last_result: Optional[Dict[str, Any]] = None  # Initialize last_result attribute

        if data is not None:
            self.build(data, **kwargs)

    def build(self, data: np.ndarray, **kwargs: Any) -> None:
        """
        Loads all data into the search structure.

        :param data: NumPy array containing the data to be indexed.
        :type data: np.ndarray
        :param kwargs: Optional keyword arguments specific to the searcher.
        :type kwargs: Any
        """
        self.data = data

    def load(self, filename: str) -> None:
        """
        Loads the search structure from a .npy file on disk.

        :param filename: Path to the .npy file to be loaded.
        :type filename: str
        """
        self.data = np.load(filename)

    def save(self, filename: str) -> None:
        """
        Saves the current search structure to a .npy file on disk.

        :param filename: Path to the .npy file where the structure will be saved.
        :type filename: str
        """
        np.save(filename, self.data)

    def search(
        self, queries: np.ndarray, k: int, **kwargs: Any
    ) -> Tuple[List[List[int]], List[List[float]]]:
        """
        Performs a brute-force k-nearest neighbors search.

        Utilizes optimized NumPy operations for specific distance functions
        (euclidean_distance and cosine_distanceU). Falls back to a list comprehension
        for any other distance functions. Records the indices, distances, number of distances calculations, total time and average time per query.

        :param queries: NumPy array containing the query vectors.
        :type queries: np.ndarray
        :param k: Number of nearest neighbors to retrieve.
        :type k: int
        :param kwargs: Optional keyword arguments specific to the searcher.
        :type kwargs: Any
        :return: A tuple containing a list of indices and a list of corresponding distances.
        :rtype: Tuple[List[List[int]], List[List[float]]]
        """
        if self.data.size == 0:
            raise ValueError("Data structure is empty.")

        num_queries = queries.shape[0] if queries.ndim > 1 else 1
        average_distance_calculations = self.data.shape[0]
        distance_calculations = num_queries * self.data.shape[0]

        start_time = time.perf_counter()

        if self.distance_func == jfmath.euclidean_distance:
            distances = np.linalg.norm(self.data - queries[:, np.newaxis], axis=2)
        elif self.distance_func == jfmath.cosine_distanceU:
            distances = 1 - np.dot(queries, self.data.T)
        else:
            # Fallback to list comprehension for custom distance functions
            if num_queries > 1:
                distances = np.array([[self.distance_func(q, point) for point in self.data] for q in queries])
            else:
                distances = np.array([self.distance_func(queries, point) for point in self.data])

        # Get the indices of the k smallest distances
        nearest_indices = np.argsort(distances, axis=1)[:, :k]
        nearest_distances = np.take_along_axis(distances, nearest_indices, axis=1)

        end_time = time.perf_counter()
        elapsed_time = end_time - start_time
        average_time = elapsed_time / num_queries if num_queries > 1 else elapsed_time

        # Store the results in last_result
        self._last_result = {
            "indices": nearest_indices,
            "distances": nearest_distances,
            "time_seconds": elapsed_time,
            "average_time_seconds": average_time,
            "distance_calculations": distance_calculations,
            "average_distance_calculations": average_distance_calculations,
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