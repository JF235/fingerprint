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
* This file defines the classes stPage and stLockablePage.
*
* @version 1.0
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
*/
#ifndef __STPAGE_H
#define __STPAGE_H

#include <arboretum/stCommon.h>
#include <stdexcept>

//----------------------------------------------------------------------------
// Class stPage
//----------------------------------------------------------------------------
/**
* This class abstracs a physical disk page in memory. It will allow some functions related
* with memory manipulation and page identification.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @author Enzo Seraphim (seraphim@icmc.usp.br)
* @version 1.0
* @ingroup storage
*/
class stPage{

   public:

      /**
      * Creates a new page.
      *
      * @param size The page size in bytes.
      * @param pageid Page id.
      */
      stPage (u_int32_t size, u_int32_t pageid = 0);

      /**
      * Disposes this page and free all allocated resources.
      */
      virtual ~stPage();

      /**
      * Writes n bytes to this page and sets the cursor to the next writing
      * position.
      *
      * @param buff The byte array to be written.
      * @param offset Position where the buff will be written.
      * @param n Number of bytes to write.
      * @warning This method will not perform error checking unless __stDEBUG__
      * is defined at compile time.
      */
      virtual void Write(unsigned char * buff, u_int32_t n, u_int32_t offset);

      /**
      * Returns the page size in bytes.
      *
      * @return The page size in bytes.
      */
      virtual u_int32_t GetPageSize();

      /**
      * Returns the pointer to the data array. This method allows the direct access
      * to the internal page data array. It will be used by the stDiskPageManager to
      * read/write this page from/to the disk.
      *
      * <P>Avoid to use this method. If a read/write operation is required, use Read()
      * and Write() methods instead.
      *
      * @return The pointer to the data array of this page.
      */
      virtual unsigned char * GetData();

      /**
      * Returns the ID of this page.
      *
      * @return The ID of this page.
      * @see SetPageID()
      */
      u_int32_t GetPageID(){

         return PageID;
      }//end GetPageID

      /**
      * Sets the ID of this page.
      *
      * @see GetPageID().
      */
      void SetPageID(u_int32_t pageID){

         PageID = pageID;
      }//end SetPageID

      /**
      * This method will copy the content of a given page to this page.
      * Both pages must have the same size.
      *
      * @param page Source page.
      * @warning This method will not perform error checking unless __stDEBUG__
      * is defined at compile time.
      */
      void Copy(stPage * page);

      /**
      * This method will clear this page (set all bytes to 0).
      */
      void Clear();

   protected:

      /**
      * The page (buffer).
      */
      unsigned char * Buffer;

      /**
      * The page size.
      */
      u_int32_t BufferSize;

   private:

      /**
      * The ID of this page.
      */
      u_int32_t PageID;
      
};//end stPage

//----------------------------------------------------------------------------
// Class stLockablePage
//----------------------------------------------------------------------------
/**
* This class is an extension to stPage which can block access to part of the
* physical page. This segment is always located in the begining of the page.
*
* <P>The stLockablePage was designed to hide a portion of the page from the
* SlimTree. The locked segment may be used to store PageManager's reserved
* information such a page header.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @version 1.0
* @todo Tests.
* @ingroup storage
*/
class stLockablePage: public stPage{

   public:

      /**
      * Creates a new stLocakablePage.
      *
      * @param size The page size in bytes.
      * @param locksize The size of the locked segment.
      * @param pageid Page id.
      * @warning This method will not perform error checking unless __stDEBUG__
      * is defined at compile time.
      */
      stLockablePage (u_int32_t size, u_int32_t locksize,
            u_int32_t pageid = 0):stPage(size, pageid){

         this->Lock(locksize);
      }//end stLockablePage

      /**
      * Writes n bytes to this page and sets the cursor to the next writing
      * position. This method can not be used to write to the locked segment.
      *
      * @param buff The byte array to be written.
      * @param offset Position where the buff will be written.
      * @param n Number of bytes to write.
      * @warning This method will not perform error checking unless __stDEBUG__
      * is defined at compile time.
      */
      virtual void Write(unsigned char * buff, u_int32_t n, u_int32_t offset);

      /**
      * Returns the page size in bytes. This value does not include the size of
      * the locked segment.
      *
      * @return The page size in bytes.
      * @see GetTruePageSize()
      */
      virtual u_int32_t GetPageSize();

      /**
      * Returns the pointer to the data array. This method allows the direct
      * access to the internal page data array. The returned pointer do not
      * include the locked segment. See GetTrueData() to get a pointer to the
      * entire page.
      *
      * <P>Avoid to use this method. If a read/write operation is required, use Read()
      * and Write() methods instead.
      *
      * @return The pointer to the data array of this page.
      * @see GetTrueData()
      */
      virtual unsigned char * GetData();

      /**
      * Checks if the page is locked.
      *
      * @return Returns true if the page is locked or false otherwise.
      */
      bool IsLocked(){

         return (this->LockSize != 0);
      }//end IsLocked

      /**
      * Returns the size of the locked segment.
      */
      u_int32_t GetLockSize(){

         return this->LockSize;
      }//end GetLockSize

      /**
      * Sets the size of the locked segment.
      *
      * @param size The size of hte locked segment.
      * @warning This method will not perform error checking unless __stDEBUG__
      * is defined at compile time.
      * @see GetLockedSize()
      * @see IsLocked()
      * @see GetTrueData()
      */
      void Lock(u_int32_t size){

         #ifdef __stDEBUG__
         if (size >= this->BufferSize){
            throw std::logic_error("Unable to lock the entire page.");
         }//end if
         #endif //__stDEBUG__

         this->LockSize = size;
      }//end Lock

      /**
      * This method returns the true data array of this page. This array will
      * include the locked segment.
      *
      * @see GetData()
      */
      const unsigned char * GetTrueData(){

         return this->Buffer;
      }//end GetTrueData

      /**
      * Returns the true page size. This size is the size of the page plus the
      * size of the locked segment.
      */
      u_int32_t GetTruePageSize(){

         return this->BufferSize;
      }//end GetTRuePageSize

   private:

      /**
      * Size of the locked segment.
      */
      u_int32_t LockSize;
      
};//end stLockablePage

//----------------------------------------------------------------------------
// Class stPageAllocator
//----------------------------------------------------------------------------
/**
* This class is the allocator implementation that allows the use of the
* stInstanceCacheTemplate
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @author Enzo Seraphim (seraphim@icmc.usp.br)
* @version 1.0
* @ingroup storage
*/
class stPageAllocator{

   public:

      /**
      * Creates a new allocator for stPage instances. All pages will have
      * pageSize bytes.
      *
      * @param pageSize The size of the new pages created bye this allocator.
      */
      stPageAllocator(u_int32_t pageSize){
         this->pageSize = pageSize;
      }//end pageSize
      
      /**
      * Creates new stPage instances.
      */
      stPage * Create(){
         return new stPage(pageSize, 0);
      }//end Create
      
      /**
      * Disposes the given stPage instance.
      *
      * @param instance The instance to be disposed.
      */
      void Dispose(stPage * instance){
         delete instance;
		 instance = 0;
      }//end Dispose

   private:

      /**
      * Size of the pages to be created.
      */
      u_int32_t pageSize;
        
};//end stPageAllocator

#endif //__STPAGE_H
