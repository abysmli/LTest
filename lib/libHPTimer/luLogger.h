// This may look like C code, but it is really -*- C++ -*-
//
//
// Author:  Author Eduard Siemens <eduard.siemens@thomson.net>
//
// File:    luLogger.h
//          Fri, Jul 27 2007
//
// $Header:   //hanvsfile02/prog/Projects/2005/hipernet/Repository/hipernet/archives/HiperNet/Linux/Apps/ltest/my_cerrno.h-arc   1.0   23 Feb 2007 14:06:48   siemense  $
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
//
// Initial revision.
//
//


#ifndef LU_LOGGER_H
#define LU_LOGGER_H

#include "SplitStreamBuf.h"

/**
 * This class provides a logging facility. Using macros,
 *
*/
class luLogstream: public std::ostream
{

public:
    /**
     * only these macros shuld be used for logging
     * and debugging purposes
     */
#define LOG (*(luLogstream::Instance()))

#define LOG_FATAL luLogstream::Instance()->Log(LogLevel::FATAL, "LogLevel::FATAL")
#define LOG_ALERT luLogstream::Instance()->Log(LogLevel::ALERT, "LogLevel::ALERT")
#define LOG_CRITICAL luLogstream::Instance()->Log(LogLevel::CRITICAL, "LogLevel::CRITICAL")
#define LOG_ERROR luLogstream::Instance()->Log(LogLevel::ERROR, "LogLevel::ERROR")
#define LOG_WARNING luLogstream::Instance()->Log(LogLevel::WARNING, "LogLevel::WARNING")
#define LOG_NOTICE luLogstream::Instance()->Log(LogLevel::NOTICE, "LogLevel::NOTICE")
#define LOG_INFO luLogstream::Instance()->Log(LogLevel::INFO, "LogLevel::INFO")
#define LOG_DEBUG luLogstream::Instance()->Log(LogLevel::DEBUG, "LogLevel::DEBUG")
#define LOG_TRACEIN luLogstream::Instance()->Log(LogLevel::TRACEIN, "LogLevel::TRACEIN")
#define LOG_TRACEOUT luLogstream::Instance()->Log(LogLevel::TRACEOUT, "LogLevel::TRACEOUT")

#define LOG_SET_SEVERITY(x) luLogstream::Instance()->setSeverity(x)
#define LOG_SET_ERRORSTREAM(x) luLogstream::Instance()->SetErrorStream(x)
#define LOG_SET_LOGSTREAM(x) luLogstream::Instance()->SetLogStream(x)



/// accessor to the singleton
    static luLogstream * Instance();
    /**
     * set the destination of error stream
     * @param Str is the pouinter to the given error stream,
           * if NULL, disable the respective stream
     * @returns true, if setting of the reference was successful
     */
    bool SetErrorStream ( std::ostream * ErrStr_ptr );
    /**
     * set the destination of the logging stream
     * @param Str is the pouinter to the given error stream,
     * if NULL, disable the respective stream
     * @returns true, if setting of the reference was successful
     */
    bool SetLogStream ( std::ostream * LogStr_ptr );

    /// set the log level of the global logger to the given severity (LU_TRACE to LU_EMERGENCY)
    /// @param Severity is the given severity
    inline void SetSeverity ( LogLevel Severity )
    {
        mLoggingStreamBuf.SetSeverity ( Severity );
    }
    /// get the severity, actually in place
    /// @returns the severity in place
    inline LogLevel GetSeverity()
    {
        return mLoggingStreamBuf.GetSeverity();
    }
/// dtor:
    virtual ~luLogstream() {}
    /**
     * logs, depending on the configured severity
     */
    luLogstream & Log ( LogLevel Sev, const char * Prefix = "LU_LOG" );
protected:
    ///ctor: set output stream to standard out
    /// @param ErrBuff points to an error stream's buffer (usually cerr)
    /// @param LogBuf points to an log file associated stream buffer
    luLogstream ( std::streambuf * ErrBuf, std::streambuf * LogBuf );
    /// The streambuffer of the stream
    CSplitStreamBuffer mLoggingStreamBuf;
};


#endif // ----- LU_LOGGER -------
