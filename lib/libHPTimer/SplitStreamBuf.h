// This may look like C code, but it is really -*- C++ -*-
//
//
// Author:  Author Eduard Siemens <eduard.siemens@thomson.net>
//
// File:    SplitStreamBuf.h
//          Thue, Jun 12 2008
//
// $Header$
// $Author:   siemense  $
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
//
//


#ifndef SPLIT_STREAM_BUF_H
#define SPLIT_STREAM_BUF_H

#include <streambuf>
#include <ostream>
#include <limits>

// the logging severities
enum class LogLevel {
    NOLOG, // disable all logging
    FATAL,
    ALERT,
    CRITICAL,
    ERROR,
    WARNING,
    NOTICE,
    INFO,
    DEBUG,
    TRACEIN,
    TRACEOUT,
    LAST_
};


/** This is a special stream buffer, which split data
 * to the error log and file log buffers
 */
class CSplitStreamBuffer: public std::streambuf
{
public:

private:
    /// Error log buffer
    std::streambuf * mErrLogBufPtr;
    /// File log buffer
    std::streambuf * mFileLogBufPtr;
    //TODO: syslog buffer pointer

    // prevent copying and assignments
    CSplitStreamBuffer ( CSplitStreamBuffer const & );
    void operator= ( CSplitStreamBuffer const & );
    /// the log severity of this streambuf.
    /// messages with severities higher than this one will be
    /// printed out
    LogLevel mBufferSeverity;
    /// The severity of the actual message
    LogLevel mMessageSeverity;

public:
    /** ctor: set both buffers to passed values
                   * @param eStreamPtr: ptr to the error stream buffer
                   * @param fStreamPtr: ptr to the log file buffer
                   * if one of the parameter is NULL, the respective buffer
                   * will be set to a discard buffer
                   */
    CSplitStreamBuffer ( std::streambuf *eStreamPtr = NULL,
                         std::streambuf *fStreamPtr = NULL );
    /// ctor: set both buffers to discard buffer
    CSplitStreamBuffer ();
    /// set error buffer explicitely. @returns pointer to the old one error log buffer
    /// @param BufPtr is the stream buffer, to be used as error log target.
    /// If ommitted, error logging will be disabled
    std::streambuf * SetErrorBuffer ( std::streambuf * BufPtr = NULL );
    /// set file log buffer explicitely. @returns pointer to the old one file buffer
    /// @param BufPtr is the stream buffer, to be used as error log target.
    /// If ommitted, file logging will be disabled
    std::streambuf * SetFileLogBuffer ( std::streambuf * BufPtr = NULL );

    /// set the buffer severity
    inline void SetSeverity ( LogLevel & level )
    {
        mBufferSeverity = level;
    }

    /// get the buffer severity
    inline LogLevel GetSeverity()
    {
        return mBufferSeverity;
    }

    inline void SetMsgSeverity ( LogLevel & level )
    {
        mMessageSeverity = level;
    }

    // the created destructor is inappropriate: do nothing...
    /// stream overwlow - perform overflow on both buffers
    virtual int_type overflow ( int_type c );
    /// put characters into both buffers
    virtual std::streamsize xsputn ( char const *str, std::streamsize size );
    /// synchronize buffers to an input buffer
    virtual int sync();
    /// move stream buffer pointer  position. @returns the position within the second buffer on success or
    /// -1 if one of the settings gone wrong
    std::streampos seekoff ( std::streamoff off, std::ios_base::seekdir way,
                             std::ios_base::openmode which = std::ios_base::in | std::ios_base::out );
    /// set stream buffer pointer to given position. @returns the position within the first buffer on success or
    /// -1 if one of the settings gone wrong
    std::streampos seekpos ( std::streampos sp,
                             std::ios_base::openmode which = std::ios_base::in | std::ios_base::out );
};


/** a stream (essentially for testing purposes, which receives data
 * and split them simultaneously to two streams
 */
class CTeeStream: public std::ostream
{
private:
    std::streambuf * mStreamBuf; // used to save the 'streambuf' created

    CTeeStream ( CTeeStream const & );
    void operator= ( CTeeStream const & );

public:
    CTeeStream ( std::streambuf *s1, std::streambuf *s2 );
    virtual ~CTeeStream();

};




#endif // ----- SPLIT_STREAM_BUF_H-------
