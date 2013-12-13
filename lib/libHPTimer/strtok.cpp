// This may look like C code, but it is really -*- C++ -*-
//
// $Id: strtok.cpp,v 2.3 2005/07/19 15:46:19 siemens Exp $
//
//
// Author:  Eduard Siemens <siemens@rvs.uni-hannover.de>
//
//
// File:    strtok.cpp
//
//
// $Log:   //hanvsfile02/prog/Projects/2005/hipernet/Repository/hipernet/archives/hipernet/Linux/Apps/ltest/strtok.cpp-arc  $
//
//   Rev 1.5.1.4   31 Jan 2007 09:11:34   siemense
//Efficiency issues solved. Interfaces to network classes has been cleaned up.
//
//   Rev 1.5.1.3   13 Nov 2006 11:58:16   siemense
//Minor bug fixes.
//Version before CIPNetworkPerformance tunings.
//
//   Rev 1.5.1.1   17 Aug 2006 13:21:12   siemense
//TSC timer class has been implemented and rudimentary tested
//
//   Rev 1.5.1.0   Jun 27 2006 10:40:22   siemense
//New Protocols Branch
//
//   Rev 1.5   26 Jan 2006 10:35:58   siemense
//intermediate
//
//   Rev 1.4   26 Jan 2006 10:06:04   siemense
//Intermediate checkin
//
//   Rev 1.3   21 Dec 2005 16:42:32   siemense
//Time correction between data sender and data recei ver added
//
//   Rev 1.2   07 Oct 2005 09:11:30   siemense
//unnecessary files removed
//
//   Rev 1.1   06 Oct 2005 17:07:00   siemense
//data rate limit extended
//
//   Rev 1.0   28 Sep 2005 16:38:32   siemense
//Initial revision.
//
//   Rev 1.0   28 Sep 2005 16:34:00   siemense
//Initial revision.
// Revision 2.3  2005/07/19 15:46:19  siemens
// Final release from Eduard Siemens at the RVS
//
// Revision 2.2  2005/02/23 09:16:08  siemens
// allDigits bug fixed
//
// Revision 2.1  2005/02/07 14:43:14  siemens
// bereinigung von fremden Code
//
//
//
//
#include "strtok.h"
#include "luLogger.h"
#include <sstream>
#include <stdexcept>

/**
   string utility class, has conversion methods
*/
std::string
CStringUtils::itos ( int i )
{
    try
    {
        std::stringstream str_stream;
        str_stream << i;
        return str_stream.str();
    }
    catch ( const std::bad_alloc & exception )
    {
        LOG_ERROR << "CStringUtils::itos: Caught std::bad_alloc exception on "
        "converting integer to string! Exception description: " << exception.what()
        << std::endl;
        throw;
    }
    catch ( const std::length_error & exception )
    {
        LOG_ERROR << "CStringUtils::itos: Caught std::length_error exception on "
        "converting integer to string! Exception description: " << exception.what()
        << std::endl;
        throw;
    }
}

/// convert unsigned int to a string
std::string
CStringUtils::uitos ( unsigned int ui )
{
    std::stringstream str_stream;
    str_stream << ui;
    return str_stream.str();
}


/**
   convert a long to a string
 */
std::string
CStringUtils::ltos ( long l )
{
    std::stringstream str_stream;
    str_stream << l;
    return str_stream.str();
}

/**
   convert a long to a string
 */
std::string
CStringUtils::ultos ( long ul )
{
    try {
        std::stringstream str_stream;
        str_stream << ul;
        return str_stream.str();
    }
    catch ( const std::bad_alloc & exception )
    {
        LOG_ERROR << "CStringUtils::ultos: Caught std::bad_alloc exception on "
        "converting long to string! Exception description: " << exception.what()
        << std::endl;
        throw;
    }
    catch ( const std::length_error & exception )
    {
        LOG_ERROR << "CStringUtils::ultos: Caught std::length_error exception on "
        "converting long to string! Exception description: " << exception.what()
        << std::endl;
        throw;
    }
}

