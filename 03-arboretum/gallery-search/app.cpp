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
#include <numeric>

namespace fs = std::filesystem;

#pragma package(smart_init)

// std::vector output overload
std::ostream &operator<<(std::ostream &os, const std::vector<float> &vec)
{
   os << "[";
   for (size_t i = 0; i < vec.size(); ++i)
   {
      os << vec[i];
      if (i != vec.size() - 1)
         os << ", ";
   }
   os << "]";
   return os;
}

// ResultDict output overload
std::ostream &operator<<(std::ostream &os, const ResultDict &map)
{
   for (auto const &mapPair : map)
   {
      os << "Sample " << mapPair.first << " (size=" << mapPair.second.size() << "):" << endl;
      for (auto const &resultPair : mapPair.second)
      {
         // Print feature Id and disntance with 2 decimal places
         os << "(" << (resultPair.first & 0xFFFFFFFF) << ", " << fixed << setprecision(2) << resultPair.second << ") ";
      }
      os << "\n";
   }
   return os;
}

void TApp::CreateTree()
{
   Tree = new SlimTree(PageManager);
}

void TApp::CreateDiskPageManager()
{
   isTreeCreated = fs::exists("SlimTree.dat");
   if (isTreeCreated)
   {
      // Open existing file
      PageManager = new stPlainDiskPageManager("SlimTree.dat");
   }
   else
   {
      // Create new file
      PageManager = new stPlainDiskPageManager("SlimTree.dat", 2048);
   }
}

