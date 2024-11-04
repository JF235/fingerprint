import os
import numpy as np
from typing import List, Optional

def load_features(directory: str, max_files: Optional[int] = None) -> np.ndarray:
    """
    Loads feature vectors from .npy files in the specified directory.

    :param directory: The path to the directory containing .npy files.
    :type directory: str
    :param max_files: The maximum number of files to load. If None, all files are loaded.
    :type max_files: int, optional
    :return: A 2D array with shape (n, d) where n is the number of files and d is the dimension of the vectors.
    :rtype: np.ndarray
    """
    feature_list: List[np.ndarray] = []
    files_loaded = 0

    for filename in os.listdir(directory):
        if filename.endswith('.npy'):
            filepath = os.path.join(directory, filename)
            feature_vector = np.load(filepath)
            feature_list.append(feature_vector)
            files_loaded += 1
            if max_files is not None and files_loaded >= max_files:
                break
    
    return np.vstack(feature_list)

def generate_unit_vectors(N: int, D: int) -> np.ndarray:
    """
    Generates N random D-dimensional unit vectors uniformly distributed on the sphere.

    :param N: The number of vectors to generate.
    :type N: int
    :param D: The dimensionality of each vector.
    :type D: int
    :return: An array of shape (N, D) containing the generated unit vectors.
    :rtype: np.ndarray
    """
    # Generate random vectors from a normal distribution
    random_vectors = np.random.randn(N, D)
    
    # Normalize each vector to have unit length
    norms = np.linalg.norm(random_vectors, axis=1, keepdims=True)
    unit_vectors = random_vectors / norms
    
    return unit_vectors