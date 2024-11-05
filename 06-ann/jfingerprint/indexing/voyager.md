# class voyager.Index(space: Space, num_dimensions: int, M: int = 12, ef_construction: int = 200, random_seed: int = 1, max_elements: int = 1, storage_data_type: StorageDataType = StorageDataType.Float32)

A nearest-neighbor search index containing vector data (i.e. lists of floating-point values, each list labeled with a single integer ID).

Think of a Voyager Index as a Dict[int, List[float]] (a dictionary mapping integers to lists of floats), where a query() method allows finding the k nearest int keys to a provided List[float] query vector.

- space

    The Space to use to calculate the distance between vectors, Space.Cosine (cosine distance).

    The choice of distance to use usually depends on the kind of vector data being inserted into the index. If your vectors are usually compared by measuring the cosine distance between them, use Space.Cosine.

- num_dimensions

    The number of dimensions present in each vector added to this index. Each vector in the index must have the same number of dimensions.

- M

    The number of connections between nodes in the tree’s internal data structure. Larger values give better recall, but use more memory. This parameter cannot be changed after the index is instantiated.

- ef_construction

    The number of vectors to search through when inserting a new vector into the index. Higher values make index construction slower, but give better recall. This parameter cannot be changed after the index is instantiated.

## add_items(vectors: ndarray[Any, dtype[float32]], ids: list[int] | None = None, num_threads: int = -1) → list[int]

Add multiple vectors to this index simultaneously.

This method may be faster than calling add_items() multiple times, as passing a batch of vectors helps avoid Python’s Global Interpreter Lock.

- vectors
    A 32-bit floating-point NumPy array, with shape (num_vectors, num_dimensions).

    If using the Space.Cosine Space, these vectors will be normalized before insertion. If using a StorageDataType other than StorageDataType.Float32, these vectors will be converted to the lower-precision data type after normalization.

- id
    An optional list of IDs to assign to these vectors. If provided, this list must be identical in length to the first dimension of vectors. If not provided, each vector’s ID will automatically be generated based on the number of elements already in this index.

- num_threads 

    Up to num_threads will be started to perform insertions in parallel. If vectors contains only one query vector, num_threads will have no effect. By default, one thread will be spawned per CPU core.

## query(vectors: ndarray[Any, dtype[float32]], k: int = 1, num_threads: int = -1, query_ef: int = -1) → tuple[ndarray[Any, dtype[uint64]], ndarray[Any, dtype[float32]]]

Query this index to retrieve the k nearest neighbors of the provided vectors.

Parameters:

- vectors – A 32-bit floating-point NumPy array, with shape (num_dimensions,) or (num_queries, num_dimensions).

- k – The number of neighbors to return.

- num_threads – If vectors contains more than one query vector, up to num_threads will be started to perform queries in parallel. If vectors contains only one query vector, num_threads will have no effect. Defaults to using one thread per CPU core.

- query_ef – The depth of search to perform for this query. Up to query_ef candidates will be searched through to try to find up the k nearest neighbors per query vector.

Returns:

- A tuple of (neighbor_ids, distances). If a single query vector was provided, both neighbor_ids and distances will be of shape (k,).

- If multiple query vectors were provided, both neighbor_ids and distances will be of shape (num_queries, k), ordered such that the i-th result corresponds with the i-th query vector.

## save(output_path: str)

## static load(filename: str) → Index

# class voyager.Space(value)

The method used to calculate the distance between vectors.

- Euclidean = 0

    Euclidean distance; also known as L2 distance. The square root of the sum of the squared differences between each element of each vector.

- InnerProduct = 1

    Inner product distance.

- Cosine = 2

    Cosine distance; also known as normalized inner product.

#   class voyager.StorageDataType(value)

- Float8 = 16

    8-bit fixed-point decimal values. All values must be within [-1, 1.00787402].

- Float32 = 32

    32-bit floating point (default).

- E4M3 = 48

    8-bit floating point with a range of [-448, 448], from the paper “FP8 Formats for Deep Learning” by Micikevicius et al.
