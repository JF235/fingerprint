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
// Edited by: João Felipe Contreras - LIDS, Unicamp
// Copyright (c) 2003 GBDI-ICMC-USP
//---------------------------------------------------------------------------
#include <iostream>
#include "app.hpp"
#include <filesystem>

namespace fs = std::filesystem;

#pragma package(smart_init)

void TApp::CreateTree()
{
   Tree = new SlimTree(PageManager);
}

void TApp::CreateDiskPageManager()
{
   PageManager = new stPlainDiskPageManager("SlimTree.dat", 2048);
}

// Run the application, loading the tree and performing the queries.
void TApp::Run(string galleryPath, string queryPath)
{
   // Load tree from .npy files
   LoadTree(galleryPath);

   // Load the queries from .npy files and perform them
   LoadVectorFromFile(queryPath);

   // if (queryObjects.size() > 0)
   //    PerformQueries();
}

// Clean up the application.
void TApp::Done()
{
   if (this->Tree != NULL)
      delete this->Tree;
   if (this->PageManager != NULL)
      delete this->PageManager;

   for (unsigned int i = 0; i < queryObjects.size(); i++)
   {
      delete (queryObjects.at(i));
   }
}

vector<string> getFilesInDirectory(const string &directoryPath)
{
   vector<string> files;
   for (const auto &entry : fs::directory_iterator(directoryPath))
   {
      if (fs::is_regular_file(entry.status()))
      {
         files.push_back(entry.path().string());
      }
   }
   return files;
}

//------------------------------------------------------------------------------
void TApp::LoadTree(string galleryPath)
{
   vector<float> data;
   vector<unsigned long> shape;
   uint64_t id;
   stArray *a;
   clock_t start, end;

   // Get all file paths inside the galleryPath directory
   vector<string> files = getFilesInDirectory(galleryPath);

   if (Tree != NULL)
   {
      // Go through all files in the directory
      start = clock();
      for (const auto &filePath : files)
      {
         // clear before using.
         data.clear();
         shape.clear();

         npy::npy_data<float> d = npy::read_npy<float>(filePath);

         data = d.data;
         shape = d.shape;

         cout << shape[0] << " from " << filePath << endl;

         // Go through the lines of matrix
         for (uint64_t i = 0; i < shape[0]; i++)
         {
            // Get ith line from data matrix
            vector<float> feature(data.begin() + i * shape[1], data.begin() + (i + 1) * shape[1]);

            // Insert feature vector in the tree
            a = new stArray(i, feature);
            Tree->Add(a);
            delete a;
         }
      }
      end = clock();
      cout << "\nTotal Time: " << ((double)end - (double)start) / (CLOCKS_PER_SEC / 1000) << "ms. ";
      cout << "Added " << Tree->GetNumberOfObjects() << " objects to tree\n\n";
   }
   else
   {
      cout << "Zero object added!!\n";
   }
}

void TApp::LoadVectorFromFile(string queryPath)
{
   vector<float> data;
   vector<unsigned long> shape;
   uint64_t id;
   clock_t start, end;

   // clear before using.
   queryObjects.clear();

   // Get all file paths inside the queryPath directory
   vector<string> files = getFilesInDirectory(queryPath);

   start = clock();
   for (const auto &filePath : files)
   {
      data.clear();
      shape.clear();

      npy::npy_data<float> d = npy::read_npy<float>(filePath);

      data = d.data;
      shape = d.shape;

      cout << shape[0] << " from " << filePath << endl;

      // Go through the lines of matrix
      for (uint64_t i = 0; i < shape[0]; i++)
      {
         // Get ith line from data matrix
         vector<float> feature(data.begin() + i * shape[1], data.begin() + (i + 1) * shape[1]);

         // Insert feature vector in the queryObjects vector
         this->queryObjects.insert(queryObjects.end(), new stArray(i, feature));
      }
   }
   end = clock();
   // cout << "\nTotal Time: " << ((double)end - (double)start) / (CLOCKS_PER_SEC / 1000) << "(mili second)";

   cout << "Added " << queryObjects.size() << " objects to query\n\n";
}

//------------------------------------------------------------------------------
void TApp::PerformQueries()
{
   if (Tree)
   {
      // cout << "\nStarting Statistics for Range Query with SlimTree.... ";
      // PerformRangeQuery();
      // cout << " Ok\n";

      cout << "\nStarting Statistics for Nearest Query with SlimTree.... ";
      PerformNearestQuery();
      cout << " Ok\n";
   }
}

//------------------------------------------------------------------------------
void TApp::PerformRangeQuery()
{

   Result *result;
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
      chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
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

   Result *result;
   clock_t start, end;
   unsigned int size;
   unsigned int i;
   int id;
   unsigned int K = 4;
   vector<float> vec;
   stArray *a;

   if (Tree)
   {
      size = queryObjects.size();
      PageManager->ResetStatistics();
      Tree->GetMetricEvaluator()->ResetStatistics();
      start = clock();
      for (i = 0; i < queryObjects.size(); i++)
      {

         result = Tree->NearestQuery(queryObjects[i], K);

         cout << endl;

         for (int k = 0; k < K; k++)
         {
            a = result->GetPair(k)->GetObject();
            vec = a->getData();
            id = a->getOID();
            cout << id << ": ";
            // Print data
            for (int j = 0; j < vec.size(); j++)
            {
               cout << vec[j] << " ";
            }
            cout << endl;
         }

         delete result;
      } // end for
      end = clock();
      cout << "\nTotal Time: " << ((double)end - (double)start) / (CLOCKS_PER_SEC / 1000) << "(mili second)";
      // is divided for queryObjects to get the everage
      cout << "\nAvg Disk Accesses: " << (double)PageManager->GetReadCount() / (double)size;
      // is divided for queryObjects to get the everage
      cout << "\nAvg Distance Calculations: " << (double)Tree->GetMetricEvaluator()->GetDistanceCount() / (double)size;
   } // end if
} // end TApp::PerformNearestQuery