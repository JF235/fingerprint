import jfingerprint as jf
import numpy as np
from jfingerprint.printing import format_time
import os
import pickle
from voyager import Index, StorageDataType, Space

if False:
    GALLERYPATH = r"C:\Users\jfcmp\Documentos\Griaule\data\gallery_ResNet18_1013_213017"
    QUERYPATH = r"C:\Users\jfcmp\Documentos\Griaule\data\querybasep_ResNet18_1013_213017"

    gallery, gallery_shapes, gallery_names = jf.data.load_features(GALLERYPATH, log_info=True, get_info=True)
    query, query_shapes, query_names = jf.data.load_features(QUERYPATH, log_info=True, get_info=True)
else:
    GALLERYPATH = r"C:\Users\jfcmp\Documentos\Griaule\data\gallery.npy"
    QUERYPATH = r"C:\Users\jfcmp\Documentos\Griaule\data\query.npy"    

    gallery = jf.data.load_features(GALLERYPATH, log_info=True)
    query = jf.data.load_features(QUERYPATH, log_info=True)


if not os.path.exists("dsinfo.pkl"):
    idx2file = []
    for i in range(len(gallery_names)):
        gidx = gallery_names[i].split("-")[0]
        idx2file += [gidx] * gallery_shapes[i]
    with open("dsinfo.pkl", "wb") as f:
        pickle.dump((gallery_names, gallery_shapes, query_names, query_shapes, idx2file), f)
else:
    with open("dsinfo.pkl", "rb") as f:
        gallery_names, gallery_shapes, query_names, query_shapes, idx2file = pickle.load(f)

# S = jf.indexing.NaiveSearcher()
# S.build(gallery)
# print(S.data.shape)

if False:
    S = jf.indexing.BallTreeSearcher("euclidean")
    if os.path.exists("ball_tree.pkl"):
        S.load("ball_tree.pkl")
    else:
        S.build(gallery, log_time=True)
        S.save("ball_tree.pkl")
    print("Loaded Ball-Tree with", S.data.shape)
    search_kwargs = {}

if False:
    S = jf.indexing.AnnoySearcher(128, distance = "angular")
    if os.path.exists("annoy.ann"):
        S.load("annoy.ann")
    else:
        S.build(gallery, log_time=True, n_trees=128*10)
        S.save("annoy.ann")
    search_kwargs = dict(search_k=5000)

if True:
    S = jf.indexing.VoyagerSearcher(space = Space.Cosine, num_dimensions = 128, M = 52, ef_construction = 4000)
    if os.path.exists("voyager.voy"):
        S.load("voyager.voy")
    else:
        S.build(gallery[:70000], log_time=True)
        S.save("voyager.voy")
    search_kwargs = dict(ef_search=1000)

# S = jf.indexing.KdTree("euclidean")
# if os.path.exists("kd_tree.pkl"):
#     S.load("kd_tree.pkl")
# else:
#     S.build(gallery, log_time=True)
#     S.save("kd_tree.pkl")
# print(S.data.shape)

k = 20
qprev = 0
for i in range(3):
    q = query[qprev:qprev + query_shapes[i]]
    qprev = qprev + query_shapes[i]
    print(f"Query {i + 1}:", q.shape[0])
    result, dist = S.search(q, k, **search_kwargs)
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
    # Remove all results with less than 50% of time
    sorted_result = [x for x in sorted_result if x[1] >= int(len(q) * 0.1)]
    print(query_names[i].split('-')[0], ":")
    print(sorted_result)