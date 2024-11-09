import jfingerprint as jf
import numpy as np
from jfingerprint.printing import format_time
import os
import sys
from math import ceil
import pickle
from voyager import Space

if len(sys.argv) < 2:
    print('Usage: python test.py <searcher>')
    sys.exit(1)

SEARCHER = sys.argv[1]

LOADED_ONCE = os.path.exists("dsinfo.pkl")

if not LOADED_ONCE:
    GALLERYPATH = r"C:\Users\Joao\Documents\Repos\Griaule\Dados\gallery_ResNet18_1013_213017"
    QUERYPATH = r"C:\Users\Joao\Documents\Repos\Griaule\Dados\querybasep_ResNet18_1013_213017"

    gallery, gallery_shapes, gallery_names = jf.data.load_features(GALLERYPATH, log_info=True, get_info=True)
    query, query_shapes, query_names = jf.data.load_features(QUERYPATH, log_info=True, get_info=True)

    idx2file = []
    for i in range(len(gallery_names)):
        gidx = gallery_names[i].split("-")[0]
        idx2file += [gidx] * gallery_shapes[i]
    with open("dsinfo.pkl", "wb") as f:
        pickle.dump((gallery_names, gallery_shapes, query_names, query_shapes, idx2file), f)
else:
    GALLERYPATH = r"C:\Users\Joao\Documents\Repos\Griaule\Dados\gallery.npy"
    QUERYPATH = r"C:\Users\Joao\Documents\Repos\Griaule\Dados\query.npy"    

    gallery = jf.data.load_features(GALLERYPATH, log_info=True)
    query = jf.data.load_features(QUERYPATH, log_info=True)

    with open("dsinfo.pkl", "rb") as f:
        gallery_names, gallery_shapes, query_names, query_shapes, idx2file = pickle.load(f)

if SEARCHER == "ball_tree":
    S = jf.indexing.BallTreeSearcher("euclidean")
    if os.path.exists("ball_tree.pkl"):
        S.load("ball_tree.pkl")
    else:
        S.build(gallery, log_time=True)
        S.save("ball_tree.pkl")
    print("Loaded Ball-Tree with", S.index.data.shape)
    search_kwargs = dict(dualtree = True)
elif SEARCHER == "annoy":
    S = jf.indexing.AnnoySearcher(128, distance = "angular")
    if os.path.exists("annoy.ann"):
        S.load("annoy.ann")
    else:
        S.build(gallery, log_time=True, n_trees=128*4, n_jobs=-1)
        S.save("annoy.ann")
    print("Loaded Annoy with", S.index.get_n_items(), "trees =", S.index.get_n_trees())
    search_kwargs = dict(search_k=600)
elif SEARCHER == "voyager":
    M = 24
    ef_construction = 800
    S = jf.indexing.VoyagerSearcher(space = Space.Cosine, num_dimensions = 128, M = M, ef_construction = ef_construction)
    if os.path.exists(f"voyager_{M}_{ef_construction}.voy"):
        S.load(f"voyager_{M}_{ef_construction}.voy")
    else:
        S.build(gallery, log_time=True, num_threads=-1) # -1 means use 1 thread per core
        S.save(f"voyager_{M}_{ef_construction}.voy")
    print("Loaded Voyager with", S.index.num_elements, "(ef_construction, M) =", (S.index.ef_construction, S.index.M))
    search_kwargs = dict(query_ef=20, num_threads=12)

k = 20
qprev = 0
for i in range(len(query_shapes)):
    q = query[qprev:qprev + query_shapes[i]]
    qprev = qprev + query_shapes[i]
    print(f"Query {i + 1}:", q.shape[0])
    result, dist = S.search(q, k, **search_kwargs)

    # If the result is empty, skip the query
    if not result:
        continue

    # If the result and dist is just a single element, convert it to a list
    if not isinstance(result[0], list):
        result = [result]
        dist = [dist]
    print("Total Time:", format_time(S.last_result["time_seconds"]))
    print("Avg Time:", format_time(S.last_result["average_time_seconds"]))
    # For all list of neighbors in result, create a dictionary with index being the key and the value being the number of times it appears in result
    result_dict = {}
    for r in result:
        for idx in r:
            if idx2file[idx] in result_dict:
                result_dict[idx2file[idx]] += 1
            else:
                result_dict[idx2file[idx]] = 1
    # Sort the dictionary by value
    sorted_result = sorted(result_dict.items(), key=lambda x: x[1], reverse=True)
    # Remove all results with less than 10% of time
    sorted_result = [x for x in sorted_result if x[1] >= int(len(q) * 0.1)]
    print(query_names[i].split('-')[0], ":")
    print(sorted_result)