# Sugestões

**Qual a maneira mais efetiva de encontrar os k elementos mais próximos de um array numpy?**

A maneira mais eficiente de encontrar os k elementos mais próximos de um array NumPy é utilizando estruturas de dados e algoritmos otimizados para busca de vizinhos mais próximos, como o `KDTree` ou `BallTree` da biblioteca `scikit-learn`. Esses métodos são mais rápidos e escaláveis do que uma busca linear simples, especialmente para grandes conjuntos de dados.

Aqui está um exemplo de como usar o `KDTree` para encontrar os k elementos mais próximos:

### Passos:
1. Importar as bibliotecas necessárias.
2. Construir a árvore `KDTree` a partir do conjunto de dados.
3. Usar o método `query` para encontrar os k vizinhos mais próximos.

### Código:
```python
import numpy as np
from sklearn.neighbors import KDTree

# Exemplo de dados
features = np.random.rand(1000, 128)  # 1000 vetores de características, cada um com 128 dimensões
query = np.random.rand(128)  # Vetor de consulta com 128 dimensões

# Construir a árvore KDTree
tree = KDTree(features)

# Encontrar os k vizinhos mais próximos
k = 5
distances, indices = tree.query([query], k=k)

print("Índices dos k vizinhos mais próximos:", indices)
print("Distâncias dos k vizinhos mais próximos:", distances)
```

### Explicação:
- **Importações**: Importamos `numpy` para manipulação de arrays e `KDTree` da biblioteca `scikit-learn` para a estrutura de dados otimizada.
- **Dados de exemplo**: Criamos um conjunto de dados de exemplo com 1000 vetores de características e um vetor de consulta.
- **Construção da árvore**: Construímos a árvore `KDTree` a partir do conjunto de dados.
- **Busca dos vizinhos**: Usamos o método `query` para encontrar os k vizinhos mais próximos do vetor de consulta.

### Vantagens:
- **Eficiência**: `KDTree` e `BallTree` são muito mais eficientes do que uma busca linear, especialmente para grandes conjuntos de dados.
- **Escalabilidade**: Esses métodos são escaláveis e podem lidar com grandes volumes de dados de forma eficiente.

### Alternativa:
Se os dados tiverem alta dimensionalidade e `KDTree` ou `BallTree` não forem eficientes, você pode considerar o uso de algoritmos de aproximação como `Annoy` ou `FAISS`.

### Exemplo com `Annoy`:
```python
from annoy import AnnoyIndex

# Exemplo de dados
features = np.random.rand(1000, 128)
query = np.random.rand(128)

# Construir o índice Annoy
f = features.shape[1]
t = AnnoyIndex(f, 'euclidean')
for i in range(len(features)):
    t.add_item(i, features[i])
t.build(10)  # Número de árvores

# Encontrar os k vizinhos mais próximos
k = 5
indices = t.get_nns_by_vector(query, k)

print("Índices dos k vizinhos mais próximos:", indices)
```

### Explicação:
- **Importações**: Importamos `AnnoyIndex` da biblioteca `annoy`.
- **Construção do índice**: Construímos o índice `Annoy` a partir do conjunto de dados.
- **Busca dos vizinhos**: Usamos o método `get_nns_by_vector` para encontrar os k vizinhos mais próximos do vetor de consulta.

Esses métodos são altamente eficientes e recomendados para encontrar os k elementos mais próximos em grandes conjuntos de dados.

# SlimTree Arboretum

(Realizados na athena)

- Teste cidades brasil

```
Adding objects in the SlimTree
Loading objects ...................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................... Added 5507 objects 

Loading the query file
Loading query objects  Added 500 query objects 
Starting Statistics for Range Query with SlimTree.... 
Total Time: 53470[µs]
Avg Disk Accesses: 66.386
Avg Distance Calculations: 229.436 Ok

Starting Statistics for Nearest Query with SlimTree.... 
Total Time: 109.555(mili second)
Avg Disk Accesses: 75.78
Avg Distance Calculations: 464.718 Ok


Finished the whole test!
```

```
Adding objects in the DummyTree
Loading objects ...................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................... Added 5507 objects 

Loading the query file
Loading query objects  Added 500 query objects 
Starting Statistics for Range Query with DummyTree.... 
Total Time: 276110[µs]
Avg Disk Accesses: 191
Avg Distance Calculations: 5507 Ok

Starting Statistics for Nearest Query with DummyTree.... 
Total Time: 337.768(mili second)
Avg Disk Accesses: 191
Avg Distance Calculations: 5507 Ok


Finished the whole test!
```

- Teste vangogh (200x300)

```
Adding objects in the SlimTree
Total Time: 1.54162s. Added 60000 objects to tree (60000)



Loading the query file
Loading query objects  Added 16 query objects 
Starting Statistics for Nearest Query with DummyTree.... 
Total Time: 23.559(mili second)
Avg Disk Accesses: 277.625
Avg Distance Calculations: 2048.44 Ok


Finished the whole test!
```