// Run the application, loading the tree and performing the queries.
void TApp::Run(string galleryPath, string queryPath)
{  
   this->galleryPath = galleryPath;
   if (!isTreeCreated)
   {
      LoadTree();
   }
   else
   {
      cout << "Elements added to Tree: " << Tree->GetNumberOfObjects() << "\n\n";
   }

   // Perform the queries
   vector<string> files = getFilesInDirectory(queryPath);
   
   for (const auto &filePath : files)
   {
      LoadQueries(filePath);
      if (queryObjects.size() > 0)
         PerformQueries();
   }
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

vector<string> TApp::getFilesInDirectory(const string &directoryPath)
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

// Build id from sampleId and id
uint64_t TApp::buildId(uint64_t sampleId, uint64_t id)
{
   // Check if id doest occupy more than 32 bits
   if (id > 0xFFFFFFFF)
   {
      throw "Too many features. Id must be less than 0xFFFFFFFF to concatenate with sampleId.";
   }
   else
      return sampleId << 32 | id;
}

uint64_t TApp::getSampleId(uint64_t id)
{
   return id >> 32;
}

uint64_t TApp::getFeatureId(uint64_t id)
{
   return id & 0xFFFFFFFF;
}

double mean(const vector<KthElemenResult> &v)
{
   // compute mean of vector of KthElemenResult
   double sum = accumulate(v.begin(), v.end(), 0.0, [](double acc, const KthElemenResult &elem) {
      return acc + elem.second;
   });
   return sum / v.size();
}

double score(const vector<KthElemenResult> &v)
{
   // compute score associated with vector of KthElemenResult
   return pow(v.size(), 0.75) / (mean(v) + 0.15);
}

//------------------------------------------------------------------------------
void TApp::LoadTree()
{
   vector<float> data;
   vector<unsigned long> shape;
   uint64_t id;
   uint64_t sampleId;
   stArray *a;
   clock_t start, end;

   // Get all file paths inside the galleryPath directory
   vector<string> files = getFilesInDirectory(galleryPath);
   
   cout << "Loading " << files.size() << " files from " << galleryPath << endl;

   if (Tree != NULL)
   {
      // Go through all files in the directory
      start = clock();
      sampleId = 0;
      for (const auto &filePath : files)
      {
         // clear before using.
         data.clear();
         shape.clear();

         // Load features matrix from file
         npy::npy_data<float> d = npy::read_npy<float>(filePath);
         data = d.data;
         shape = d.shape;

         
         // Go through the lines of matrix
         for (uint64_t i = 0; i < shape[0]; i++)
         {
            // Get ith line from data matrix
            vector<float> feature(data.begin() + i * shape[1], data.begin() + (i + 1) * shape[1]);

            // Insert feature vector in the tree
            id = buildId(sampleId, i);

            // cout << id << ":: " <<  getSampleId(id) << " - " << getFeatureId(id) << endl;
            a = new stArray(id, feature);
            Tree->Add(a);
            delete a;
         }

         sampleId++;


        if (sampleId % (files.size() / 100) == 0) {
            int progress = (sampleId * 100) / files.size();
            cout << "\r" << progress << "% completed" << flush;
        }
      }
      end = clock();
      cout << "\nTotal Time: " << ((double)end - (double)start) / CLOCKS_PER_SEC << "s. ";
      cout << "Added " << Tree->GetNumberOfObjects() << " objects to tree\n\n";
   }
   else
   {
      cout << "Zero object added!!\n";
   }
}

void TApp::LoadQueries(string queryFile)
{
   vector<float> data;
   vector<unsigned long> shape;
   uint64_t id;
   clock_t start, end;

   // clear before using.
   queryObjects.clear();

   // Get all file paths inside the queryPath directory

   npy::npy_data<float> d = npy::read_npy<float>(queryFile);

   data = d.data;
   shape = d.shape;

   // Go through the lines of matrix
   for (uint64_t i = 0; i < shape[0]; i++)
   {
      // Get ith line from data matrix
      vector<float> feature(data.begin() + i * shape[1], data.begin() + (i + 1) * shape[1]);

      // Insert feature vector in the queryObjects vector
      this->queryObjects.insert(queryObjects.end(), new stArray(i, feature));
   }

   cout << "\nQuery: " << queryObjects.size() << " from " << queryFile.substr(queryFile.find_last_of("/\\") + 1);
}

//------------------------------------------------------------------------------
void TApp::PerformQueries()
{
   ResultDict map;
   if (Tree)
   {
      // cout << "\nStarting Statistics for Range Query with SlimTree.... ";
      // PerformRangeQuery();
      // cout << " Ok\n";

      map = PerformNearestQuery();
            
      // Sort map by score and outputs list of 8 highest scores
      vector<pair<uint64_t, double>> sortedScores;
      for (auto const &mapPair : map)
      {
         sortedScores.push_back(make_pair(mapPair.first, score(mapPair.second)));
      }
      sort(sortedScores.begin(), sortedScores.end(), [](const pair<uint64_t, double> &a, const pair<uint64_t, double> &b) {
         return a.second > b.second;
      });

      // For the 3 first cores, also print the name of the file associated
      vector<string> files = getFilesInDirectory(galleryPath);
      string fileName;
      for (int i = 0; i < min(8, (int)sortedScores.size()); i++)
      {
         cout << sortedScores[i].first << ": " << fixed << setprecision(4) << sortedScores[i].second;
         if (i < 3)
         {
            fileName = files[sortedScores[i].first];
            cout << " (" << fileName.substr(fileName.find_last_of("/\\") + 1) << ")";
         }
         cout << endl;
      }
      
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
ResultDict TApp::PerformNearestQuery()
{

   Result *result;
   clock_t start, end;
   uint64_t id;
   double dist;
   ResultDict map; // SampleId: vector<(FeatureId, Distance)>

   unsigned long size = queryObjects.size();

   if (Tree)
   {
      PageManager->ResetStatistics();
      Tree->GetMetricEvaluator()->ResetStatistics();
      start = clock();
      for (unsigned int i = 0; i < size; i++)
      {

         result = Tree->NearestQuery(queryObjects[i], K);

         for (unsigned int j = 0; j < result->GetNumOfEntries(); j++)
         {
            id = result->GetPair(j)->GetObject()->getOID();
            dist = result->GetPair(j)->GetKey();
            map[getSampleId(id)].push_back(KthElemenResult (id, dist));
         }

         delete result;
      } // end for
      
      // Outputs generated map
      //cout << map;

      end = clock();

      // Stats
      cout << ", Total Time: " << ((double)end - (double)start) / (CLOCKS_PER_SEC / 1000) << "(ms)\n";
      // cout << "\nTotal Time: " << ((double)end - (double)start) / (CLOCKS_PER_SEC / 1000) << "(ms)";
      // cout << "\nAvg Disk Accesses: " << (double)PageManager->GetReadCount() / (double)size;
      // cout << "\nAvg Distance Calculations: " << (double)Tree->GetMetricEvaluator()->GetDistanceCount() / (double)size;
      // cout << "\n";
   }

   return map;
}