/**
   convert a double to a string
*/
std::string
CStringUtils::ftos ( double f )
{
    std::stringstream str_stream;
    str_stream << f;
    return str_stream.str();
}

/**
   convert a long long to a string
*/
std::string
CStringUtils::lltos ( long long ll )
{
    std::stringstream str_stream;
    str_stream << ll;
    return str_stream.str();
}

/// convert unsigned long long to a string
std::string CStringUtils::ulltos ( unsigned int ull )
{
    std::stringstream str_stream;
    str_stream << ull;
    return str_stream.str();
}

/**
   convert a string to an integer
*/
int
CStringUtils::stoi ( std::string s )
{
    try {
        int a ( 0 );
        std::string str ( "" );

        for ( unsigned int i ( 0 ); s.length() >i; i++ )
        {
            if ( a==0 )
            {
                int b=i+1;
                if ( s[i]=='-' )
                {
                    if ( ( s[b]=='0' ) || ( s[b]=='1' ) || ( s[b]=='2' ) || ( s[b]=='3' ) || ( s[b]=='4' ) || ( s[b]=='5' ) || ( s[b]=='6' ) || ( s[b]=='7' ) || ( s[b]=='8' ) || ( s[b]=='9' ) )
                    {
                        str=s.substr ( i );
                        a=1;
                    }
                }

                if ( ( s[i]=='0' ) || ( s[i]=='1' ) || ( s[i]=='2' ) || ( s[i]=='3' ) || ( s[i]=='4' ) || ( s[i]=='5' ) || ( s[i]=='6' ) || ( s[i]=='7' ) || ( s[i]=='8' ) || ( s[i]=='9' ) )
                {
                    str=s.substr ( i );
                    a=1;
                }

            }
        }

        return ( int ) strtol ( str.c_str(),0,10 );
    }
    catch ( const std::bad_alloc & exception )
    {
        LOG_ERROR << "CStringUtils::stoi: Caught std::bad_alloc exception on "
        "converting string to integer! Exception description: " << exception.what()
        << std::endl;
        throw;
    }
    catch ( const std::length_error & exception )
    {
        LOG_ERROR << "CStringUtils::stoi: Caught std::length_error exception on "
        "converting string to integer! Exception description: " << exception.what()
        << std::endl;
        throw;
    }
}


/**
   convert a string to a long
*/
long
CStringUtils::stol ( std::string s )
{
    int a ( 0 );
    std::string str ( "" );

    // check about illegal characters: only the pattern [+|-]\d+ is valid
    for ( unsigned int i ( 0 ); s.length() >i; i++ )
    {
        if ( a==0 )
        {
            int b=i+1;
            if ( s[i]=='-' )
            {
                if ( ( s[b]=='0' ) || ( s[b]=='1' ) || ( s[b]=='2' ) || ( s[b]=='3' ) || ( s[b]=='4' ) || ( s[b]=='5' ) || ( s[b]=='6' ) || ( s[b]=='7' ) || ( s[b]=='8' ) || ( s[b]=='9' ) )
                {
                    str=s.substr ( i );
                    a=1;
                }
            }

            if ( ( s[i]=='0' ) || ( s[i]=='1' ) || ( s[i]=='2' ) || ( s[i]=='3' ) || ( s[i]=='4' ) || ( s[i]=='5' ) || ( s[i]=='6' ) || ( s[i]=='7' ) || ( s[i]=='8' ) || ( s[i]=='9' ) )
            {
                str=s.substr ( i );
                a=1;
            }

        }
    }
    return ( long ) strtol ( str.c_str(),0,10 );
}

