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

   // Read flags
   uint32_t i = 3;
   while (i < argc) {
      // -h for help
      if (strcmp(argv[i], "-h") == 0) {
         cout << "Usage: " << argv[0] << " <galleryPath> <queryPath>" << endl;
         return 0;
      }
      
      // -K for setting the number of nearest neighbors
      if (strcmp(argv[i], "-K") == 0) {
         if (i + 1 < argc) {
            app.setK(atoi(argv[i + 1]));
            i+=2;
         } else {
            cout << "Missing argument for -K" << endl;
            return 1;
         }
      }      
   }
   
   app.Init();

   app.Run(galleryPath, queryPath);

   app.Done();

   return 0;
}
