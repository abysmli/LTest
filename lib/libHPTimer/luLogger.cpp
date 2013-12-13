// This may look like C code, but it is really -*- C++ -*-
//
//
// Author:  Author Eduard Siemens <eduard.siemens@thomson.net>
//
// File:    luLogger.cpp
//          Fri, Jul 27 2007
//
// $Header$
// $Author:   siemense  $
// $Date:   23 Feb 2007 14:06:48  $
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
//   Rev 1.0   23 Feb 2007 14:06:48   siemense
//Initial revision.
//
//

#include "luLogger.h"
#include <iostream>

using namespace std;

// singleton acessor implementation
luLogstream *
luLogstream::Instance()
{
    static luLogstream logger( std::cerr.rdbuf(), NULL );
    return &logger;
}

// connect to the specified ostream
bool
luLogstream::SetErrorStream ( std::ostream * ErrStr_ptr )
{
    if ( NULL == ErrStr_ptr )
    {
        flush();
        mLoggingStreamBuf.SetErrorBuffer();
        return true;
    }
    ErrStr_ptr->flush();
    // connect my buffer to the Str output
    mLoggingStreamBuf.SetErrorBuffer ( ErrStr_ptr->rdbuf() );
    return true;
}

// connect to the specified ostream
bool
luLogstream::SetLogStream ( std::ostream * LogStr_ptr )
{
    if ( NULL == LogStr_ptr )
    {
        flush();
        mLoggingStreamBuf.SetFileLogBuffer();
        return true;
    }
    LogStr_ptr->flush();
    // connect my buffer to the Str output
    mLoggingStreamBuf.SetFileLogBuffer ( LogStr_ptr->rdbuf() );
    return true;
}

///ctor: sets the default log level to LU_NOTICE
luLogstream::luLogstream ( std::streambuf * ErrBuf, std::streambuf * LogBuf ) :
        std::ostream ( &mLoggingStreamBuf ),
        mLoggingStreamBuf ( ErrBuf, LogBuf )
{
}

/// logs either to discard or to connected output
luLogstream &
luLogstream::Log ( LogLevel Sev, const char * Prefix )
{
    mLoggingStreamBuf.SetMsgSeverity ( Sev );
    *this << Prefix << ":\t";
    return *this;
}
