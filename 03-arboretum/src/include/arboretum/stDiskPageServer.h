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
/**
* @file
*
* This file defines the abstract class stPageServer.
*
* @version 1.0
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
*/
#ifndef __STDISKPAGESERVER_H
#define __STDISKPAGESERVER_H

#include  <arboretum/stPage.h>
#include  <arboretum/stPageServer.h>
#include  <arboretum/stClientPageManager.h>

/**
* @version 1.0
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @ingroup storage
* @todo Implementation of this class.
*/
class stDiskPageServer: public stPageServer{
   public:


      stClientPageManager * OpenClient(int clientID);
      void CloseClient(stClientPageManager * client);
      stClientPageManager * CreateClient();

      void Open();
      void Close();
      void Create();

   private:
      struct stPageHeader{
         u_int32_t next;
         int clientID;
      };

      struct stClientEntry{
         int ID;
         u_int32_t HeaderPageID;
      };//end stClientEntry

      struct stHeader{
         char Magic[4];
         u_int32_t InUse;
         u_int32_t PageCount;
         u_int32_t firstFree;
         int ClientCount;
         int LatPage
         u_int32_t Next;
      };

      struct stExtHeader{
         int ClientCount;
         u_int32_t Next;
      };

};//end stDiskPageServer

#endif //__STDISKPAGESERVER_H
