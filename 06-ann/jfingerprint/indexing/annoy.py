from .searcher import Searcher
from typing import Any, List, Tuple, Dict, Optional
import numpy as np
from annoy import AnnoyIndex
import pickle
import time

import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from jfingerprint.printing import format_time


class AnnoySearcher(Searcher):
    """
    Annoy searcher implementation using the Annoy library.

    This searcher leverages the Annoy (Approximate Nearest Neighbors Oh Yeah) library
    for efficient k-nearest neighbors (k-NN) searches using tree ensembles.
    """

    def __init__(
        self,
        f: int,
        distance_metric: str = "angular",
        data: Optional[np.ndarray] = None,
        **kwargs: Any
    ) -> None:
        """
        Initializes the AnnoySearcher.

        :param f: Number of dimensions for the vectors.
        :type f: int
        :param distance_metric: The distance metric to use. Supported metrics include
                                "angular", "euclidean", "manhattan", "hamming", or "dot".
                                Defaults to "angular".
        :type distance_metric: str, optional
        :param data: NumPy array containing the data to be indexed. Defaults to None.
        :type data: Optional[np.ndarray], optional
        :param kwargs: Additional keyword arguments specific to the AnnoyIndex.
        :type kwargs: Any
        """
        self.f: int = f
        self.distance_metric: str = distance_metric
        self.annoy_index: Optional[AnnoyIndex] = AnnoyIndex(f, metric=distance_metric)
        self.data: Optional[np.ndarray] = None
        self._last_result: Optional[Dict[str, Any]] = None  # Initialize last_result attribute
        self.index_built: bool = False

        if data is not None:
            self.build(data, **kwargs)

    def build(self, data: np.ndarray, **kwargs: Any) -> None:
        """
        Builds the Annoy index from the provided data.
        
        The typical number of trees is 2*f to 10*f, where f is the number of dimensions.

        :param data: NumPy array containing the data to be indexed.
        :type data: np.ndarray
        :param kwargs: Optional keyword arguments specific to the AnnoyIndex.
                      Includes 'n_trees' (int) for the number of trees,
                      and 'log_time' (bool) to enable timing.
        :type kwargs: Any
        """
        n_trees = kwargs.pop('n_trees', self.f * 2)
        log_time = kwargs.pop('log_time', False)

        if log_time:
            start_time = time.perf_counter()

        self.data = data
        for i, vector in enumerate(data):
            self.annoy_index.add_item(i, vector.tolist())
        self.annoy_index.build(n_trees, n_jobs=kwargs.pop('n_jobs', -1))
        self.index_built = True

        if log_time:
            end_time = time.perf_counter()
            elapsed_time = end_time - start_time
            print(f"Annoy index built in {format_time(elapsed_time)}")

    def load(self, filename: str) -> None:
        """
        Loads the Annoy index from a binary file on disk.

        :param filename: Path to the binary file to be loaded.
        :type filename: str
        """
        if not os.path.exists(filename):
            raise FileNotFoundError(f"The file {filename} does not exist.")

        filename = filename.split(".")[0]
        filenamePick = filename + ".pkl"
        filenameAnnoy = filename + ".ann"
        
        with open(filenamePick, 'rb') as file:
            loaded_data = pickle.load(file)
            self.data = loaded_data['data']
            f_loaded = loaded_data['f']
            distance_metric_loaded = loaded_data['distance_metric']
            self.annoy_index = AnnoyIndex(f_loaded, metric=distance_metric_loaded)
            self.annoy_index.load(filenameAnnoy)
            self.f = f_loaded
            self.distance_metric = distance_metric_loaded
            self.index_built = True

    def save(self, filename: str) -> None:
        """
        Saves the current Annoy index to a binary file on disk.

        :param filename: Path to the binary file where the index will be saved.
        :type filename: str
        """
        if not self.index_built or self.data is None:
            raise ValueError("Annoy index has not been built. Use the 'build' method to create the index before saving.")
        
        filename = filename.split(".")[0]
        filenamePick = filename + ".pkl"
        filenameAnnoy = filename + ".ann"
        

        self.annoy_index.save(filenameAnnoy)
        with open(filenamePick, 'wb') as file:
            pickle.dump({
                'data': self.data,
                'f': self.f,
                'distance_metric': self.distance_metric
            }, file)

    def search(
        self, queries: np.ndarray, k: int, **kwargs: Any
    ) -> Tuple[List[List[int]], List[List[float]]]:
        """
        Performs a k-nearest neighbors search using the Annoy index.

        Supports multiple query vectors and records total time, average time, and distance calculations.

        :param queries: Query vector or array of query vectors.
        :type queries: np.ndarray
        :param k: Number of nearest neighbors to retrieve.
        :type k: int
        :param kwargs: Additional keyword arguments such as 'search_k' and 'include_distances'.
        :type kwargs: Any
        :return: A tuple containing a list of lists of indices and a list of lists of corresponding distances.
        :rtype: Tuple[List[List[int]], List[List[float]]]
        """
        if not self.index_built or self.data is None:
            raise ValueError("Annoy index has not been built. Use the 'build' method to load data.")

        search_k = kwargs.get('search_k', -1)
        include_distances = kwargs.get('include_distances', False)

        num_queries = queries.shape[0] if queries.ndim > 1 else 1
        #distance_calculations = num_queries * k  # Approximation

        start_time = time.perf_counter()  # Start timing

        nearest_indices = []
        nearest_distances = []

        if queries.ndim == 1:
            queries = queries.reshape(1, -1)

        for query in queries:
            result = self.annoy_index.get_nns_by_vector(query.tolist(), k, search_k=search_k, include_distances=include_distances)
            if include_distances:
                indices, distances = result
                nearest_indices.append(indices)
                nearest_distances.append(distances)
            else:
                nearest_indices.append(result)
                nearest_distances.append([])  # No distances provided

        end_time = time.perf_counter()
        elapsed_time = end_time - start_time
        average_time = elapsed_time / num_queries if num_queries > 0 else elapsed_time

        self._last_result = {
            "indices": nearest_indices,
            "distances": nearest_distances,
            "time_seconds": elapsed_time,
            "average_time_seconds": average_time,
            #"distance_calculations": distance_calculations,
            #"average_distance_calculations": distance_calculations / num_queries if num_queries > 0 else distance_calculations,
        }

        return nearest_indices, nearest_distances

    @property
    def last_result(self) -> Optional[Dict[str, Any]]:
        """
        Retrieves the last search result containing indices, distances, timing information, and distance calculations (total and average).

        :return: Dictionary with keys "indices", "distances", "time_seconds",
                 "average_time_seconds", "distance_calculations",
                 "average_distance_calculations".
                 Returns None if no search has been performed yet.
        :rtype: Optional[Dict[str, Any]]
        """
        return self._last_result