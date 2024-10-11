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
// app.cpp - Implementation of the application.
//
// To change the behavior of this application, comment and uncomment lines at
// TApp::Init() and TApp::Query().
//
// Authors: Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
// Copyright (c) 2003 GBDI-ICMC-USP
//---------------------------------------------------------------------------
#include <iostream>
#pragma hdrstop
#include "app.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// Class TApp
//------------------------------------------------------------------------------
void TApp::CreateTree()
{
   // create for Slim-Tree
   Tree = new myDummyTree(PageManager);
} // end TApp::CreateTree

//------------------------------------------------------------------------------
void TApp::CreateDiskPageManager()
{

   isTreeCreated = exists("DummyTree.dat");
   if (isTreeCreated)
   {
      // Open existing file
      PageManager = new stPlainDiskPageManager("DummyTree.dat");
   }
   else
   {
      // Create new file
      PageManager = new stPlainDiskPageManager("DummyTree.dat", 2048*4);
   }

} // end TApp::CreateDiskPageManager

//------------------------------------------------------------------------------
void TApp::Run()
{
   // Lets load the tree with a lot values from the file.
   LoadTree(DATAFILE);

   cout << "Height of Tree: " << Tree->GetHeight() << "\n";
   cout << "Node count: " << Tree->GetNodeCount() << "\n";
   cout << "Min/Max node occupation: " << Tree->GetMinOccupation() << "/" << Tree->GetMaxOccupation() << "\n\n";

   LoadVectorFromFile(QUERYFILE);

   if (queryObjects.size() > 0)
   {
      // Do 500 queries.
      PerformQueries();
   } // end if
   // Hold the screen.
   cout << "\n\nEND.\n";
} // end TApp::Run

//------------------------------------------------------------------------------
void TApp::Done()
{

   if (this->Tree != NULL)
   {
      delete this->Tree;
   } // end if
   if (this->PageManager != NULL)
   {
      delete this->PageManager;
   } // end if

   // delete the vetor of queries.
   for (unsigned int i = 0; i < queryObjects.size(); i++)
   {
      delete (queryObjects.at(i));
   } // end for
} // end TApp::Done

//------------------------------------------------------------------------------
void TApp::LoadTree(char *fileName)
{
   vector<float> data;
   vector<unsigned long> shape;
   myArray *array;
   clock_t start, end;

   if (isTreeCreated){
      cout << "Loaded tree with " << Tree->GetNumberOfObjects() << " objects\n\n";
      return;
   }

   if (Tree != NULL)
   {
      // Go through all files in the directory
      start = clock();
      npy::npy_data<float> d = npy::read_npy<float>(fileName);
      data = d.data;
      shape = d.shape;

      cout << "Loading data with shape: " << shape[0] << "x" << shape[1] << "\n";
      uint32_t noElemen = 0;
      // Go through the lines of matrix
      for (uint32_t i = 0; i < shape[0]; i++)
      {
         // Get ith line from data matrix
         vector<float> feature(data.begin() + i * shape[1], data.begin() + (i + 1) * shape[1]);

         array = new myArray(i, feature);
         Tree->Add(array);
         noElemen++;

         if (i % (shape[0] / 100) == 0)
         {
            int progress = (i * 100) / shape[0];
            cout << "\r" << progress << "% completed" << flush;
         }
         delete array;
      }
      end = clock();
      cout << "Total Time: " << ((double)end - (double)start) / CLOCKS_PER_SEC << "s.\n";
      cout << "Added " << Tree->GetNumberOfObjects() << " objects to tree (" << noElemen << ")\n\n";
   }
   else
   {
      cout << "Zero object added!!\n";
   }
}