///convert a string to a long long
long long
CStringUtils::stoll ( std::string s )
{
    int a ( 0 );
    std::string str ( "" );

    // check about illegal characters: only the pattern [+|-]\d+ is valid
    for ( unsigned int i ( 0 ); s.length() >i; i++ )
    {
        if ( a==0 )
        {
            int b=i+1;
            if ( s[i]=='-' )
            {
                if ( ( s[b]=='0' ) || ( s[b]=='1' ) || ( s[b]=='2' ) || ( s[b]=='3' ) || ( s[b]=='4' ) || ( s[b]=='5' ) || ( s[b]=='6' ) || ( s[b]=='7' ) || ( s[b]=='8' ) || ( s[b]=='9' ) )
                {
                    str=s.substr ( i );
                    a=1;
                }
            }

            if ( ( s[i]=='0' ) || ( s[i]=='1' ) || ( s[i]=='2' ) || ( s[i]=='3' ) || ( s[i]=='4' ) || ( s[i]=='5' ) || ( s[i]=='6' ) || ( s[i]=='7' ) || ( s[i]=='8' ) || ( s[i]=='9' ) )
            {
                str=s.substr ( i );
                a=1;
            }

        }
    }

    return ( long long ) strtoll ( str.c_str(),0,10 );
}


///convert a string to a double
double
CStringUtils::stof ( std::string s )
{
    int a ( 0 );
    std::string str ( "" );

    for ( unsigned int i ( 0 ); s.length() >i; i++ )
    {
        if ( a==0 )
        {
            int b=i+1;
            if ( s[i]=='-' )
            {
                if ( ( s[b]=='0' ) || ( s[b]=='1' ) || ( s[b]=='2' ) || ( s[b]=='3' ) || ( s[b]=='4' ) || ( s[b]=='5' ) || ( s[b]=='6' ) || ( s[b]=='7' ) || ( s[b]=='8' ) || ( s[b]=='9' ) )
                {
                    str=s.substr ( i );
                    a=1;
                }
            }

            if ( ( s[i]=='0' ) || ( s[i]=='1' ) || ( s[i]=='2' ) || ( s[i]=='3' ) || ( s[i]=='4' ) || ( s[i]=='5' ) || ( s[i]=='6' ) || ( s[i]=='7' ) || ( s[i]=='8' ) || ( s[i]=='9' ) )
            {
                str=s.substr ( i );
                a=1;
            }

        }
    }

    return strtod ( str.c_str(),0 );
}

///convert a string to a vector of chars
std::vector<unsigned char>
CStringUtils::stov ( std::string s )
{
    std::vector<unsigned char> v;
    for ( unsigned int i ( 0 ); i<s.length(); i++ )
    {
        v.push_back ( s[i] );
    }

    return v;
}

/// convert a vector of chars to a string
std::string
CStringUtils::vtos ( std::vector<unsigned char> v )
{
    std::string str;
    for ( unsigned int i ( 0 ); i<v.size(); i++ )
    {
        str.push_back ( v[i] );
    }
    return str;
}


/// convert a string to a bool, case insensitive
bool
CStringUtils::stob ( std::string s )
{
    std::string ch=" ";
    std::string s1=CStringUtils::trim ( s,ch );
    std::string s2 = CStringUtils::toupper ( s1 );
    if ( s2 == "JA" || s2 == "Y" || s2 == "1" ||
            s2 == "TRUE" || s2 == "YES" )
        return true;
    return false;
}

/// convert a bool to a string
std::string
CStringUtils::btos ( bool b )
{
    return ( b ) ?  "TRUE" : "FALSE";
}

/// converts a char * to string
std::string
CStringUtils::chtos ( const char * char_ptr )
{
    std::string str ( char_ptr );
    return str;
}

/// convert a string to uppercase
std::string
CStringUtils::toupper ( std::string l )

{
    std::string r ( l );
    for ( unsigned int i=0; i < l.size(); i++ )
    {
        r[i] = static_cast<char> ( std::toupper ( l[i] ) );
    }
    return r;
}

/// convert a string to lowercase
std::string
CStringUtils::tolower ( std::string l )

{
    std::string r ( l );
    for ( unsigned int i=0; i < l.size(); i++ )
    {
        r[i] = static_cast<char> ( std::tolower ( l[i] ) );
    }
    return r;
}


std::string
CStringUtils::trim ( const std::string& s, const std::string& ch )
{
    std::string s2 ( s );
    unsigned int b ( s2.find_first_not_of ( ch ) );
    unsigned int e ( s2.find_last_not_of ( ch ) +1 );
    return s2.substr ( b, e-b );
}


