1. Tamanho da galeria (349 M)

```bash
$ du -sh /dados/home/andreigor/workspace/FingerprintIdentification/datasets/gallery/features/ResNet18_1013_213017/

349M    /dados/home/andreigor/workspace/FingerprintIdentification/datasets/gallery/features/ResNet18_1013_213017/
```

2. Compilação da aplicação (3 seg)

```bash
$ time make
g++ main.cpp app.cpp  -std=c++17 -L../build -I../src/include -lstdc++ -lm -larboretum -o main 

real    0m3.021s
user    0m2.710s
sys     0m0.311s
```

3. Construir árvore SlimTree na Athena.Lids (182 seg)

Tamanho da árvore em disco: ~1.47GB

Opções:

```c++
PageManager = new stPlainDiskPageManager("SlimTree.dat", 2048);
Tree = new SlimTree(PageManager);
```

```bash
$ time make run

./main /dados/home/andreigor/workspace/FingerprintIdentification/datasets/gallery/features/ResNet18_1013_213017/ /dados/home/andreigor/workspace/FingerprintIdentification/datasets/query_basep/features/ResNet18_1013_213017/ -K 64
Loading 7064 files from /dados/home/andreigor/workspace/FingerprintIdentification/datasets/gallery/features/ResNet18_1013_213017/
99% completed
Total Time: 182.665s. Added 677439 objects to tree


^Cmake: *** [Makefile:17: run] Interrupt

real    3m5.012s
user    2m46.185s
sys     0m18.784s
```

obs.: Interrompi a execução após finalização da construção da árvore.

4. Construir Dummy Tree na Athena.Lids (5 seg)

Tamanho da árvore em disco: ~441.04 MB

```bash
$ time make run
./main /dados/home/andreigor/workspace/FingerprintIdentification/datasets/gallery/features/ResNet18_1013_213017/ /dados/home/andreigor/workspace/FingerprintIdentification/datasets/query_basep/features/ResNet18_1013_213017/ -K 64
Loading 7064 files from /dados/home/andreigor/workspace/FingerprintIdentification/datasets/gallery/features/ResNet18_1013_213017/
99% completed
Total Time: 5.57207s. Added 677439 objects to tree


^Cmake: *** [Makefile:17: run] Interrupt


real    0m7.173s
user    0m5.052s
sys     0m2.121s
```

5. KNN na SlimTree (Buscando somente 1 vetor na árvore)

```bash
$ time ./main $gallerypath $querypath -K 7 -maxFeatures 1 -maxQueries 1
Elements added to Tree: 677438


Query: 1 from 1003-105731288.npy
Sample 5252 (size=2):
(26, 0.70) (19, 0.71) 
Sample 37 (size=1):
(42, 0.68) 
Sample 6 (size=1):
(42, 0.68) 
Sample 5259 (size=3):
(51, 0.64) (93, 0.67) (105, 0.69) 

, Total Time: 5570.63(ms)
5259: 2.7938 (25917-rollV07.npy)
5252: 1.9703 (25914-rollU04.npy)
37: 1.2012 (1031-95601599.npy)
6: 1.2012

real    0m5.605s
user    0m4.868s
sys     0m0.736s
```

6. KNN na DummyTree (Buscando somente 1 vetor na árvore)

```bash
$ time ./main $gallerypath $querypath -K 7 -maxFeatures 1 -maxQueries 1
Elements added to Tree: 677439


Query: 1 from 1003-105731288.npy
Sample 5252 (size=2):
(26, 0.70) (19, 0.71) 
Sample 37 (size=1):
(42, 0.68) 
Sample 6 (size=1):
(42, 0.68) 
Sample 5259 (size=3):
(51, 0.64) (93, 0.67) (105, 0.69) 

, Total Time: 4007.43(ms)
5259: 2.7938 (25917-rollV07.npy)
5252: 1.9703 (25914-rollU04.npy)
37: 1.2012 (1031-95601599.npy)
6: 1.2012

real    0m4.033s
user    0m3.761s
sys     0m0.272s
```

7. KNN na SlimTree (Buscando múltiplos vetores)

```bash
$ time ./main $gallerypath $querypath -K 7 -maxQueries 1
Elements added to Tree: 677438

Query: 14 from 1003-105731288.npy

# Resultado da busca

, Total Time: 80032.46(ms)
37: 5.9003 (1031-95601599.npy)
6: 5.4317 (1003-95601599.npy)
1301: 4.9526 (2140-63209999.npy)
4251: 3.9111
240: 3.2343
5259: 2.7938
5338: 2.7331
6561: 2.7306

real    1m20.063s
user    1m11.223s
sys     0m8.836s
```

8. KNN na DummyTree (Buscando múltiplos vetores)

```bash
time ./main $gallerypath $querypath -K 7 -maxQueries 1
Elements added to Tree: 677439

# Resultado da busca

, Total Time: 52800.22(ms)
6: 5.9003 (1003-95601599.npy)
37: 5.4317 (1031-95601599.npy)
1301: 4.9526 (2140-63209999.npy)
4251: 3.9111
240: 3.2343
5259: 2.7938
5338: 2.7331
6561: 2.7306

real    0m52.836s
user    0m50.202s
sys     0m2.628s
```

obs.: resultado diferente pode ser causa por conta de empate?