import numpy as np


def euclidean_distance(a: np.ndarray, b: np.ndarray) -> float:
    """
    Computes the Euclidean distance between two vectors.
    
    :param a: The first vector.
    :type a: np.ndarray
    :param b: The second vector.
    :type b: np.ndarray
    :return: The Euclidean distance between the two vectors.
    :rtype: float
    """
    return np.linalg.norm(a - b)

def cosine_distance(a: np.ndarray, b: np.ndarray) -> float:
    """
    Computes the cosine distance between two vectors.
    
    :param a: The first vector.
    :type a: np.ndarray
    :param b: The second vector.
    :type b: np.ndarray
    :return: The cosine distance between the two vectors.
    :rtype: float
    """
    return 1 - np.dot(a, b) / (np.linalg.norm(a) * np.linalg.norm(b))

def cosine_distanceU(a: np.ndarray, b: np.ndarray) -> float:
    """
    Computes the cosine distance between two unit vectors.
    
    :param a: The first unit vector.
    :type a: np.ndarray
    :param b: The second unit vector.
    :type b: np.ndarray
    :return: The cosine distance between the two unit vectors.
    :rtype: float
    """
    return 1 - np.dot(a, b)