```
Adding objects in the DummyTree
Total Time: 0.264441s. Added 60000 objects to tree (60000)



Loading the query file
Loading query objects  Added 16 query objects 
Starting Statistics for Nearest Query with DummyTree.... 
Total Time: 229.432(mili second)
Avg Disk Accesses: 1667
Avg Distance Calculations: 60000 Ok


Finished the whole test!
```

- Teste vangogh (1000x1000)

```
Adding objects in the SlimTree
99% completed
Total Time: 28.1572s. Added 1000000 objects to tree (1000000)

Height of Tree: 5
Node count: 60974
Min/Max node occupation: 0.25/28


Loading the query file
Loading query objects  Added 16 query objects 
Starting Statistics for Nearest Query with SlimTree.... Result from 0th query: 15
Distance from 438022: 0
Distance from 591078: 0
Distance from 528013: 0
Distance from 553139: 0
Distance from 539207: 0
Distance from 702286: 0
Distance from 665328: 0
Distance from 525176: 0
Distance from 578078: 0
Distance from 545192: 0
Distance from 517171: 0.00392157
Distance from 479184: 0.00392157
Distance from 529173: 0.00392157
Distance from 561134: 0.00392157
Distance from 546155: 0.00392157

Total Time: 137.331(mili second)
Avg Disk Accesses: 4381.5
Avg Distance Calculations: 20388.9 Ok


Finished the whole test!
```

```
Adding objects in the DummyTree
99% completed
Total Time: 3.56654s. Added 1000000 objects to tree (1000000)

Height of Tree: 0
Node count: 27778
Min/Max node occupation: -1/36


Loading the query file
Loading query objects  Added 16 query objects 
Starting Statistics for Nearest Query with DummyTree.... Result from 0th query: 15
Distance from 438022: 0
Distance from 525176: 0
Distance from 528013: 0
Distance from 539207: 0
Distance from 545192: 0
Distance from 553139: 0
Distance from 578078: 0
Distance from 591078: 0
Distance from 665328: 0
Distance from 702286: 0
Distance from 479184: 0.00392157
Distance from 507010: 0.00392157
Distance from 507048: 0.00392157
Distance from 517171: 0.00392157
Distance from 529173: 0.00392157

Total Time: 3899.54(mili second)
Avg Disk Accesses: 27778
Avg Distance Calculations: 1e+06 Ok


Finished the whole test!
```

- Teste vangogh Full (2009x2772)

Note que aqui a DummyTree perde para a operação de multiplicação de matriz do numpy: Total time: 8.57, Median time: 472.088 ms

```


Adding objects in the DummyTree
Loaded tree with 5468498 objects

Height of Tree: 0
Node count: 151903
Min/Max node occupation: -1/36


Loading the query file
Loading query objects  Added 16 query objects 
Starting Statistics for Nearest Query with DummyTree.... Result from 0th query: 64
Distance from 3940478: 0; 
Distance from 4111784: 0; Distance from 4111864: 0; Distance from 4130841: 0; Distance from 4177446: 0; Distance from 4185449: 0; Distance from 4215585: 0; Distance from 4217992: 0; Distance from 4231902: 0; 
Distance from 4253625: 0; Distance from 4258810: 0; Distance from 4321614: 0; Distance from 4357000: 0; Distance from 4558678: 0; Distance from 4659350: 0; Distance from 3248616: 0.00392157; ...
Total Time: 20473.3(mili second)
Avg Disk Accesses: 151903
Avg Distance Calculations: 5.4685e+06 Ok


Finished the whole test!
```

```
Adding objects in the SlimTree
99% completed
Total Time: 173.221s. Added 5468498 objects to tree (5468498)

Height of Tree: 6
Node count: 336772
Min/Max node occupation: 0.25/28


Loading the query file
Loading query objects  Added 16 query objects 
Starting Statistics for Nearest Query with SlimTree.... 
Partial Time: 49.312(mili second)Result from 0th query: 64
Distance from 4659350: 0; Distance from 4558678: 0; Distance from 4253625: 0; Distance from 4357000: 0; Distance from 4185449: 0; Distance from 4130841: 0; Distance from 4217992: 0; Distance from 4258810: 0; Distance from 4231902: 0; 
Distance from 4177446: 0; Distance from 4111784: 0; Distance from 4111864: 0; Distance from 4215585: 0; Distance from 4321614: 0; Distance from 3940478: 0; Distance from 4220739: 0.00392157;
Partial Time: 32.954(mili second)
Partial Time: 39.782(mili second)
Partial Time: 54.108(mili second)
Partial Time: 51.526(mili second)
Partial Time: 40.857(mili second)
Partial Time: 48.04(mili second)
Partial Time: 52.778(mili second)
Partial Time: 47.805(mili second)
Partial Time: 18.331(mili second)
Partial Time: 36.603(mili second)
Partial Time: 19.498(mili second)
Partial Time: 54.14(mili second)
Partial Time: 49.114(mili second)
Partial Time: 48.894(mili second)
Partial Time: 43.565(mili second)
Total Time: 687.688(mili second)
Avg Disk Accesses: 20686
Avg Distance Calculations: 97499.4 Ok


Finished the whole test!
```