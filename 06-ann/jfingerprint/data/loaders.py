import os
import numpy as np
import pickle
import time
from typing import List, Optional, Union

import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from jfingerprint.printing import format_time

def load_features(path: List[str] | str, max_files: int | None = None, log_info: bool = False, file_type: str | None = None, cache: bool = False, cache_name: str = "tmp_cache") -> tuple[np.ndarray, List[int], List[str]]:
    """
    Load features from files or directories.

    This function loads feature vectors from specified files or directories. It supports caching to speed up subsequent loads.

    :param path: A string or list of strings representing file paths or directories to load features from.
    :type path: List[str] | str
    :param max_files: Maximum number of files to load. If None, all files will be loaded.
    :type max_files: int, optional
    :param log_info: If True, logs information about the loading process.
    :type log_info: bool, optional
    :param file_type: The type of files to load. If None, the file type will be inferred from the file extension.
    :type file_type: str, optional
    :param cache: If True, caches the loaded features to disk for faster subsequent loads.
    :type cache: bool, optional
    :param cache_name: The base name for the cache files.
    :type cache_name: str, optional
    :return: A tuple containing the loaded features, their shapes, and the filenames.
    :rtype: tuple[np.ndarray, List[int], List[str]]
    :raises ValueError: If the path is not a string or list of strings, or if an unsupported file type is encountered.
    """
    if log_info:
        start_time = time.perf_counter()

    if cache:
        if isinstance(path, str) and os.path.exists(path + ".pkl") and os.path.exists(path + ".npy"):
            with open(path + ".pkl", "rb") as f:
                feature_shapes, filenames = pickle.load(f)
            features = np.load(path + ".npy")
                
            if log_info:
                end_time = time.perf_counter()
                elapsed_time = end_time - start_time
                print(f"Loaded {len(filenames)} cached file(s) in {format_time(elapsed_time)}")
                print(f"Shape of loaded features: {features.shape}")
            return features, feature_shapes, filenames
        print("Cache not found. Loading features from scratch.")
            
    
    feature_list: List[np.ndarray] = []
    feature_shapes: List[int] = []
    filenames: List[str] = []
    files_loaded = 0

    supported_files = ['npy', 'mntx', 'tpt']
    
    # Dealing with input
    if not isinstance(path, list) and not isinstance(path, str):
        raise ValueError("Path must be a string or a list of strings.")

    if isinstance(path, str):
        path = [path]

    files = []
    
    # Convert all directories to a list of files and append to the files list
    for p in path:
        if os.path.isdir(p):
            files += [os.path.join(p, f) for f in os.listdir(p)]
        else:
            files.append(p)
    
    # Processing sanitized input, just a list of files to load
    for f in files:
        if os.path.isfile(f):
            if file_type is None:
                file_type = os.path.splitext(f)[1][1:]
            if file_type not in supported_files:
                raise ValueError(f"Unsupported file type: {file_type}")
            
            if file_type == 'npy':
                feat_vec_array = load_npy_file(f)
            elif file_type == 'mntx':
                feat_vec_array = load_mntx_file(f)
            elif file_type == 'tpt':
                feat_vec_array = load_tpt_file(f)
            
            feature_list.append(feat_vec_array)
            feature_shapes.append(feat_vec_array.shape[0])
            files_loaded += 1
            filenames.append(os.path.basename(f))
            
            if max_files is not None and files_loaded >= max_files:
                break
        else:
            print(f"Warning: {f} is not a file.")
        
    features = np.vstack(feature_list)
    
    if log_info:
        end_time = time.perf_counter()
        elapsed_time = end_time - start_time
        print(f"Loaded {files_loaded} file(s) in {format_time(elapsed_time)}")
        print(f"Shape of loaded features: {features.shape}")

    if cache:
        with open(cache_name + ".pkl", "wb") as f:
            pickle.dump((feature_shapes, filenames), f)
        np.save(cache_name + ".npy", features)

    return features, feature_shapes, filenames


def load_npy_file(filepath: str) -> np.ndarray:
    return np.load(filepath)

def load_mntx_file(filepath: str) -> np.ndarray:
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