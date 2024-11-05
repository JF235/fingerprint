from .searcher import Searcher
from typing import Any, List, Tuple, Dict, Optional
import numpy as np
import time

import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'voyager')))
from voyager import Index, StorageDataType, Space

# Import jfingerprint.printing.format_time
from jfingerprint.printing import format_time


class VoyagerSearcher(Searcher):
    """
    Voyager searcher implementation using the Voyager library.

    This searcher leverages the Voyager Index for efficient k-nearest neighbors (k-NN) searches.
    """

    def __init__(
        self,
        space: Space = Space.Euclidean,
        num_dimensions: int = 128,
        M: int = 12,
        ef_construction: int = 200,
        random_seed: int = 1,
        max_elements: int = 1,
        storage_data_type: StorageDataType = StorageDataType.Float32,
        **kwargs: Any
    ) -> None:
        """
        Initializes the VoyagerSearcher.

        :param space: The Space to use to calculate the distance between vectors.
                      Defaults to Space.Cosine.
        :type space: Space, optional
        :param num_dimensions: The number of dimensions present in each vector added to this index.
                               Defaults to 128.
        :type num_dimensions: int, optional
        :param M: The number of connections between nodes in the tree's internal data structure.
                  Defaults to 12.
        :type M: int, optional
        :param ef_construction: The number of vectors to search through when inserting a new vector.
                                 Defaults to 200.
        :type ef_construction: int, optional
        :param random_seed: The seed for the random number generator. Defaults to 1.
        :type random_seed: int, optional
        :param max_elements: The maximum size of the index at construction time. Defaults to 100000.
        :type max_elements: int, optional
        :param storage_data_type: The data type used to store vectors. Defaults to StorageDataType.Float32.
        :type storage_data_type: StorageDataType, optional
        :param kwargs: Additional keyword arguments for the Voyager Index.
        :type kwargs: Any
        """
        self.space: Space = space
        self.num_dimensions: int = num_dimensions
        self.M: int = M
        self.ef_construction: int = ef_construction
        self.random_seed: int = random_seed
        self.max_elements: int = max_elements
        self.storage_data_type: StorageDataType = storage_data_type
        self.index: Optional[Index] = None

        self._last_result: Optional[Dict[str, Any]] = None  # Initialize last_result attribute

        if kwargs.get('data') is not None:
            self.build(kwargs.pop('data'), **kwargs)

    def build(self, data: np.ndarray, **kwargs: Any) -> None:
        """
        Builds the Voyager index from the provided data.

        :param data: NumPy array containing the data to be indexed.
        :type data: np.ndarray
        :param kwargs: Optional keyword arguments for the Voyager Index.
                       Includes 'log_time' (bool) to enable timing.
        :type kwargs: Any
        """
        log_time = kwargs.pop('log_time', False)

        if log_time and data.size > 0:
            start_time = time.perf_counter()

        self.index = Index(
            space=self.space,
            num_dimensions=self.num_dimensions,
            M=self.M,
            ef_construction=self.ef_construction,
            random_seed=self.random_seed,
            max_elements=self.max_elements,
            storage_data_type=self.storage_data_type
        )
        
        if data.size > 0:
            self.index.add_items(data.astype(np.float32), num_threads=kwargs.pop('num_threads', -1))

        if log_time and data.size > 0:
            end_time = time.perf_counter()
            elapsed_time = end_time - start_time
            print(f"Voyager index built in {format_time(elapsed_time)}")

    def load(self, filename: str, **kwargs) -> None:
        """
        Loads the Voyager index from a binary file on disk.

        :param filename: Path to the binary file to be loaded.
        :type filename: str
        """
        with open(filename, "rb") as f:
            self.index = Index.load(f)
            self.num_dimensions = self.index.num_dimensions
            self.M = self.index.M
            self.ef_construction = self.index.ef_construction
            #self.random_seed = self.index.random_seed
            self.max_elements = self.index.max_elements
            self.storage_data_type = self.index.storage_data_type
            self.space = self.index.space
            #self.ef = self.index.ef
            


    def save(self, filename: str) -> None:
        """
        Saves the current Voyager index to a binary file on disk.

        :param filename: Path to the binary file where the index will be saved.
        :type filename: str
        """
        if self.index is None:
            raise ValueError("Voyager index has not been built.")
        else:
            self.index.save(filename)

    def search(
        self, queries: np.ndarray, k: int, **kwargs: Any
    ) -> Tuple[List[List[int]], List[List[float]]]:
        """
        Performs a k-nearest neighbors search using the Voyager index.

        Supports multiple query vectors and records total time, average time, and distance calculations.

        :param queries: Query vector or array of query vectors.
        :type queries: np.ndarray
        :param k: Number of nearest neighbors to retrieve.
        :type k: int
        :param kwargs: Additional keyword arguments such as 'num_threads' and 'query_ef'.
        :type kwargs: Any
        :return: A tuple containing a list of lists of indices and a list of lists of corresponding distances.
        :rtype: Tuple[List[List[int]], List[List[float]]]
        """
        if self.index is None:
            raise ValueError("Voyager index has not been built.")

        num_queries = queries.shape[0] if queries.ndim > 1 else 1
        distance_calculations = 0

        start_time = time.perf_counter()  # Start timing

        neighbor_ids, distances = self.index.query(queries.astype(np.float32), k=k, num_threads=kwargs.get('num_threads', -1), query_ef=kwargs.get('query_ef', -1))

        end_time = time.perf_counter()  # End timing
        elapsed_time = end_time - start_time  # Total time
        average_time = elapsed_time / num_queries if num_queries > 0 else elapsed_time  # Average time

        # Convert numpy arrays to lists
        nearest_indices = neighbor_ids.tolist()
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

        :return: Dictionary with keys "indices", "distances", "time_seconds",
                 "average_time_seconds", "distance_calculations",
                 "average_distance_calculations".
                 Returns None if no search has been performed yet.
        :rtype: Optional[Dict[str, Any]]
        """
        return self._last_result