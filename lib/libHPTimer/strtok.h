// This may look like C code, but it is really -*- C++ -*-
//
// $Id: strtok.h,v 2.2 2005/07/19 15:46:19 siemens Exp $
//
// Author:  Eduard Siemens <siemens@rvs.uni-hannover.de>
//
// File:    strtok.h
//
// Author:  Author Eduard Siemens <siemens@rvs.uni-hannover.de>
//
// File:   Basics.cpp
//         Wed Aug 21 2002
//
// $Id: Basics.cpp,v 2.2 2004/05/25 14:08:29 siemens Exp $
// $Author:   siemense  $
// $Date:   31 Jan 2007 09:10:18  $
// Copyright (C) 2004, Eduard Siemens All Rights Reserved. siemens@rvs.uni-hannover.de
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
//    * All advertising materials mentioning features or use of this software
//      must display the following acknowledgement:
//      This product includes software developed by Eduard Siemens
//      University of Hannover.

//
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
// $Log:   //hanvsfile02/prog/Projects/2005/hipernet/Repository/hipernet/archives/hipernet/Linux/Apps/ltest/strtok.h-arc  $
//
//   Rev 1.6.1.4   31 Jan 2007 09:11:34   siemense
//Efficiency issues solved. Interfaces to network classes has been cleaned up.
//
//   Rev 1.6.1.3   13 Nov 2006 11:58:18   siemense
//Minor bug fixes.
//Version before CIPNetworkPerformance tunings.
//
//   Rev 1.6.1.1   17 Aug 2006 13:21:14   siemense
//TSC timer class has been implemented and rudimentary tested
//
//   Rev 1.6.1.0   Jun 27 2006 10:40:22   siemense
//New Protocols Branch
//
//   Rev 1.6   21 Jun 2006 13:51:46   siemense
//Improved Cristian Algorithm implemented properly
//
//   Rev 1.5   21 Dec 2005 16:42:32   siemense
//Time correction between data sender and data recei ver added
//
//   Rev 1.4   15 Nov 2005 15:10:42   siemense
//zero duration and packet count bug fixed
// *
// *    Rev 1.3   14 Nov 2005 16:32:14   siemense
// * Data range of rate extended to long long. Minor bug fixes.
// *
// *    Rev 1.2   07 Oct 2005 09:11:30   siemense
// * unnecessary files removed
// *
// *    Rev 1.1   06 Oct 2005 17:07:00   siemense
// * data rate limit extended
//
//    Rev 1.0   28 Sep 2005 16:40:40   siemense
//  Initial revision.
// Revision 2.2  2005/07/19 15:46:19  siemens
// Final release from Eduard Siemens at the RVS
//
// Revision 2.1  2005/02/07 14:43:14  siemens
// bereinigung von fremden Code
//
//
//


#ifndef _STRTOK_H
#define _STRTOK_H

#include <string.h>
#include <string>
#include <stdlib.h>
// #ifdef NEEDS_USING_STD
// #endif
#include <vector>




/**
   class for throwing errors in conjunctiorn with string failures
   author Igor Olkhovsky
*/
class str_exception: public std::exception {
protected:
    /// exception message
    std::string emsg;
    /// error code value
    int eval;
public:
    ///ctor
    str_exception():
            emsg(""),
            eval(0)
    {
    }
    /// ctor with predefined error message
    str_exception(const std::string & _emsg):
            emsg(_emsg),
            eval(0)
    {
    }
    /// ctor with predefined error message and error value
    str_exception(const std::string & _emsg, int _eval):
            emsg(_emsg),
            eval(_eval)
    {
    }
    virtual const char * what() const throw ()
    {
        return emsg.c_str();
    }
    virtual int code() const
    {
        return eval;
    }
    /// dtor
    virtual ~str_exception() throw ()
    {
    }
};

/**
   class with static functions for string manipulations
   @author E. Siemens
*/
class CStringUtils {
public:

    /** convert int i to a string
     * @exception std::bad_alloc if a memory allocation failed
     * @exception std::length_error if the maximum size of a string would be exceeded
     */
    static std::string itos(int i);

    /// convert unsigned int to a string
    static std::string uitos(unsigned int ui);

    /// convert long l to a string
    static std::string ltos(long l);

    /** convert long l to a string
     * @exception std::bad_alloc if a memory allocation failed
     * @exception std::length_error if the maximum size of a string would be exceeded
     */
    static std::string ultos(long ul);

    /// convert long long l to a string
    static std::string lltos(long long l);

    /// convert unsigned long long to a string
    static std::string ulltos(unsigned int ull);

    /// convert float f to a string
    static std::string ftos(double f);

    /** convert a string to an integer
     * @exception std::bad_alloc if a memory allocation failed
     * @exception std::length_error if the maximum size of a string would be exceeded
     */
    static int    stoi(std::string s);

    /// convert a string to a long
    static long   stol(std::string s);

