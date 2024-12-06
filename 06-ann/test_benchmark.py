GALLERY = [r"C:\Users\jfcmp\Documentos\Griaule\data\g1", r"C:\Users\jfcmp\Documentos\Griaule\data\g2"]
QUERY = [r"C:\Users\jfcmp\Documentos\Griaule\data\g2"]

import numpy as np
import jfingerprint as jf
from jfingerprint.benchmark import Benchmark

gallery, query = jf.data.load_data_set(GALLERY, QUERY, cache=True, log_info=True)

print(gallery.features.shape)
print(query.features.shape)

# Save .npy for gallery and query
np.save("gallery.npy", gallery.features)
np.save("query.npy", query.features)

bench = Benchmark(gallery, query)

print(bench.gallery.number_of_features)
print(bench.query.number_of_features)
