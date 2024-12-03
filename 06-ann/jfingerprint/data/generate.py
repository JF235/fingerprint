import numpy as np

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