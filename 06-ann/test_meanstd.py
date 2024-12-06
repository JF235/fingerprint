import jfingerprint as jf
import numpy as np

path = r"C:\Users\jfcmp\Documentos\Griaule\data\teste1"

features, shapes, filenames = jf.data.load_features(path, log_info=True, cache=False)

features = features.astype(np.float32)
# For each feature (with shapes[0] features per image), compute the mean and standard deviation
prev = 0
for i in range(len(shapes)):
    feature = features[prev:prev+shapes[i]]
    mean = np.mean(feature, axis=0, dtype=np.float32)
    std = np.std(feature, axis=0, dtype=np.float32)
    prev += shapes[i]
    print(f"Image {i+1} - Mean: {np.mean(mean)}, Std: {np.mean(std)}")