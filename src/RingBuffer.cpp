// This may look like C code, but it is really -*- C++ -*-
//
//
// Author:  Author Eduard Siemens <siemens@rvs.uni-hannover.de>
//
// File:    RingBuffer.cpp
//          Sat Jun 27 2004
//
// $Id: RingBuffer.cpp,v 2.2 2005/07/19 15:46:19 siemens Exp $
// $Author:   siemense  $
// $Date:   31 Jan 2007 09:11:32  $
// Copyright (C) 2004, Eduard Siemens All Rights Reserved.
//
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:

//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.  
//     * Neither the name of the Institute for Computer Networks
//       nor the names of its contributors may be used to endorse or
//       promote products derived from this software without specific
//       prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//
//
//
//unnecessary files removed
//
//
//data rate limit extended
//
//
//Initial revision.
//
//
//Initial revision.
// Revision 2.2  2005/07/19 15:46:19  siemens
// Final release from Eduard Siemens at the RVS
//
// Revision 2.1  2004/06/28 19:20:08  siemens
// corbaRemoteAddress bug fixed, RingBuffer class for TCP reassembling added
//
//
//

#include <unistd.h>
#include <iostream>
#include <string.h>
#include "RingBuffer.h"  

namespace ltest {

using namespace std;

/// default ctor, malloc buffer, initialize pointers
CRingBuffer::CRingBuffer(unsigned int pCapacity, unsigned int pMaxSegment):
  mBuffer(0),
  mCapacity(pCapacity),
  mMaxSegment(pMaxSegment),
  mStoredChars(0)
{
  mBuffer = new char[2*pMaxSegment + pCapacity];
  mRead_ptr = mWrite_ptr = mBuffer;
  mTotalBufferLength = 2 * mMaxSegment + mCapacity;
  mEndBuff_ptr = mEndData_ptr = mBuffer + mTotalBufferLength;
}

/// dtor: free the buffer
CRingBuffer::~CRingBuffer(){

  if (mBuffer){
    delete mBuffer;
    mBuffer = 0;
  }
}

/// store the char * string
int 
CRingBuffer::Store(char * pBuf, unsigned int pLen)
{
  // buffer overrun
  if (mStoredChars >= mCapacity) return -1;
  
  // string is longer as the tail in the buffer, store the string
  // at the beginning of the buffer
  if ( mWrite_ptr + pLen > mEndBuff_ptr)
    {
      // mark the end of data
      mEndData_ptr = mWrite_ptr;
      memcpy(mBuffer, pBuf, pLen);
      mWrite_ptr = mBuffer + pLen;
    }
  // the string fits into the tail 
  else 
    {
      memcpy(mWrite_ptr, pBuf, pLen);
      mWrite_ptr += pLen;
    }
  mStoredChars += pLen;
  return pLen;
}


/// store the char * strin and go ahead in the Buffer
int 
CRingBuffer::Fetch(char * pBuf, unsigned int pLen, 
		   bool pCopy, bool pAtomic)
{
  // real read len
  unsigned int RealLen(pLen);

  if (pAtomic && pLen > mStoredChars) return 0; 

  // only mStoredChars are to read
  if ( pLen > mStoredChars  && mRead_ptr != mWrite_ptr)
    {
      return this->Fetch(pBuf, mStoredChars, pCopy, pAtomic);
    }

  // buffer is empty
  if (mRead_ptr == mWrite_ptr) return 0;
  
  // can't read more than Capacity
  if ( pLen > mCapacity ) return -1;

  // read over the data border is only possible in thwo steps
  if (mEndData_ptr - mRead_ptr <= pLen)
    {
      unsigned int RestLen(mEndData_ptr - mRead_ptr);
      // step one, is skiped if read within the data border
      RealLen -= RestLen;
       pCopy && memcpy(pBuf, mRead_ptr, RestLen);
      // switch over the read pointer and move the copy pointer
      mRead_ptr = mBuffer;
      mEndData_ptr = mEndBuff_ptr;
      pBuf += RestLen;
    }
  // step two
  pCopy && memcpy(pBuf, mRead_ptr, RealLen);
  mRead_ptr += RealLen;
  mStoredChars -= pLen;
  return pLen;
}

int 
CRingBuffer::FetchHead(char * pBuf, unsigned int pHeadLen, unsigned int pLen)
{
  // first, read the header
  unsigned int res(this->Fetch(pBuf, pHeadLen, true, true));
  // second, free the rest of pLen bytes
  if ( res == pHeadLen )
    {
      res += this->Fetch(0, pLen - pHeadLen, false, true);
    }
  return res;		   
}  



/**
   copy pLen, but maximum mStoredChars to the buffer pBuf, 
   go not ahead with the read pointer
   @param pBuf represents the bufer to store result
   @param pLen the maximum lengh of read data
*/
int 
CRingBuffer::Read(char * pBuf, unsigned int pLen, bool pAtomic)
{
  // real read len
  unsigned int RealLen(pLen);

  if (pAtomic && pLen > mStoredChars) return 0; 

  if ( pLen > mStoredChars  && mRead_ptr != mWrite_ptr)
    {
      return this->Read(pBuf, mStoredChars, pAtomic);
    }

  // buffer is empty
  if (mRead_ptr == mWrite_ptr) return 0;
  
  // can't read more than Capacity
  if ( pLen > mCapacity ) return -1;

  // read over the data border is only possible in thwo steps
  
  if (mEndData_ptr - mRead_ptr <= pLen)
    {
      unsigned int RestLen(mEndData_ptr - mRead_ptr);
      // step one, is skiped if read within the data border
      RealLen -= RestLen;
      memcpy(pBuf, mRead_ptr, RestLen);
      pBuf += RestLen;
      // step two, read
      memcpy(pBuf, mBuffer, RealLen);
      return pLen;
    }
  else
    {
      memcpy(pBuf, mRead_ptr, pLen);
    }
  return pLen;
}

/// returns stored chars
unsigned int
CRingBuffer::StoredChars()
{
  return mStoredChars;
}

} // end of namespace ltest


     
