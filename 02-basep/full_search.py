from jfingerprint.indexing import *
from jfingerprint.plotting import *
import pickle

gallery_dataset = {"path": "/data/gallery/", "img_path": "images/", "patch_path": "patches/", "mnt_path": "mnts/", "features_path": "features/ResNet18_1013_213017/"}
queries_dataset = {"path": "/data/query_basep/", "img_path": "images/", "patch_path": "patches/", "mnt_path": "mnts/", "features_path": "features/ResNet18_1013_213017/"}

QDS = load_dataset_names(queries_dataset)
GDS = load_dataset_names(gallery_dataset)

with open('data/mtree_full.pkl', 'rb') as f:
    # 6min para criar
    mtree = pickle.load(f)

full_search(QDS, mtree, queries_dataset, no_queries = 60)