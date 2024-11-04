import numpy as np
import matplotlib.pyplot as plt
from typing import List, Callable, Optional, Dict

def plot_distance_distribution(features: List[np.ndarray], distance_func: Callable[[np.ndarray, np.ndarray], float], k: Optional[int] = None, query: Optional[np.ndarray] = None, ax: Optional[plt.Axes] = None, hist_kwargs: Optional[Dict] = None):
    """
    Plots the distance distribution of features with respect to an exemplar element.

    :param features: A list of feature vectors.
    :type features: List[np.ndarray]
    :param distance_func: A function to compute the distance between two feature vectors.
    :type distance_func: Callable[[np.ndarray, np.ndarray], float]
    :param k: The index of the exemplar element. If None, a random element is chosen.
    :type k: int, optional
    :param query: An exemplar element. If None, the element at index k or a random element is chosen.
    :type query: np.ndarray, optional
    :param ax: Matplotlib Axes object to plot on. If None, a new figure and axes are created.
    :type ax: plt.Axes, optional
    :param hist_kwargs: Dictionary of keyword arguments to pass to the histogram plot.
    :type hist_kwargs: Dict, optional
    """
    if query is None:
        if k is not None:
            query = features[k]
        else:
            query = features[np.random.randint(len(features))]

    distances = [distance_func(query, feature) for feature in features]

    if ax is None:
        fig, ax = plt.subplots(figsize=(10, 6))

    if hist_kwargs is None:
        hist_kwargs = {'bins': 30, 'edgecolor': 'black'}

    ax.hist(distances, **hist_kwargs)