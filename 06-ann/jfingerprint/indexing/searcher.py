from abc import ABC, abstractmethod
from typing import Any, List, Tuple, Dict, Optional
import numpy as np

class Searcher(ABC):
    """
    Abstract base class for indexing structures and k-nearest neighbors (k-NN) search.

    This class defines a standardized interface for various search structures such as 
    NaiveSearcher (exhaustive search), KDTree, BallTree, etc., ensuring consistency 
    across different implementations.
    """

    @abstractmethod
    def build(self, data: np.ndarray, **kwargs: Any) -> None:
        """
        Loads all data into the indexing structure.

        :param data: NumPy array containing the data to be indexed.
        :type data: np.ndarray
        :param kwargs: Optional keyword arguments specific to each indexing structure.
        :type kwargs: Any
        """
        pass

    @abstractmethod
    def load(self, filename: str) -> None:
        """
        Loads the indexing structure from a file on disk.

        :param filename: Path to the file to be loaded.
        :type filename: str
        """
        pass

    @abstractmethod
    def save(self, filename: str) -> None:
        """
        Saves the current indexing structure to a file on disk.

        :param filename: Path to the file where the structure will be saved.
        :type filename: str
        """
        pass

    @abstractmethod
    def search(
        self, query: np.ndarray, k: int, **kwargs: Any
    ) -> Tuple[List[int], List[float]]:
        """
        Performs a k-nearest neighbors search.

        :param query: Query vector.
        :type query: np.ndarray
        :param k: Number of nearest neighbors to retrieve.
        :type k: int
        :param kwargs: Optional keyword arguments specific to each indexing structure.
        :type kwargs: Any
        :return: A tuple containing a list of indices and a list of corresponding distances.
        :rtype: Tuple[List[int], List[float]]
        """
        pass

    @property
    @abstractmethod
    def last_result(self) -> Optional[Dict[str, Any]]:
        """
        Retrieves the last search result containing indices, distances, and search time.

        :return: Dictionary with keys 'indices', 'distances', and 'time_seconds'.
                 Returns None if no search has been performed yet.
        :rtype: Optional[Dict[str, Any]]
        """
        pass