std::string
CStringUtils::trimleft ( const std::string& s, const std::string& ch )
{
    std::string s2 ( s );
    return s2.substr ( s2.find_first_not_of ( ch ) );
}


std::string
CStringUtils::trimright ( const std::string& s, const std::string& ch )
{
    std::string s2 ( s );
    return s2.substr ( 0,s2.find_last_not_of ( ch ) +1 );
}

/// returns true if all characters in the string are digits, false otherwise
bool
CStringUtils::allDigits ( const std::string& s )
{
    if ( s.empty() )
        return false;
    for ( unsigned int i=0; i<s.length(); i++ )
    {
        if ( ( s[i] < '0' ) || ( s[i] > '9' ) )
            return false;
    }
    return true;
}


/**
   Replaces all occurences of  from in str by to and
   returns the resulting
*/
std::string
CStringUtils::replaceInString ( const std::string & str, const std::string & from, const std::string & to )
{
    unsigned int a ( str.find ( from,0 ) );
    if ( a >  str.length() )
    {
        return "";
    }

    std::string myStr ( str );
    while ( a < myStr.length() )
    {
        int b ( from.length() );
        myStr.replace ( a,b,to );
        a=myStr.find ( from,0 );
    }
    return myStr;
}

/**
compare two strings case insensetive
@return zero if identical
*/
int
CStringUtils::stricmp ( const std::string &a, const std::string &b )
{
    std::string c=tolower ( a );
    std::string d=tolower ( b );
    return c.compare ( d );
}

/** Converts a time_t structure into a string with the
    format DD.MM.YYYY HH:MM:SS
*/
std::string
CStringUtils::timetos ( const time_t& time )
{
    tm *nun ( 0 );
    nun = localtime ( &time );
    std::stringstream s;
    s << nun->tm_mday << '.' << nun->tm_mon+1 << '.' << nun->tm_year+1900 << ' ' << nun->tm_hour << ':' << nun->tm_min << ':' << nun->tm_sec << std::endl;

    return s.str();
}

/**
   Decoding a time string (format: DD.MM.YYYY HH.MM.SS)
 */
time_t
CStringUtils::stotime ( const std::string& text )
{
    if ( text.length() !=19 ) return 1;
    else
    {
        struct tm date;
        memset ( &date,0,sizeof ( date ) );
        int day ( stoi ( text.substr ( 0,2 ) ) );
        int mon ( stoi ( text.substr ( 3,2 ) ) );
        int year ( stoi ( text.substr ( 6,4 ) ) );
        int hour ( stoi ( text.substr ( 11,2 ) ) );
        int min ( stoi ( text.substr ( 14,2 ) ) );
        int sec ( stoi ( text.substr ( 17,2 ) ) );
        date.tm_mday=day;
        date.tm_mon=mon;
        date.tm_year=year-1900;
        date.tm_hour=hour;
        date.tm_min=min;
        date.tm_sec=sec;

        return mktime ( &date );
    }
    return 0;
}
/*
/// converts a string into a CTimer
CTimer
CStringUtils::stot ( const string& timestr )
{
	CStringTokenizer time_tok ( timestr,".",true );
	CStringTokenizer t_tok ( timestr,"." );


	if ( t_tok.countTokens() !=2 && t_tok.countTokens() !=1 )
	{
		string err_msg ( "CStringUtils.stot: too many tokens." );
		throw str_exception ( err_msg, 10 );
	}

	string sec = time_tok.nextToken();
	string usec = time_tok.nextToken();
	int msec ( stoi ( usec ) * ( int ) pow ( 10.0, ( 6.0 - usec.length() ) ) );

	string second = t_tok.nextToken();
	string usecond = t_tok.nextToken();
	int msecond ( stoi ( usecond ) * ( int ) pow ( 10.0, ( 6.0 - usec.length() ) ) );

	if ( second[0]=='-' )
	{
		string s = trimleft ( second,"-" );
		CTimer ts ( stoi ( s ), msecond, true );

		return ts;
	}
	if ( timestr[0]=='0' )
	{
		string a = trimleft ( timestr,"0" );
		return stot ( a );
	}

	if ( timestr[0]=='.' )
	{
		CTimer ts ( stoi ( sec ),msec );
		return ts;
	}
	else
	{

		CTimer ts ( stoi ( second ), msecond );
		return ts;
	}
	return 0;

}
*/
/// prints out delimited form of the digit

