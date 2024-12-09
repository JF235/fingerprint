import jfingerprint as jf
import numpy as np

path = r"C:\Users\jfcmp\Documentos\Griaule\data"

class Feature:
    _next_id = 1
    def __init__(self, vec):
        self.id = Feature._next_id
        Feature._next_id += 1
        self.vec = vec
    
    def set_individual(self, individual):
        self.individual = individual
    
    def reset_id():
        Feature._next_id = 1

class Individual:
    _next_id = 1
    
    def __init__(self, features, mean, std):
        self.id = Individual._next_id
        Individual._next_id += 1
        self.features = features
        self.mean = mean
        self.std = std

# Gen data
np.random.seed(42)
individuals = []
all_features = []
num_individuals = 2
dimensions = 5
num_features = 3

for i in range(num_individuals):
    unit_vectors = jf.data.generate_unit_vectors(num_features, dimensions)
    unit_vectors = unit_vectors.astype(np.float32)
    np.save(path + "\\teste2\\" + f"individual_{i+1}.npy", unit_vectors)
    
    features = []
    for vec in unit_vectors:
        f = Feature(vec)
        features.append(f)
        all_features.append(f)
    
    mean = np.mean(unit_vectors, axis=0)
    std = np.std(unit_vectors, axis=0)
    individual = Individual(features, mean, std)
    for f in features:
        f.set_individual(individual)

print("Feature\tId\tMean\tStd")
for f in all_features:
    print(np.round(f.vec, 3), f.individual.id, np.round(f.individual.mean, 3), np.round(f.individual.std, 3))

# Build index
Feature.reset_id()
index = []
for f in all_features:
    shifted_f = Feature(f.vec * f.individual.std + f.individual.mean)
    shifted_f.set_individual(f.individual)
    index.append(shifted_f)
    

print("Id\tFeature")
for data_point in index:
    print(data_point.id, data_point.vec)

# Perform query
query = jf.data.generate_unit_vectors(1, dimensions)[0]
query = query.astype(np.float32)
print("Query\n", query)
np.save(path + "\\teste1\\query.npy", query.reshape(1, dimensions))

def distance(query, data_point, log=False):
    query_shifted = query * data_point.individual.std + data_point.individual.mean
    print("Comparing", np.round(query_shifted, 3), np.round(data_point.vec, 3))
    return np.linalg.norm(query_shifted - data_point.vec)

knn = []
for data_point in index:
    d = distance(query, data_point, log=True)
    knn.append((data_point.id, d))

knn.sort(key=lambda x: x[1])
print("KNN")
for id, d in knn:
    print(id, d)