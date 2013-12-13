// This may look like C code, but it is really -*- C++ -*-
//
//
// Author:  Author Eduard Siemens <siemens@rvs.uni-hannover.de>
//
// File:    DataSource.h
//          Sat Jun 27 2004
//
//
// $Id: RingBuffer.h,v 2.2 2005/07/19 15:46:19 siemens Exp $
// $Author:   siemense  $
// $Date:   31 Jan 2007 09:11:34  $
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
//zero duration and packet count bug fixed
// * 
//
// * Data range of rate extended to long long. Minor bug fixes.
// * 
//
// * unnecessary files removed
// * 
//
// * data rate limit extended
//  * 
//
//  * Initial revision.
// Revision 2.2  2005/07/19 15:46:19  siemens
// Final release from Eduard Siemens at the RVS
//
// Revision 2.1  2004/06/28 19:20:08  siemens
// corbaRemoteAddress bug fixed, RingBuffer class for TCP reassembling added
//
//
//


#ifndef RINGBUFFER_H
#define RINGBUFFER_H
#include <stdio.h>

namespace ltest {

/**
   This class implements a virtually endless ring buffer of characters, 
   that provides an endless stream (i.e. for TCP-Packet reassembling)
   Segments of maximum MaxSegment can be stored at once
   maximum capacity is MaxCapacity, the interface ist almost functional for 
   efficiency purposes - the write call returns -1 if capacity is overrun
   the read call works like read call on files of on sockets
*/
class CRingBuffer{
 public: 
  /// ctor, sets the capacity of the buffer and maximals stored segment in one 
  ///  call
  CRingBuffer(unsigned int pCapacity, unsigned int pMaxSegment);
  /// dtor
  virtual ~CRingBuffer(); 

  /** store the char string to the buffer, 
      returns stored bytes, or -1 � can't store the string (i.e. if more
      then capacity already stored
   */
  int Store(char * pBuf, unsigned int pLen);

  /** fetch a string of the length len, the fetched string 
      will be freed in the buffer
      returns bytes read, -1 if couldn't read
      @param pCopy tells, to copy the buffer (true), 
      or only move the read pointer and free the buffer, @param pAtomic
      tells, to return zero if, less then len bytes available, if false, 
      available part of buffer will be returned. Return value is the count 
      of bytes, stored
  */
  int Fetch(char * pBuf, unsigned int pLen, 
	    bool pCopy=true, bool pAtomic = true);

  /** fetch a head of the string of the length pHeadLen, the string 
      of length len will be freed (for shortening the memcopy call,
      if not whole string is needed for further processing
      returns bytes freed, -1 if couldn't read
  */
  int FetchHead(char * pBuf, unsigned int pHeadLen, unsigned int pLen);

  /**
     Read is like fetch, with the difference, that the data are copied
     to the pBuf, and the read pointer doesn't go ahead. So multiple read
     of the same data segment is possible, the pCopy parameter is not
     applicable because it is nonsense in that context
  */
  int Read(char * pBuf, unsigned int pLen, bool pAtomic = true);


  /** returns count of chars, stored in the buffer
   */
  unsigned int StoredChars();

 protected:
  char * mBuffer;
  char * mRead_ptr;
  char * mWrite_ptr;
  /// pointer behind the last byte in the buffer
  char * mEndBuff_ptr;
  /// pointer to the end of Data, can'be before the end of buffer, 
  /// because if the string doesn't fit behind the write buffer, 
  /// we'll write at the beginning, to avoide two write calls
  char * mEndData_ptr;
  /// capacity of the buffer
  unsigned int mCapacity;
  /// maximal writing an read segment length
  unsigned int mMaxSegment;
  /// counter for stored charakters in the buffer
  unsigned int mStoredChars;
  unsigned int mTotalBufferLength;
};

} // end of namespace ltest
#endif // ----- RINGBUFFER_H -------
