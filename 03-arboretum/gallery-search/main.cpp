#include <ostream>
#include <iostream>
#include "app.hpp"
#include <string>
#include <vector>
#include <iomanip>


int main(int argc, char* argv[]){
   TApp app;                                         

   // argv to string
   if (argc < 3) {
      cout << "Usage: " << argv[0] << " <galleryPath> <queryPath>" << endl;
      return 1;
   }

   const string galleryPath = argv[1];
   const string queryPath = argv[2];
   
   app.Init();

   app.Run(galleryPath, queryPath);

   app.Done();

   return 0;
}
