/* Copyright 2003-2017 GBDI-ICMC-USP <caetano@icmc.usp.br>
* 
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//---------------------------------------------------------------------------
// app.h - Implementation of the application.
//
// Authors: Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
// Edited by: João Felipe Contreras - LIDS, Unicamp
// Copyright (c) 2003 GBDI-ICMC-USP
//---------------------------------------------------------------------------
#ifndef appH
#define appH

#include <chrono>
#include <sstream>
#include <string.h>
#include <fstream>

#include "npy.hpp"

// Metric Tree includes
#include <arboretum/stPlainDiskPageManager.h>
#include <arboretum/stSlimTree.h>
#include <hermes/EuclideanDistance.h>

// Object type
#include <util/BasicArrayObject.h> 

using namespace std;

typedef BasicArrayObject< float > stArray;
typedef EuclideanDistance< stArray > L2;


//---------------------------------------------------------------------------
// class TApp
//---------------------------------------------------------------------------
class TApp{
   public:

      typedef stResult < stArray > Result;
      typedef stMetricTree < stArray, L2 > MetricTree;
      typedef stSlimTree < stArray, L2 > SlimTree;

      TApp(){
         PageManager = NULL;
         Tree = NULL;
      }

      /**
      * Initializes the application.
      *
      * @param pageSize
      * @param minOccup
      * @param quantidade
      * @param prefix
      */
      void Init(){
         CreateDiskPageManager();
         CreateTree();
      }

      /**
      * Runs the application.
      *
      * string galleryPath and queryPath
      */
      void Run(string galleryPath, string queryPath);

      /**
      * Deinitialize the application.
      */
      void Done();

   private:


      stPlainDiskPageManager * PageManager;

      MetricTree * Tree;

      vector <stArray *> queryObjects;


      vector<string> getFilesInDirectory(const string &directoryPath);

      /**
      * Creates a disk page manager. It must be called before CreateTree().
      */
      void CreateDiskPageManager();

      /**
      * Creates a tree using the current PageManager.
      */
      void CreateTree();

      /**
      * Loads the tree from file with a set of cities.
      */
      void LoadTree(string galleryPath);

      /**
      * Loads the vector for queries.
      */
      void LoadQueries(string queryPath);

      /**
      * Performs the queries and outputs its results.
      */
      void PerformQueries();

      void PerformNearestQuery();

      void PerformRangeQuery();

};//end TApp

#endif //end appH
