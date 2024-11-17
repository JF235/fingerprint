import os
import numpy as np
import time
from typing import List, Optional, Union

import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from jfingerprint.printing import format_time

def load_features(path: str, max_files: Optional[int] = None, log_info: bool = False, get_info: bool = False, file_type: Optional[str] = None) -> Union[np.ndarray, tuple]:
    """
    Loads feature vectors from files in the specified directory or from a single file.

    :param path: The path to the directory containing feature files or a single feature file.
    :type path: str
    :param max_files: The maximum number of files to load. If None, all files are loaded.
    :type max_files: int, optional
    :param log_info: Whether to log the time taken to load the features.
    :type log_info: bool, optional
    :param get_info: Whether to return additional information (feature shapes and filenames).
    :type get_info: bool, optional
    :param file_type: The type of feature file ('npy', 'mntx', 'tpt'). If None, inferred from file extension.
    :type file_type: str, optional
    :return: A NumPy array containing the loaded feature vectors, or a tuple with additional information if get_info is True.
    :rtype: np.ndarray or tuple
    """
    feature_list: List[np.ndarray] = []
    feature_shapes: List[int] = []
    filenames: List[str] = []
    files_loaded = 0

    if log_info:
        start_time = time.perf_counter()

    file_loaders = {
        'npy': load_npy_file,
        'mntx': load_mntx_file,
        'tpt': load_tpt_file
    }

    if os.path.isfile(path):
        # If the path is a single file
        if file_type is None:
            file_type = os.path.splitext(path)[1][1:]  # Infer file type from extension
        if file_type in file_loaders:
            feature_vector = file_loaders[file_type](path)
            feature_list.append(feature_vector)
            feature_shapes.append(feature_vector.shape[0])
            files_loaded = 1
            filenames.append(path)
        else:
            raise ValueError(f"Unsupported file type: {file_type}")
    elif os.path.isdir(path):
        # If the path is a directory
        for filename in os.listdir(path):
            if max_files is not None and files_loaded >= max_files:
                break
            filepath = os.path.join(path, filename)
            if file_type is None:
                file_type = os.path.splitext(filename)[1][1:]
            if file_type in file_loaders and filename.endswith(f'.{file_type}'):
                feature_vector = file_loaders[file_type](filepath)
                if feature_vector.shape[0] == 0:
                    continue
                feature_list.append(feature_vector)
                feature_shapes.append(feature_vector.shape[0])
                files_loaded += 1
                filenames.append(filename)
            else:
                continue
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


def load_npy_file(filepath: str) -> np.ndarray:
    return np.load(filepath)

def load_mntx_file(filepath: str) -> np.ndarray:
    """
    Loads feature vectors from a .mntx file.

    :param filepath: The path to the .mntx file.
    :type filepath: str
    :return: A NumPy array containing the loaded and normalized feature vectors.
    :rtype: np.ndarray
    """
    with open(filepath, 'r') as file:
        lines = file.readlines()

    # The first line contains the number of features and the first three values
    header = lines[1].strip().split()
    feature_num = int(header[0])
    val1, val2, val3 = map(float, header[1:4])

    # Initialize an empty list to store the feature vectors
    feature_vectors = []

    # Process each feature line
    for line in lines[2:]:
        parts = line.strip().split()
        x, y, theta, score = map(float, parts[:4])
        z_values = list(map(int, parts[4:132]))  # z1 to z128

        # Normalize the z_values and convert to float32
        z_values = np.array(z_values, dtype=np.float32)
        z_values /= np.linalg.norm(z_values)

        feature_vectors.append(z_values)

    # Convert the list of feature vectors to a NumPy array
    feature_vectors = np.array(feature_vectors, dtype=np.float32)

    return feature_vectors

def load_tpt_file(filepath: str) -> np.ndarray:
    return load_mntx_file(filepath)

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