std::string
CStringUtils::decades_formatted ( const long long pDigit,
                                  const std::string pDelim )
{
    std::stringstream mySStream;
    std::string digitStr ( lltos ( pDigit ) );
    unsigned int  pos ( 0 ), endPos ( digitStr.size() % 3 );


    // assure, the endPos is behind the first digit
    if ( ( digitStr[0] == '+' || digitStr[0] == '-' ) && endPos == 1 )
    {
        endPos += 3;
    }



    if ( endPos == pos )
    {
        endPos +=3;
    }


    if ( endPos >= digitStr.size() )
    {
        endPos = digitStr.size();
    }

    // print out the first chunk
    mySStream << digitStr.substr ( pos, endPos - pos );

    // print chunks of the string
    while ( endPos < digitStr.size() )
    {
        pos = endPos;
        endPos +=3;
        endPos = ( endPos >= digitStr.size() ) ? digitStr.size() : endPos;

        mySStream << pDelim << digitStr.substr ( pos, endPos - pos );
    }
    return mySStream.str();
}




/// string tokenizer class. similar to the StringTokenizer in Java.
/// ctor
CStringTokenizer::CStringTokenizer ( std::string str, std::string delim, bool returnTokens, bool clusterDelimiters )
try :
    mString ( str ),
    mDelimiter ( delim ),
    mReturnTokens ( returnTokens ),
    mPos ( 0 ),
    mClusterDelimiters ( clusterDelimiters ),
    mLastReturnDelim ( true )
    {}
catch ( const std::bad_alloc & exception )
{
    LOG_ERROR << "CStringTokenizer::CStringTokenizer: Caught std::bad_alloc "
    "exception on string member initialization! Exception description: "
    << exception.what() << std::endl;
    throw;
}


/// dtor. non virtual, since this class should be regarded as final.
CStringTokenizer::~CStringTokenizer()
{
    // will come later
}


/// count amount of tokens, return the number of the not delivered tokens
/// if the current delimiter string ist used
int
CStringTokenizer::countTokens() const
{
    int tokensRigthwards = 0;
    int pos       = mPos;

    if ( !mClusterDelimiters )
    {
        while ( pos < ( int ) mString.length() )
        {
            int size = nonDelimiterSize ( pos );
            pos += size;
            size = delimiterSize ( pos );
            pos += size;
            if ( size > 0 ) tokensRigthwards++;
        }
        if ( mPos< ( int ) mString.length() )
        {
            if ( mReturnTokens )
            {
                tokensRigthwards*=2;
                if ( mLastReturnDelim ) tokensRigthwards++;
            }
            else
            {
                tokensRigthwards++;
            }
        }


    }
    // count tokens and delimiters, if returnTokens is set
    else
    {
        while ( pos < ( int ) mString.length() )
        {
            int size = delimiterSize ( pos );

            if ( size > 0 && mReturnTokens )
                tokensRigthwards++;
            pos += size;
            size = nonDelimiterSize ( pos );
            if ( size > 0 )
                tokensRigthwards++;
            pos += size;

        }
    }
    return tokensRigthwards;
}

/// count amount of tokens, return the number of the not delivered tokens
/// if the given delimiter string ist used
int
CStringTokenizer::countTokens ( std::string delim )
{
    try
    {
        mDelimiter = delim;
    }
    catch ( const std::bad_alloc & exception )
    {
        LOG_ERROR << "CStringTokenizer::countTokens ( std::string ): Caught "
        "std::bad_alloc on assigning delimiter string! Exception description: "
        << exception.what() << std::endl;
        throw;
    }
    return countTokens();
}

/// @return true if not jet all tokens are returned via nextToken()
bool
CStringTokenizer::hasMoreTokens() const
{
    return mPos < ( int ) mString.length();
}


