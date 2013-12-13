// This may look like C code, but it is really -*- C++ -*-
//
//
// Author:  Author Eduard Siemens <eduard.siemens@thomson.net>
//
// File:    SplitStreamBuf.cpp
//          Thu, Jun 12 2008
//
// $Header$
// $Author:   siemense  $
// $Date$
// Copyright (C) 2007, Thomson Corporation, All Rights Reserved.
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
// $Log$

#include "SplitStreamBuf.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////
// Split stream buffer
//////////////////////////////////////////////////////////////////////////

/// ctor: set both buffers to discard buffer
CSplitStreamBuffer::CSplitStreamBuffer ( streambuf *eStreamPtr, streambuf
        *fStreamPtr ) :
        streambuf(),
        mErrLogBufPtr ( eStreamPtr ),
        mFileLogBufPtr ( fStreamPtr ),
        mBufferSeverity ( LogLevel::NOTICE ),
        mMessageSeverity ( LogLevel::LAST_ )
{}

/// set error buffer explicitely. @returns pointer to the old one error log
streambuf *
CSplitStreamBuffer::SetErrorBuffer ( streambuf * BufPtr )
{
    streambuf * tmpBuf ( mErrLogBufPtr );
    mErrLogBufPtr = BufPtr;
    return tmpBuf;
}

/// set file log buffer explicitely. @returns pointer to the old one file buffer
streambuf *
CSplitStreamBuffer::SetFileLogBuffer ( streambuf * BufPtr )
{
    streambuf * tmpBuf ( mFileLogBufPtr );
    mFileLogBufPtr = BufPtr;
    return tmpBuf;
}

/// implement the overflow method, run sync on both streams
streambuf::int_type
CSplitStreamBuffer::overflow ( streambuf::int_type c )
{
    streambuf::int_type retEBuf ( traits_type::not_eof ( 'a' ) ), retFBuf ( traits_type::not_eof ( 'a' ) );
    // skip if we have not to log the actual severity
    if ( mMessageSeverity > mBufferSeverity )
    {
        return traits_type::not_eof ( c );
    }

    // since NULL pointers are used to disable the particular
    // outputs independently, we have to perfrom each stream individually
    if ( NULL != mErrLogBufPtr )
    {
        retEBuf = mErrLogBufPtr->sputc ( c );
    }

    if ( NULL != mFileLogBufPtr )
    {
        retFBuf = mFileLogBufPtr->sputc ( c );
    }
    if ( retEBuf == traits_type::eof() || retFBuf == traits_type::eof() )
    {
        return traits_type::eof();
    }
// on success, we have to return an arbitrary integer but EOF
    return traits_type::not_eof ( c );
}

/// this is the only routine which puts the data into the appropriate buffers
/// @return the less one number of bytes written to the buffers
streamsize CSplitStreamBuffer::xsputn ( char const *str, streamsize size )
{
    streamsize retErrLog ( std::numeric_limits<std::streamsize>::max() ),
    retFileLog ( std::numeric_limits<std::streamsize>::max() ), retVal;

    if ( mMessageSeverity > mBufferSeverity )
    {
        return size;
    }

    if ( NULL != mErrLogBufPtr )
    {
        retErrLog = mErrLogBufPtr->sputn ( str, size );
    }
    if ( NULL != mFileLogBufPtr )
    {
        retFileLog = mFileLogBufPtr->sputn ( str, size );
    }
    retVal = std::min ( retErrLog, retFileLog );
    // if we have not written into either buffer, we have to return size
    return retVal == std::numeric_limits<std::streamsize>::max() ? size : retVal;
}

/// synchronize streams
int CSplitStreamBuffer::sync()
{
    int retErrLog ( 0 ), retFileLog ( 0 );

    if ( mErrLogBufPtr )
    {
        retErrLog = mErrLogBufPtr->pubsync();
    }
    if ( mFileLogBufPtr )
    {
        retFileLog = mFileLogBufPtr->pubsync();
    }

    if ( -1 == retErrLog || -1 == retFileLog )
    {
        return -1;
    }
    // on success, we have to return zero
    return 0;
}

/// set stream position pointer, since we have two buffers,
/// we set it to file buffers
streampos
CSplitStreamBuffer::seekoff ( streamoff off, ios_base::seekdir way,
                              ios_base::openmode which )
{
    streampos retEBuf ( 0 ), retFBuf ( 0 );
    if ( NULL != mErrLogBufPtr )
    {
        retEBuf = mErrLogBufPtr->pubseekoff ( off, way, which );
    }
    if ( NULL != mFileLogBufPtr )
    {
        retFBuf = mFileLogBufPtr->pubseekoff ( off, way, which );
    }
// test, if one of the seekoffs failed
    if ( ( retEBuf == pos_type(off_type(-1)) ) ||
            ( retFBuf == pos_type(off_type(-1)) ))
    {
        return pos_type(off_type(-1));
    }
    return retFBuf;
}

/// set stream pointer to given position
streampos
CSplitStreamBuffer::seekpos ( streampos sp, ios_base::openmode which )
{
    streampos retEBuf ( 0 ), retFBuf ( 0 );
    if ( NULL != mErrLogBufPtr )
    {
        retEBuf = mErrLogBufPtr->pubseekpos ( sp, which );
    }
    if ( NULL != mFileLogBufPtr )
    {
        retFBuf = mFileLogBufPtr->pubseekpos ( sp, which );
    }
// test, if one of the seekoffs failed
    if ( ( retEBuf == pos_type(off_type(-1))) || ( retFBuf == pos_type(off_type(-1))) )
    {
        return pos_type(off_type(-1));
    }
    return retFBuf;
}

//////////////////////////////////////////////////////////////////////////
// The Tee stream
//////////////////////////////////////////////////////////////////////////

/// ctor: create a new stream with the split stream buffer
CTeeStream::CTeeStream ( streambuf *s1, streambuf *s2 ) :
        ostream ( new CSplitStreamBuffer ( s1,s2 ) ),
        mStreamBuf ( NULL )
{
    mStreamBuf = rdbuf();
}

///dtor
CTeeStream::~CTeeStream()
{
// delete the split stream buffer
    if ( NULL != mStreamBuf )
    {
        delete mStreamBuf;
        mStreamBuf = NULL;
    }
}

