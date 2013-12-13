// This may look like C code, but it is really -*- C++ -*-
//
//
// Author:  Author Eduard Siemens <eduard.siemens@thomson.net>
//
// File:    my_cerrno.cpp
//          Thu, Jan 11 2007
//
// $Header:   //hanvsfile02/prog/Projects/2005/hipernet/Repository/hipernet/archives/HiperNet/Linux/Apps/ltest/my_cerrno.cpp-arc   1.0   23 Feb 2007 14:06:48   siemense  $
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
// $Log:   //hanvsfile02/prog/Projects/2005/hipernet/Repository/hipernet/archives/HiperNet/Linux/Apps/ltest/my_cerrno.cpp-arc  $
//
//   Rev 1.0   23 Feb 2007 14:06:48   siemense
//Initial revision.
//
//


#include "my_cerrno.h"

/**
   This file store the resolution of glibc errno to the respective errno name and errno description
 * currently, only the network related codes are stored
*/
errno_res::errno_res() :
        m_name ( "" ),
        m_desc ( "" )
{}

/**
 * assignment operator for the errno resolution structure
 */
void
errno_res::assign ( const std::string & s1, const std::string & s2 )
{
    m_name = s1;
    m_desc = s2;
}


/**
* the Cmy_cerrno class Stores the errno names and comments
*/
Cmy_cerrno::Cmy_cerrno()
{
    // from linux/errno.h
    // from bits/errno.h
    // from asm-generic/errno-base.h


#define EPIPE  32 /* Broken pipe */
#define EDOM  33 /* Math argument out of domain of func */
#define ERANGE  34 /* Math result not representable */

    ( *this ) [EPERM].assign ( "EPERM", "Operation not permitted" );            /* 1    */
    ( *this ) [ENOENT].assign ( "ENOENT", "No such file or directory" );        /* 2    */
    ( *this ) [ESRCH].assign ( "ESRCH", "No such process" );                    /* 3    */
    ( *this ) [EINTR].assign ( "EINTR", "Interrupted system call" );            /* 4    */
    ( *this ) [EIO].assign ( "EIO", "I/O error" );                              /* 5    */
    ( *this ) [ENXIO].assign ( "ENXIO", "No such device or address" );          /* 6    */
    ( *this ) [E2BIG].assign ( "E2BIG", "Argument list too long" );             /* 7    */
    ( *this ) [ENOEXEC].assign ( "ENOEXEC", "Exec format error" );              /* 8    */
    ( *this ) [EBADF].assign ( "EBADF", "Not a valid descriptor" );             /* 9    */
    ( *this ) [ECHILD].assign ( "ECHILD", "No child processes" );               /* 10    */
    ( *this ) [EAGAIN].assign ( "EAGAIN", "Try again" );                        /* 11   */
    ( *this ) [ENOMEM].assign ( "ENOMEM", "Out of memory" );                    /* 12   */
    ( *this ) [EACCES].assign ( "EACCES", "Permission denied" );                /* 13   */
    ( *this ) [EFAULT].assign ( "EFAULT", "Bad address" );                      /* 14   */
    ( *this ) [ENOTBLK].assign ( "ENOTBLK", "Block device required" );          /* 15   */
    ( *this ) [EBUSY].assign ( "EBUSY", "Device or resource busy" );            /* 16   */
    ( *this ) [EEXIST].assign ( "EEXIST", "File exists" );                      /* 17   */
    ( *this ) [EXDEV].assign ( "EAGAIN", "Cross-device link" );                 /* 18   */
    ( *this ) [ENODEV].assign ( "ENODEV", "No such device" );                   /* 19   */
    ( *this ) [ENOTDIR].assign ( "ENOTDIR", "Not a directory" );                /* 20   */
    ( *this ) [EISDIR].assign ( "EISDIR", "Is a directory" );                   /* 21   */
    ( *this ) [EINVAL].assign ( "EINVAL", "Invalid argument" );                 /* 22   */
    ( *this ) [ENFILE].assign ( "ENFILE", "File table overflow" );              /* 23   */
    ( *this ) [EMFILE].assign ( "EMFILE", "Too many open files" );              /* 24   */
    ( *this ) [ENOTTY].assign ( "ENOTTY", "Not a typewriter" );                 /* 25   */
    ( *this ) [ETXTBSY].assign ( "ETXTBSY", "Text file busy" );                 /* 26   */
    ( *this ) [EFBIG].assign ( "EFBIG", "File too large" );                     /* 27   */
    ( *this ) [ENOSPC].assign ( "ENOSPC", "No space left on device" );          /* 28   */
    ( *this ) [ESPIPE].assign ( "ESPIPE", "Illegal seek" );                     /* 29   */
    ( *this ) [EROFS].assign ( "EROFS", "Read-only file system" );              /* 30   */
    ( *this ) [EMLINK].assign ( "EMLINK", "Too many links" );                   /* 31   */
    ( *this ) [EPIPE].assign ( "EPIPE", "Broken pipe" );                        /* 32   */
    ( *this ) [ENODATA].assign ( "ENODATA", "No data available" );              /* 61   */
    ( *this ) [ETIME].assign ( "ETIME", "Timer expired" );                      /* 62   */
    ( *this ) [ECOMM].assign ( "ECOMM", "Communication error on send" );        /* 70   */
    ( *this ) [EMSGSIZE].assign ( "EMSGSIZE", "Message to long" );              /* 90   */
    ( *this ) [ENETDOWN].assign ( "ENETDOWN", "Network is down" );              /* 100  */
    ( *this ) [ENETUNREACH].assign ( "ENETUNREACH", "Netowrk is unreachable" ); /* 101  */
    ( *this ) [ENETRESET].assign ( "ENETRESET", "Network dropped connection because of reset" );  /* 102  */
    ( *this ) [ECONNABORTED].assign ( "ECONNABORTED", "Software caoused connection abort" );      /* 103  */
    ( *this ) [ECONNRESET].assign ( "ECONNRESET", "Connection reset by peer" );                   /* 104  */
    ( *this ) [ENOBUFS].assign ( "ENOBUFS", "No buffer space available" );                        /* 105  */
    ( *this ) [EISCONN].assign ( "EISCONN", "Transport endpoint is already connected" );          /* 106  */
    ( *this ) [ENOTCONN].assign ( "ENOTCONN", "Transpoirt endpoint is not connected" );           /* 107  */
    ( *this ) [ESHUTDOWN].assign ( "ESHUTDOWN", "Cannot send after transport endpoint shutdown" ); /* 108  */
    ( *this ) [ETIMEDOUT].assign ( "ETIMEDOUT", "Connection timed out" );       /* 110  */
    ( *this ) [ECONNREFUSED].assign ( "ECONNREFUSED", "Connection refused" );   /* 111  */
    ( *this ) [EHOSTDOWN].assign ( "EHOSTDOWN", "Host ist down" );              /* 112  */
    ( *this ) [EHOSTUNREACH].assign ( "EHOSTUNREACH", "No route to host" );     /* 113  */
    ( *this ) [EALREADY].assign ( "EALREADY", "Operation already in progress" );                   /* 114  */
    ( *this ) [EINPROGRESS].assign ( "EINPROGRESS", "Operation now in progress" );                 /* 115  */
}



/**
 * index operator. Each time of an not existent element will be accessed, assure, this element
 * will be added, filled by a default constructor of errno_res
 */
errno_res & Cmy_cerrno::operator[] ( const size_t pIdx )
{
    if ( pIdx >= my_errno_v.size() )
    {
        my_errno_v.resize ( pIdx + 1 );
    }
    return my_errno_v[pIdx];
}