/// return next token
std::string
CStringTokenizer::nextToken()
{
    try
    {
        int startPos ( mPos );
        int size;

        // subsequent calls garantee that we start on a nonDelimiter boundary.
        if ( ( mPos == 0 || mReturnTokens ) && mClusterDelimiters )
        {
            size = delimiterSize ( mPos );
            mPos += size;
            if ( size > 0 && mReturnTokens )
            {
                return mString.substr ( startPos, size );
            }
        }

        // get the word
        startPos = mPos;
        size = nonDelimiterSize ( mPos );

        if ( mReturnTokens && !mClusterDelimiters && size == 0 )
        {
            if ( mLastReturnDelim )
            {
                mLastReturnDelim = false;
                return "";
            }
            else
            {
                mLastReturnDelim = true;
                size++;
                mPos += size;
                return mString.substr ( startPos, size );
            }
        }
        mLastReturnDelim=false;
        mPos += size;

        // strip trailing delimiter, if not needed
        if ( !mReturnTokens )
            mPos += delimiterSize ( mPos );

        // return the token
        if ( size > 0 )
            return mString.substr ( startPos, size );
        else
            return "";
    }
    catch ( const std::bad_alloc & exception )
    {
        LOG_ERROR << "CStringTokenizer::nextToken(): Caught std::bad_alloc exception "
        "on returning next token! Exception description: " << exception.what()
        << std::endl;
        throw;
    }
    catch ( const std::length_error & exception )
    {
        LOG_ERROR << "CStringTokenizer::nextToken(): Caught std::length_error "
        "exception on returning next token! Exception description: "
        << exception.what() << std::endl;
        throw;
    }
}


/// return the next token by using a new token delimiter. The delimiter is
/// used for subsequent calls to nextToken()
std::string
CStringTokenizer::nextToken ( std::string delim )
{
    try {
        // store new delimiter
        mDelimiter = delim;
    }
    catch ( const std::bad_alloc & exception )
    {
        LOG_ERROR << "CStringTokenizer::nextToken ( std::string ): Caught "
        "std::bad_alloc on assigning delimiter string! Exception description: "
        << exception.what() << std::endl;
        throw;
    }
    return nextToken();
}

/// Returns the rest of the tokenizer
std::string
CStringTokenizer::remainder ( bool devourRemainder )
{
    try
    {
        if ( devourRemainder )
        {
            mPos = mString.length();
        }
        return mString.substr ( mPos );
    }
    catch ( const std::bad_alloc & exception )
    {
        LOG_ERROR << "CStringTokenizer::remainder: Caught std::bad_alloc on returning "
        "remainder string! Exception description: " << exception.what() << std::endl;
        throw;
    }
}


/// count the distance to the end  of next delimiter character
///from the actual position
int
CStringTokenizer::delimiterSize ( int pos ) const
{
    // actual position or the search start position is at the end of string
    if ( mPos >= ( int ) mString.length() || pos >= ( int ) mString.length() )
        return 0;
    // find the mark behind the last delimiter char
    int newPos=mString.find_first_not_of ( mDelimiter, pos );
    // many tokan chars are regarded as one delimiter
    if ( mClusterDelimiters )
    {
        if ( newPos >= 0 )
            return newPos-pos;
        else
            return mString.length()-pos;
    }
    // every character is a new delimiter
    else
    {
        // go step by step
        return 1;
    }
}


/// count the distance to the first non-delimiter character from the position
/// pos
int
CStringTokenizer::nonDelimiterSize ( int pos ) const
{

    // actual position or the search start position is at the end of string
    if ( mPos >= ( int ) mString.length() || pos >= ( int ) mString.length() )
        return 0;

    int newPos=mString.find_first_of ( mDelimiter, pos );
    if ( newPos > 0 )
        return newPos-pos;
    // we are at the start of string, beginning with a non delimiter character
    else if ( newPos == 0 && mPos == 0 )
        return 0;
    // there aren't any more delimiter characters
    else
        return mString.length()-pos;
}