    /// convert a string to a long long
    static long long  stoll(std::string s);

    /// convert a string to a double
    static double stof(std::string s);

    /// convert a string to a bool
    static bool stob(std::string s);

    /// convert a bool to a string
    static std::string btos(bool b);

    /// converts a string to a vector of chars.
    static std::vector<unsigned char> stov(std::string s);

    /// converts a vector of chars to a string.
    static std::string vtos(std::vector<unsigned char> vec);

    /// converts a char * to string
    static std::string chtos(const char * ch_ptr);

    /** comapre to strings case insensitive.
      * @return zero if identical.
      */
    static int stricmp(const std::string &a, const std::string &b);

    /** make a string of uppercase characters
     * @return uppercase string
     */
    static std::string toupper(std::string l);

    /** make a string of lowercase characters
     * @return lowercase string
     */
    static std::string tolower(std::string l);

    /**
       trim cuts all leading and trailing characters which are given
       in ch from a string.
       If no characters are specified all white spaces (space, tab, new line,
       carriage return) will be deleted.

       @param s the string which should be trimed
       @param ch the characters that should be eleminated

       @return the string without the cut characters
    */
    static std::string trim(const std::string& s, const std::string& ch = " \r\n\t");

    /**
    trimleft same as trim but only leading characters will be deleted
    */
    static std::string trimleft(const std::string& s, const std::string& ch = " \r\n\t");

    /**
       trimright same as trim but only trailing characters will be deleted
    */
    static std::string trimright(const std::string& s, const std::string& ch = " \r\n\t");

    /**
       returns true if all characters in the string are digits
    */
    static bool allDigits(const std::string& s);

    /**
        Decoding a time string (format: DD.MM.YYYY HH.MM.SS)
    */
    static time_t stotime(const std::string& text);

    /** Converts a time_t structure into a string with the
     *  format DD.MM.YYYY HH:MM:SS
     */
    static std::string timetos (const time_t& t);

    /** Replaces all occurences of  from in str by to and
        returns the resulting string
    */
    static std::string replaceInString (const std::string & str, const std::string & from,
                                        const std::string & to);

    //static CTimer stot(const string& timestr);


    /**
       insert delimiters pDelim into the pDigit
    */
    static std::string decades_formatted (const long long pDigit,
                                          const std::string pDelim = ",");

};


/**
   class, that works like java tokenizer
   ATTENTION!!! we don't implement a lazy parser,
   so this tokenizer is not suitable for handle with big amounts of data!
   the string is really cracked down in the constructor !!!
*/
class CStringTokenizer {
public:
    /**
     * ctor
     * @param returnTokens is a flag, if true, delimiters are returned as tokens
     * @param clusterDelimiters is a flag, if true, more than one delimiter characters
     * in a block are treated as one delimiter, else every delimiter character
     * delimits (maby by an empty) token
     * @exception std::bad_alloc if a memory allocation failed
     */
    CStringTokenizer(std::string str, std::string delim=" \t\n", bool returnTokens = false, bool clusterDelimiters=true);

    /// dtor, non virtual, because
    /// should be final.
    ~CStringTokenizer();

    /// @return the number of the not delivered tokens,
    /// if the current delimiter string is used.
    int           countTokens() const;

    /**
     * @return the number of the not delivered tokens,
     * if the given delimiter string is used. Set the delimiter
     * string to the new one
     * @exception std::bad_alloc if a memory allocation failed
     */
    int           countTokens(std::string delim);
    /// @return true, if there are more tokens to get
    bool          hasMoreTokens() const;

    /** return the next token
     * @exception std::bad_alloc if a memory allocation failed
     * @exception std::length_error if the maximum size of a string would be exceeded
     */
    std::string        nextToken();

    /**
     * @return the next token by using a new token delimiter. The delimiter is
     * used for subsequent calls to nextToken()
     * @exception std::bad_alloc if a memory allocation failed
    * @exception std::length_error if the maximum size of a string would be exceeded
     */
    std::string        nextToken(std::string delim);

    /**
     * @returns the remainder of the tokenized string
     * @param devourRemainer tells if the remaind string
     *        should be devoured
     * @exception std::bad_alloc if a memory allocation failed
     */
    std::string        remainder(bool devourRemainder = true);

private:
    /// count the length of the delimiter characters from the position pos
    int           delimiterSize(int pos) const;

    /// count the length of the non-delimiter characters from the position
    int           nonDelimiterSize(int pos) const;

    /// the string to be cracked
    std::string        mString;

    /// the delimiter string
    std::string        mDelimiter;

    /// this flag shows, if delimiter tockens are returned
    bool          mReturnTokens;

    /// the current position
    int           mPos;

    /// are tokens to be regarded as space?
    bool          mClusterDelimiters;

    /// Indicates if the last returned token was a delimiter
    bool          mLastReturnDelim;
};
#endif // _STRTOK_H


