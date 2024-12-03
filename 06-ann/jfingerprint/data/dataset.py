import os
import numpy as np
from typing import List, Optional, Union

from .loaders import load_features

class Dataset:
    def __init__(self, features: np.ndarray, feature_shapes: List[int], filenames: List[str], cache_name: str):
        self.features = features
        self.feature_shapes = feature_shapes
        self.filenames = filenames
        self.fileIdx = fileIdx(feature_shapes)
        self.number_of_features = features.shape[0]
        self.cache_name = cache_name


def fileIdx(shapes: List[int]) -> List[int]:
    """
    Create a list that maps a single feature to the file it belongs to.
    
    Given shapes = [shape1, shape2, shape3, ...]
    Create a list that maps a single feature index to the index of the file it belongs to.
    
    :param shapes: A list containing the number of features in each file.
    :type shapes: List[int]
    :return: A list that maps a single feature to the file it belongs to.
    :rtype: List[int]
    """
    
    fileIdx = []
    for i in range(len(shapes)):
        fileIdx += [i] * shapes[i]
    return np.array(fileIdx)

def load_data_set(gallery_path: Union[List[str], str], query_path: Union[List[str], str], cache: bool = True, log_info: bool = True) -> tuple[Dataset, Dataset]:
    g_cn = "gallery_tmp"
    q_cn = "query_tmp"
    
    # Check if cache is exist
    if os.path.exists(g_cn + '.pkl') and os.path.exists(q_cn + '.pkl'):
        gallery_path = g_cn
        query_path = q_cn
    
    gallery_features, gallery_shapes, gallery_filenames = load_features(gallery_path, cache=cache, log_info=log_info, cache_name=g_cn)
    query_features, query_shapes, query_filenames = load_features(query_path, cache=cache, log_info=log_info, cache_name=q_cn)
    
    gallery = Dataset(gallery_features, gallery_shapes, gallery_filenames, cache_name=g_cn)
    query = Dataset(query_features, query_shapes, query_filenames, cache_name=q_cn)
    
    return gallery, query