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

class KNNResult:
    def __init__(self, knn_list):
        # Flat the list
        self.knn_list = []
        for knn_result in knn_list:
            self.knn_list += knn_result
    
    def pick_best(self, k, method):
        if method == "frequency":
            return self.pick_best_frequency(k)
        elif method == "distance":
            return self.pick_best_distance(k)

    def pick_best_frequency(self, k):
        freq = {}
        for id, ind_id, d in self.knn_list:
            if ind_id not in freq:
                freq[ind_id] = 0
            freq[ind_id] += 1
        
        # Return the k most frequent individuals using tuple (id, freq)
        freq = [(k, v) for k, v in freq.items()]
        freq = sorted(freq, key=lambda x: x[1], reverse=True)
        
        best = [x for x in freq[:k]]
        return best

    def pick_best_distance(self, k):
        sorted_knn = sorted(self.knn_list, key=lambda x: x[2])
        # Return the k closest *different* individuals
        best = []
        for id, ind_id, d in sorted_knn:
            if ind_id not in best:
                best.append((ind_id, d))
            if len(best) == k:
                break
        return best
    
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

# Generate queries
num_queries = 2
queries = jf.data.generate_unit_vectors(num_queries, dimensions)
queries = queries.astype(np.float32)
print("Queries\n", queries)
np.save(path + "\\teste1\\queries.npy", queries)

# Define the index distance function, considering the mean and std of each individual
def distance(query, data_point, log=False):
    query_shifted = query * data_point.individual.std + data_point.individual.mean
    print("Comparing", np.round(query_shifted, 3), np.round(data_point.vec, 3))
    return np.linalg.norm(query_shifted - data_point.vec)

# Perform the queries
results = []
for query in queries:
    knn = []
    for data_point in index:
        d = distance(query, data_point, log=True)
        knn.append((data_point.id, data_point.individual.id, d))

    knn.sort(key=lambda x: x[2])
    print("KNN")
    for id, ind_id, d in knn:
        print(id, ind_id, d, end="; ")
    print()
    
    results.append(knn)

print(results)
# For each list in results, pick the first k elements
k = 3
new_results = []
for knn_result in results:
    new_results.append(knn_result[:k])

# Pick the best individual
best = KNNResult(new_results).pick_best(2, "frequency")
print("Best by frequency")
print(best)

best = KNNResult(new_results).pick_best(2, "distance")
print("Best by distance")
print(best)