from mtree import MTree
from jfingerprint.plotting import *
from jfingerprint.indexing import *
import numpy as np
import matplotlib.pyplot as plt
from scipy.spatial import distance
from collections import namedtuple
import os
import pickle

plt.style.use('dark_background')

gallery_dataset = {"path": "/data/gallery/", "img_path": "images/", "patch_path": "patches/", "mnt_path": "mnts/", "features_path": "features/ResNet18_1013_213017/"}
queries_dataset = {"path": "/data/query_basep/", "img_path": "images/", "patch_path": "patches/", "mnt_path": "mnts/", "features_path": "features/ResNet18_1013_213017/"}

N = 7065

print(f"Building MTree of size {N}")

mtree = build_mtree(gallery_dataset, tree_max_size=N)

with open(f'/work/data/mtree_full.pkl', 'wb') as output:
    pickle.dump(mtree, output)