// This may look like C code, but it is really -*- C++ -*-
//
//
// Author:  Author Eduard Siemens <eduard.siemens@thomson.net>
//
// File:    my_cerrno.h
//          Thu, Jan 11 2007
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
// $Log:   //hanvsfile02/prog/Projects/2005/hipernet/Repository/hipernet/archives/HiperNet/Linux/Apps/ltest/my_cerrno.h-arc  $
//
//   Rev 1.0   23 Feb 2007 14:06:48   siemense
//Initial revision.
//
//


#ifndef MY_CERRNO_H
#define MY_CERRNO_H

#include <errno.h>
#include <vector>
#include <string>


// from linux/errno.h
// from bits/errno.h
// from asm-generic/errno-base.h
//

/**
   This fEMSGSIZEile store the resolution of glibc errno to the respective errno name and errno description
 * currently, only the network related codes are stored
*/
struct errno_res
{
    std::string m_name; // the name of the macro
    std::string m_desc; // the description of the macro
    /* ctor */
    errno_res();
    // the assignment operator
    void assign(const std::string & s1, const std::string & s2);
};

/**
 * the Cmy_cerrno class Stores the errno names and comments
*/
class Cmy_cerrno
{
public:
    /* the vector of errno codes */
    std::vector <errno_res> my_errno_v;

    /**
     * ctor
     */
    Cmy_cerrno();
    /**
     * idx operator for access to elements of my_errno_v.
     * the access is save a zero element is returned, if out-of-bound access has been performed
     */
    errno_res & operator[](const size_t pIdx);

};


#endif // ----- MY_CERRNO_H -------
