No arquivo main.

```cpp
TApp app;                                         

// Init application.
app.Init();
// Run it.
app.Run();
// Release resources.
app.Done();
```

No init, inicio a árvore e o gerenciador de páginas.

```c++
void Init(){
    // To create it in disk
    CreateDiskPageManager();
    // Creates the tree
    CreateTree();
}//end Init
```

# Page Manager

## stPageManager

- `~stPageManager()` destrutor
- `IsEmpty()` quando isso nao eh vazio? Sempre que tem uma tree nao vazia
- `Write/GetHeaderPage` header page é uma pagina usada para a SlimTree escrever sobre si própria

## stDiskPageManager

```c++
void TApp::CreateDiskPageManager(){
   PageManager = new stDiskPageManager("SlimTree2.dat", 1024, 128, 7);
}//end TApp::CreateDiskPageManager
```

```
This class controls the Slim Tree disk access.
* This class implements the abstract class stPageManager.
*
*<P> This version uses CStorage to disk access, ie, all disk controls is made by
* CStorage.
```

```
Loading query objects  Added 500 query objects
Starting Statistics for Range Query with SlimTree.... 
Total Time: 6730365[µs]
Avg Disk Accesses: 67.692
Avg Distance Calculations: 227.862 Ok

Starting Statistics for Nearest Query with SlimTree.... 
Total Time: 1847.73(mili second)
Avg Disk Accesses: 78.292
Avg Distance Calculations: 475.228 Ok
```

## stPlainDiskPageManager

Sobre o disk page manager: No caso do stPlainDiskPageManager, especialmente para a SLIM-Tree

```c++
void TApp::CreateDiskPageManager(){
   //for SlimTree
   PageManager = new stPlainDiskPageManager("SlimTree.dat", 1024);
}//end TApp::CreateDiskPageManager
```

```
* This class implements a page manager capable to read and write disk pages. It
* was designed to provide a fair base for algorithm comparisons and is not suposed
* to be used by users which seek for performance.
*
* <p>The implementation is based on the standard I/O system calls. All read/write
* operations are performed without chaching pages. As an additional feature, it
* is possible to disable the system I/O cache in some operational systems.
```

```
Loading the query file
Loading query objects  Added 500 query objects
Starting Statistics for Range Query with SlimTree.... 
Total Time: 5740212[µs]
Avg Disk Accesses: 67.692
Avg Distance Calculations: 227.862 Ok

Starting Statistics for Nearest Query with SlimTree.... 
Total Time: 1384.17(mili second)
Avg Disk Accesses: 78.292
Avg Distance Calculations: 475.228 Ok
```

## stMemoryPageManager