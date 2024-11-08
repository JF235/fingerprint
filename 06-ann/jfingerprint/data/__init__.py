import os
import numpy as np
import time
from typing import List, Optional

import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from jfingerprint.printing import format_time

def load_features(path: str, max_files: Optional[int] = None, log_info: bool = False, get_info: bool = False) -> np.ndarray:
    """
    Loads feature vectors from .npy files in the specified directory or from a single .npy file.

    :param path: The path to the directory containing .npy files or a single .npy file.
    :type path: str
    :param max_files: The maximum number of files to load. If None, all files are loaded.
    :type max_files: int, optional
    :param log_info: Whether to log the time taken to load the features.
    :type log_info: bool, optional
    :param get_info: Whether to return additional information (feature shapes and filenames).
    :type get_info: bool, optional
    :return: A NumPy array containing the loaded feature vectors.
    :rtype: np.ndarray
    """
    feature_list: List[np.ndarray] = []
    feature_shapes: List[int] = []
    filenames: List[str] = []
    files_loaded = 0

    if log_info:
        start_time = time.perf_counter()

    if os.path.isfile(path):
        # If the path is a single .npy file
        feature_vector = np.load(path)
        feature_list.append(feature_vector)
        feature_shapes.append(feature_vector.shape[0])
        files_loaded = 1
        filenames.append(path)
    elif os.path.isdir(path):
        # If the path is a directory
        for filename in os.listdir(path):
            if filename.endswith('.npy'):
                filepath = os.path.join(path, filename)
                feature_vector = np.load(filepath)
                feature_list.append(feature_vector)
                feature_shapes.append(feature_vector.shape[0])
                files_loaded += 1
                filenames.append(filename)
                if max_files is not None and files_loaded >= max_files:
                    break
    else:
        raise ValueError(f"The path {path} is neither a file nor a directory.")

    features = np.vstack(feature_list)

    if log_info:
        end_time = time.perf_counter()
        elapsed_time = end_time - start_time
        print(f"Loaded {files_loaded} file(s) from {path} in {format_time(elapsed_time)}")
        print(f"Shape of loaded features: {features.shape}")

    if get_info:
        return features, feature_shapes, filenames

    return features

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