//------------------------------------------------------------------------------
void TApp::LoadVectorFromFile(char *fileName)
{
   vector<float> data;
   vector<unsigned long> shape;
   clock_t start, end;

   // clear before using.
   queryObjects.clear();

   npy::npy_data<float> d = npy::read_npy<float>(fileName);
   data = d.data;
   shape = d.shape;

   cout << "Loading query objects with shape: " << shape[0] << "x" << shape[1] << "\n";
   // Go through the lines of matrix
   for (uint32_t i = 0; i < shape[0]; i++)
   {
      // Get ith line from data matrix
      vector<float> feature(data.begin() + i * shape[1], data.begin() + (i + 1) * shape[1]);

      this->queryObjects.insert(queryObjects.end(), new myArray(i, feature));
   }

   cout << "Added " << queryObjects.size() << " query objects\n\n";
} // end TApp::LoadVectorFromFile

//------------------------------------------------------------------------------
void TApp::PerformQueries()
{
   if (Tree)
   {
      // cout << "\nStarting Statistics for Range Query with DummyTree.... ";
      // PerformRangeQuery();
      // cout << " Ok\n";

      PerformNearestQuery();
   } // end if
} // end TApp::PerformQuery

//------------------------------------------------------------------------------
void TApp::PerformRangeQuery()
{

   myResult *result;
   double radius;
   // clock_t start, end;
   unsigned int size;
   unsigned int i;

   if (Tree)
   {
      size = queryObjects.size();
      // reset the statistics
      PageManager->ResetStatistics();
      Tree->GetMetricEvaluator()->ResetStatistics();
      // start = clock();
      std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
      for (i = 0; i < size; i++)
      {
         result = Tree->RangeQuery(queryObjects[i], 0.2);
         delete result;
      } // end for
      // end = clock();
      std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
      // cout << "\nTotal Time: " << ((double )end-(double )start) / 1000.0 << "(s)";
      cout << "\nTotal Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]";
      // is divided for queryObjects to get the everage
      cout << "\nAvg Disk Accesses: " << (double)PageManager->GetReadCount() / (double)size;
      // is divided for queryObjects to get the everage
      cout << "\nAvg Distance Calculations: " << (double)Tree->GetMetricEvaluator()->GetDistanceCount() / (double)size;
   } // end if

} // end TApp::PerformRangeQuery

//------------------------------------------------------------------------------
void TApp::PerformNearestQuery()
{

   myResult *result;
   clock_t start, end;
   clock_t start2, end2;
   unsigned int size;
   unsigned int i;
   unsigned int k;

   if (Tree)
   {
      size = queryObjects.size();
      cout << "Performing " << size << " queries with DummyTree\n";
      PageManager->ResetStatistics();
      Tree->GetMetricEvaluator()->ResetStatistics();
      start = clock();
      for (k = 0; k < 1; k++){
      for (i = 0; i < size; i++)
      {
         clock_t start2 = clock();
         result = Tree->NearestQuery(queryObjects[i], 32);
         clock_t end2 = clock();
         cout << "Single Query Time: " << ((double)end2 - (double)start2) / (CLOCKS_PER_SEC / 1000) << "(mili second)\n";
         if (i == 0 && k == 0)
         {
            cout << "k = " << result->GetNumOfEntries() << "\n";
            for (int j = 0; j < result->GetNumOfEntries(); j++)
            {
               // Output distance with 4 decimal places
               cout << "(" << result->GetPair(j)->GetObject()->GetOID() << ", " << std::fixed << std::setprecision(4) << result->GetPair(j)->GetDistance() << ") ";
               if ((j + 1) % 8 == 0)
                  cout << "\n";
               // Reset fixed
               cout << std::resetiosflags(std::ios::fixed);
            }
         }
         delete result;
      } // end for
      }
      cout << "\n\n";
      end = clock();
      cout << "Total Time: " << ((double)end - (double)start) / (CLOCKS_PER_SEC / 1000) << "(mili second)\n";
      // is divided for queryObjects to get the everage
      cout << "Avg Disk Accesses: " << (double)PageManager->GetReadCount() / (double)size << "\n";
      // is divided for queryObjects to get the everage
      cout << std::fixed << std::setprecision(0);
      cout << "Avg Distance Calculations: " << (double)Tree->GetMetricEvaluator()->GetDistanceCount() / (double)size << "\n";
      cout << std::resetiosflags(std::ios::fixed);
   } // end if
} // end TApp::PerformNearestQuery
