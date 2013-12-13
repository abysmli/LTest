// This may look like C code, but it is really -*- C++ -*-
//
// $Id: network.cpp,v 2.12 2005/07/19 15:46:19 siemens Exp $
//
// Author:  Eduard Siemens <siemens@rvs.uni-hannover.de>
//
// File:    network.cpp
//          Wed May 07 2003
//
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
//    * All advertising materials mentioning features or use of this software
//      must display the following acknowledgement:
//      This product includes software developed by Eduard Siemens
//      University of Hannover.

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

// $Log:   //hanvsfile02/prog/Projects/2005/hipernet/Repository/hipernet/archives/hipernet/Linux/Apps/ltest/network.cpp-arc  $
//
//   Rev 1.8.1.18   10 Apr 2007 17:45:46   siemense
//performance monitor interface added
//
//   Rev 1.8.1.17   03 Apr 2007 21:51:56   siemense
//changed back to sendmsg
//
//   Rev 1.8.1.14   30 Mar 2007 18:11:40   siemense
//double calll in RecvV bug fixed
//
//   Rev 1.8.1.13   27 Mar 2007 09:03:36   siemense
//minor bug fixed
//
//   Rev 1.8.1.12   26 Mar 2007 16:04:50   siemense
//additional CLDUTSocket constructor added
//
//   Rev 1.8.1.11   22 Mar 2007 17:28:28   siemense
//buuuug fixed
//
//   Rev 1.8.1.10   22 Mar 2007 17:16:54   siemense
//const conversion bug fixed
//
//   Rev 1.8.1.9   19 Mar 2007 14:36:26   siemense
//new Accept interface for udp added
//
//   Rev 1.8.1.8   16 Mar 2007 14:07:06   austa
//add errno debug output to Connect
//
//   Rev 1.8.1.6   22 Feb 2007 09:06:24   siemense
//GetProto accessor to the socket protocol has been added
//
//   Rev 1.8.1.5   31 Jan 2007 09:11:32   siemense
//Efficiency issues solved. Interfaces to network classes has been cleaned up.
//
//   Rev 1.8.1.4   21 Dec 2006 14:15:22   siemense
//CIPSocket extended by BRecv and TRecv (not yet testsed)
//
//   Rev 1.8.1.3   13 Nov 2006 11:57:38   siemense
//Minor bug fixes.
//Version before CIPNetworkPerformance tunings.
//
//   Rev 1.8.1.1   17 Aug 2006 13:21:12   siemense
//TSC timer class has been implemented and rudimentary tested
//
//   Rev 1.8.1.0   Jun 27 2006 10:40:22   siemense
//New Protocols Branch
//
//   Rev 1.8   16 Feb 2006 16:10:14   siemense
//FullSpeed source added. Minor bugs fixed.
//
//   Rev 1.7   08 Feb 2006 13:25:42   siemense
//Infiniband support added
//
//   Rev 1.6   26 Jan 2006 10:06:00   siemense
//Intermediate checkin
//
//   Rev 1.5   13 Jan 2006 14:53:42   siemense
//Local address assignment bug fixed
//
//   Rev 1.4   21 Dec 2005 16:42:30   siemense
//Time correction between data sender and data recei ver added
//
//   Rev 1.3   15 Nov 2005 15:10:40   siemense
//zero duration and packet count bug fixed
//
//   Rev 1.2   07 Oct 2005 09:11:28   siemense
//unnecessary files removed
//
//   Rev 1.1   06 Oct 2005 17:06:58   siemense
//data rate limit extended
//
//   Rev 1.0   28 Sep 2005 16:38:32   siemense
//Initial revision.
//
//   Rev 1.0   28 Sep 2005 16:34:00   siemense
//Initial revision.
// Revision 2.12  2005/07/19 15:46:19  siemens
// Final release from Eduard Siemens at the RVS
//
// Revision 2.11  2005/02/23 09:16:08  siemens
// allDigits bug fixed
//
// Revision 2.10  2005/02/07 14:43:14  siemens
// bereinigung von fremden Code
//
// Revision 2.9  2004/07/02 21:20:28  siemens
// TCP connection closing bug fixed
//
// Revision 2.8  2004/07/01 19:48:09  siemens
// intermediate version, tcp performance bug is still there
//
// Revision 2.7  2004/05/25 14:08:29  siemens
// BSD license copyright notice added
//
// Revision 2.6  2004/04/02 14:30:35  siemens
// 100 percent error bug fixed
//
// Revision 2.5  2004/02/24 16:41:30  siemens
// xml config support added
//
// Revision 2.4  2003/12/30 10:26:27  siemens
// rewritten config staff and command line parsing
//
// Revision 2.3  2003/12/11 13:32:58  siemens
// Working version with remote source
//
// Revision 2.2  2003/09/10 12:57:03  siemens
// checkin before chnging to distributed system
//
// Revision 2.1  2003/09/04 13:30:16  siemens
// priority scheduling tuned
//
// Revision 2.0  2003/06/30 07:06:13  siemens
// beta release
//
// Revision 1.5  2003/06/16 14:02:24  siemens
// *** empty log message ***
//
// Revision 1.4  2003/06/11 07:50:00  siemens
// Alfha release for ltest
//
// Revision 1.3  2003/06/04 14:47:04  siemens
// *** empty log message ***
//
// Revision 1.2  2003/05/27 11:47:02  siemens
// last checkin before alpha release
//
// Revision 1.1  2003/05/13 14:07:21  siemens
// *** empty log message ***
//
//



using namespace std;

//#include <math.h>
extern "C"
{
#include <cerrno>
#include <errno.h>
};

#include <unistd.h>
#include <cstring>
#include "network.h"
#include "strtok.h"

#ifdef solaris
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/ip.h>
#endif // solaris

#ifdef CUSTOM_CC
//#include "cc.h"
#endif
#include "my_cerrno.h"

#include "luLogger.h"

// default values for JET/UDP buffers
static int defaultJET_BUF = 32 * 1024 * 1024; // 32 MBytes
static int defaultUDP_BUF = 28 * 1024 * 1024; // 32 MBytes
static int defaultJET_MSS = 1500; // default MSS for ethernets

//KOEH inconsistency with linux header files, errno is already defined
//extern "C" int errno;

static Cmy_cerrno Cerrno;

// static CCPUUsageTracker * Stracker = new CCPUUsageTracker();

/** ctor: create an empty socket class. Can't really used for real communication. But
 * can be used as pre-created pools of sockets and later real opened sockets can be assigned to
 * this one
 */
CIPSocket::CIPSocket() :
		mSockFD ( -1 ),
		mBindAddr ( INADDR_ANY ),
		mBindPort ( 0 ),
		mLocalAddr ( 0 ),
		mLocalPort ( 0 ),
		mOpen ( true ),
		mConnected ( false ),
		mConnectedFrom ( false ),
		mTOS ( 0 )
{
	init_socket();
}

/// ctor: get an socket of type pType and bind to first free port
CIPSocket::CIPSocket ( int pType, SocketProto_t pProto,  bool listen, unsigned char pTOS ) :
		mSockFD ( -1 ),
		mProto ( pProto ),
		mBindAddr ( INADDR_ANY ),
		mBindPort ( 0 ),
		mLocalAddr ( 0 ),
		mLocalPort ( 0 ),
		mOpen ( true ),
		mConnected ( false ),
		mConnectedFrom ( false ),
		mTOS ( pTOS )
{
	init_socket();
	int Domain = PF_INET; // address domain - supported domains are PF_INET and AF_INET_SDP

#ifdef SDP_ENABLED

	if ( pProto == S_SDP )
	{
		Domain = AF_INET_SDP;
	}
#endif // SDP_ENABLED

	mSockFD = socket ( Domain, pType, 0 );

	// set TOS field. Only defined for PF_INET services
	if ( mTOS != 0 && Domain == PF_INET )
	{
		int res;
#ifdef linux

		res = setsockopt ( mSockFD, SOL_IP, IP_TOS, ( int* ) &mTOS, sizeof ( int ) );
#elif solaris

		res = setsockopt ( mSockFD, IPPROTO_IP, IP_TOS, ( int * ) &mTOS, sizeof ( int ) );
#endif // linux, solaris

		if ( res == -1 )
		{
			LOG_NOTICE << "can't set TOS field" << endl;
		}
	}

	LOG_INFO << "Socket with descriptor number: " << mSockFD << " opened" << endl;
	if ( mSockFD == -1 )
	{
		mOpen = false;
		// throw CNetwork_exception("can't get socket", errno);
		throw CNetwork_exception ( "socket system call failed. Error is: "
		                           + Cerrno[errno].m_name + " " + Cerrno[errno].m_desc, 0,
		                           errno );
	}
	// prepare listening socket, but don't set into listen state,
	// this would be done by the Listen method
	if ( listen )
	{
		// the very first thing on servers is to set O_REUSEADDR
		const int on ( 1 );
		setsockopt ( mSockFD, SOL_SOCKET, SO_REUSEADDR, &on, sizeof ( on ) );
		// bind to the socket, if a listening socket
		struct sockaddr_in sin;
		memset ( ( char * ) &sin, 0, sizeof (
		             sin ) );
		sin.sin_port = htons ( mBindPort );
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = htonl ( INADDR_ANY );
		if ( bind ( mSockFD, ( struct sockaddr * ) &sin, sizeof ( sin ) ) < 0 )
			throw CNetwork_exception ( "cannot bind socket. Error is: "
			                           + Cerrno[errno].m_name + " " +
			                           Cerrno[errno].m_desc, 0, errno );
		memset ( ( char * ) &sin, 0, sizeof ( sin ) );
		socklen_t len ( sizeof ( sin ) );
		getsockname ( mSockFD, ( sockaddr * ) &sin, &len );
		mBindPort = ntohs ( sin.sin_port );
	}
}

////////////////////////////////////////////////////////////////////////
/// ctor: get an socket of type pType and bind to pPort and pIPAddress
////////////////////////////////////////////////////////////////////////
CIPSocket::CIPSocket ( int pType,
                       SocketProto_t pProto,
                       unsigned short pPort,
                       unsigned int pIPAddress,
                       bool listen,
                       unsigned char pTOS ) :
		mSockFD ( -1 ),
		mProto ( pProto ),
		mBindAddr ( pIPAddress ),
		mBindPort ( pPort ),
		mLocalAddr ( 0 ),
		mLocalPort ( 0 ),
		mOpen ( true ),
		mConnected ( false ),
		mConnectedFrom ( false ),
		mTOS ( pTOS )
{
	init_socket();
	int Domain = PF_INET; // address domain - supported domains are PF_INET and AF_INET_SDP

#ifdef SDP_ENABLED

	if ( pProto == S_SDP )
	{
		Domain = AF_INET_SDP;
	}
#endif // SDP_ENABLED

	mSockFD = socket ( Domain, pType, 0 );

	if ( mSockFD == -1 )
	{
		mOpen = false;
		//    throw CNetwork_exception("can't get socket", errno);
		throw CNetwork_exception ( "can't get socket on port " +
		                           CStringUtils::itos ( pPort ) );
	}
	// the very first thing on servers is to set O_REUSEADDR
	const int on ( 1 );
	setsockopt ( mSockFD, SOL_SOCKET, SO_REUSEADDR, &on, sizeof ( on ) );
	// set TOS
	if ( 0 != mTOS && Domain == PF_INET )
	{
		int res;
#ifdef linux

		res = setsockopt ( mSockFD, SOL_IP, IP_TOS, ( int* ) &mTOS, sizeof ( int ) );
#elif solaris

		res = setsockopt ( mSockFD, IPPROTO_IP, IP_TOS, ( int * ) &mTOS, sizeof ( int ) );
#endif // linux, solaris

		if ( res == -1 )
		{
			LOG_NOTICE << "can't set TOS field" << endl;
		}
	}

	// prepare listening socket, but don't set into listen state,
	// this would be done by the Listen method
	if ( listen )
	{
		// the very first thing on servers is to set O_REUSEADDR
		const int on ( 1 );
		setsockopt ( mSockFD, SOL_SOCKET, SO_REUSEADDR, &on, sizeof ( on ) );
		//bind to socket, if a listening socket
		struct sockaddr_in sin;
		memset ( ( char * ) &sin, 0, sizeof ( sin ) );
		sin.sin_port = htons ( mBindPort );
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = htonl ( mBindAddr );
		if ( bind ( mSockFD, ( struct sockaddr * ) &sin, sizeof ( sin ) ) < 0 )
		{

			throw CNetwork_exception ( "can't bind socket to port " +
			                           CStringUtils::itos ( pPort ) );

		}
		memset ( ( char * ) &sin, 0, sizeof ( sin ) );
		socklen_t socklen ( sizeof ( sin ) );
		getsockname ( mSockFD, ( struct sockaddr * ) &sin, &socklen );
		mLocalAddr = ntohl ( sin.sin_addr.s_addr );
		mBindPort = mLocalPort = ntohs ( sin.sin_port );
		LOG_NOTICE << "local Socket bound to address: " << CIPAddress::atodd ( mLocalAddr )
		<<" Port: " << mLocalPort << endl;
	}
}

/// dtor: close connection
CIPSocket::~CIPSocket()
{
	// Stracker->PrintAllTrackers(cout);
	if ( mSockFD != -1 )
	{
		this->Close();
	}
}


/// socket initialization measures, common to all constructors
void
CIPSocket::init_socket()
{
	// prepare for fast select operations
	FD_ZERO ( &mFdSet );
	mSelectTimer.tv_sec = 0;
	mSelectTimer.tv_usec = 0;
	mTmpSelectTimer = mSelectTimer;

	// prepare the message headeer sturcture
	if ( mProto == S_UDP )
	{
		mRecvMsg.msg_name = mInAddrBuff;
		mRecvMsg.msg_namelen  = sizeof ( mInAddrBuff );
		memset ( mInAddrBuff, 0, sizeof ( mInAddrBuff ) );
	}
	else
	{
		mRecvMsg.msg_name = NULL;
		mRecvMsg.msg_namelen = 0;
	}
	mRecvMsg.msg_control     = NULL;
	mRecvMsg.msg_controllen  = 0;
	// send message header
	mSendMsg.msg_name       = NULL;
	mSendMsg.msg_namelen    = 0;
	mSendMsg.msg_control    = NULL;
	mSendMsg.msg_controllen = 0;

}

/// get port number
unsigned short
CIPSocket::BindPort()
{
	return mBindPort;
}


/// get Socket's bind address
unsigned long
CIPSocket::BindAddr()
{
	return mBindAddr;
}


/**
* set the timeout timer - the mSelectTimer as well as the SO_RECVTIMEO option of the socket
 */
bool
CIPSocket::SetRecvTimeout ( const HPTimer::HighPerTimer & pTimeout )
{

	struct timeval tv_set;
	pTimeout.SetTV ( tv_set );
	mSelectTimer = tv_set;
	int result = setsockopt ( mSockFD, SOL_SOCKET, SO_RCVTIMEO, &tv_set, sizeof ( tv_set ) );
	if ( 0 == result )
	{
		return true;
	}
	return false;
}

/**
 * set the sending timeout timer - the SO_RECVTIMEO option of the socket
 */
bool
CIPSocket::SetSendTimeout ( const HPTimer::HighPerTimer & pTimeout )
{
	struct timeval tv_set;
	pTimeout.SetTV ( tv_set );
	mSelectTimer = tv_set;
	int result = setsockopt ( mSockFD, SOL_SOCKET, SO_SNDTIMEO, &tv_set, sizeof ( tv_set ) );
	if ( 0 == result )
	{
		return true;
	}
	return false;
}



/** get peer's IP address, if not already connected to the peer,
    sets and returns the peer address, associated with the socket
    (essentially useful for sockets receiving UDP data packets)
 */
// address of the remote side of socket. On client side socket set only after connect
CIPAddress
CIPSocket::RemoteAddr()
{
	return mRemoteAddr;
}



// address of the origin of the socket

/* CIPAddress
CIPSocket::OriginAddr()
{
        return mOriginAddr;
}; */


/// close the opened socket, unset the open flag
void
CIPSocket::Close()
{
	if ( mOpen )
	{
		mConnected = false;
		mConnectedFrom = false;
		shutdown ( mSockFD,SHUT_RD );
		sleep ( 1 );
            ::close ( mSockFD );
		mOpen = false;
	}
}


/**
   connect to the remote peer
*/
bool
CIPSocket::Connect ( CIPAddress & pAddress )
{
	return Connect ( pAddress.IPAddress(), pAddress.Port() );
}


/// connect to the remote peer, set assigned local ip address and port number
bool
CIPSocket::Connect ( const unsigned int pServer, const unsigned short pPort )
{
	struct sockaddr_in sin;

	if ( pServer == INADDR_ANY || pPort == 0 )
	{
		throw CNetwork_exception ( ( string ) "Can't connect: invalid remote host address or " +
		                           ( string ) "port number " + CIPAddress::atodd ( pServer ) + ":" +
		                           CStringUtils::itos ( pPort ), 2 );
	}
	memset ( ( char * ) &sin, 0, sizeof ( sin ) );
	sin.sin_port = htons ( pPort );
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl ( pServer );
	mRemoteAddr.IPAddress ( pServer );
	mRemoteAddr.Port ( pPort );
	if ( connect ( mSockFD, ( sockaddr * ) &sin, sizeof ( sin ) ) != -1 )
	{
		// mBindAddr = ntohl(sin.sin_addr.s_addr); /* TODO: must be changed! */
		// mBindPort = ntohs(sin.sin_port);

		struct sockaddr_in sockAddr;
		int size ( sizeof ( sockAddr ) );
		memset ( ( char * ) &sockAddr, 0, size );
		if ( getsockname ( mSockFD, ( struct sockaddr * ) &sockAddr, ( socklen_t * ) &size ) == 0 )
		{
			mLocalAddr = ntohl ( sockAddr.sin_addr.s_addr );
			mLocalPort = ntohs ( sockAddr.sin_port );
		}
		mConnected = true;
		return true;
	}
	LOG_ERROR << "couldn't connect: error code: " << errno << " reason:  " << Cerrno[errno].m_name  <<" "  << Cerrno[errno].m_desc << endl;
	return false;

}

// set the connectFrom* variables
void CIPSocket::SetConnFrom ( sockaddr_in & pSAddr )
{
	mRemoteAddr.IPAddress ( ntohl ( pSAddr.sin_addr.s_addr ) );
	mRemoteAddr.Port ( ntohs ( pSAddr.sin_port ) );
	mConnectedFrom = true;

}


/// send data on an socket
int
CIPSocket::Send ( const char * msg, const size_t len )
{
	ssize_t bytes;
	if ( ( bytes = send ( mSockFD, ( void * ) msg, len, 0 ) ) > 0 )
		return bytes;
	else if ( 0 == bytes )
	{
		/* Remote side connection has been closed or resetted */
		return -1;
	}
	else
	{ // -1 returned
		switch ( errno )
		{
			case EINTR: // the send call is interrupted by a signal
#ifdef DEBUG_OUTPUT
				LOG_LU_INFO << "the send call within " << __PRETTY_FUNCTION__
				<< " has been interrputed"      << endl;
#endif

			case EAGAIN:
#ifdef DEBUG_OUTPUT

				LOG_LU_DEBUG << "the send call within " << __PRETTY_FUNCTION__
				<< " has been timed out"     << endl;
#endif

				return 0;
		}
		// all other errors means an brocken socket communication. We can't receive
		// data any more. Throw an exception
		throw CNetwork_exception ( "cannot send data on socket. Error is: "
		                           + Cerrno[errno].m_name + " " + Cerrno[errno].m_desc,
		                           2, errno );
	}
}


/// send data on an socket
int
CIPSocket::SendV ( iovec * iov, const int vLen )
{
	ssize_t bytes;
	mSendMsg.msg_iov = iov;
	mSendMsg.msg_iovlen = vLen;
	// internally, send and sendto maps to sendmsg
	if ( ( bytes = sendmsg ( mSockFD, &mSendMsg,  0 ) ) > 0 )
		return bytes;
	else if ( 0 == bytes )
	{
		/* Remote side connection has been closed or resetted */
		return -1;
	}
	else
	{ // -1 returned
		switch ( errno )
		{
			case EINTR: // the send call is interrupted by a signal
#ifdef DEBUG_OUTPUT

				LOG_LU_DEBUG << "the send call within " << __PRETTY_FUNCTION__
				<< " has been interrputed"      << endl;
#endif

			case EAGAIN:
#ifdef DEBUG_OUTPUT

				LOG_LU_DEBUG << "the send call within " << __PRETTY_FUNCTION__
				<< " has been timed out"     << endl;
#endif

				return 0;
		}
		// all other errors means an brocken socket communication. We can't receive
		// data any more. Throw an exception
		throw CNetwork_exception ( "cannot send data on socket. Error is: "
		                           + Cerrno[errno].m_name + " " + Cerrno[errno].m_desc,
		                           2, errno );
	}
}


/// ctor: get a TCP socket and bind to first free port
CTCPSocket::CTCPSocket ( bool pListen, unsigned char pTOS ) :
		CIPSocket ( SOCK_STREAM, S_TCP, pListen, pTOS )
{
	// struct protoent* proto = getprotobyname("TCP");
	// mProto = proto->p_proto;
}

/// ctor: get a TCP socket and bind to the given Address and port
CTCPSocket::CTCPSocket ( bool pListen,
                         unsigned short pPort,
                         unsigned int pIPAddress,
                         unsigned char pTOS ) :
		CIPSocket ( SOCK_STREAM, S_TCP, pPort, pIPAddress, pListen, pTOS )
{
	// struct protoent* proto = getprotobyname("TCP");
	// mProto = proto->p_proto;
}

///dtor:
CTCPSocket::~CTCPSocket()
{}

/// listen on the port address
bool
CTCPSocket::Listen ( const int pBacklog )
{
	if ( listen ( mSockFD, pBacklog ) != -1 )
		return true;
	return false;
}
//////////////////////////////////////////////////////////////////////////
/// accept an incoming connection
/// @return true if successfull, false if the accept call has been timed out.
/// On an error it thorws an CNetwork_exception
//////////////////////////////////////////////////////////////////////////
CIPSocket *
CTCPSocket::Accept ( bool pSingleton )
{
	struct sockaddr_in sin;
	memset ( ( char * ) &sin, 0, sizeof ( sin ) );
	socklen_t sin_size ( sizeof ( sin ) );
	int newSock ( -1 );

	newSock = accept ( mSockFD, ( sockaddr * ) &sin, &sin_size );

	if ( -1 == newSock )
	{
		switch ( errno )
		{
			case EINTR: // the recv call is interrupted by a signal
#ifdef DEBUG_OUTPUT

				LOG_LU_DEBUG << "the recv call within " << __PRETTY_FUNCTION__
				<< " has been interrputed"      << endl;
#endif

				return NULL;

			case ECONNABORTED: // the recv call is interrupted by a signal
#ifdef DEBUG_OUTPUT

				LOG_LU_DEBUG << "the recv call within " << __PRETTY_FUNCTION__
				<< " has been aborted"                 << endl;
#endif

				return NULL;
			case EAGAIN:
#ifdef DEBUG_OUTPUT

				LOG_LU_DEBUG << "the recv call within " << __PRETTY_FUNCTION__
				<< " has been timed out"        << endl;
#endif

				return NULL;

		}
		throw CNetwork_exception ( "Error on syscall accept() on socket with fd " +
		                           CStringUtils::itos ( mSockFD ) + ". Error is: " +
		                           Cerrno[errno].m_name + " " + Cerrno[errno].m_desc + ".",
		                           3, errno );
	} // end of newSock == -1

	CIPSocket * connSock_ptr; // the connected socket
	if ( pSingleton )
	{
            ::close ( mSockFD );
		mSockFD = newSock;
		connSock_ptr = this;
	}
	else
	{ // create a new socket, clone this to that socket and set the
		// connected state related data
		// TODO Since this pattern is common to all types of sockets, it cries for a
		// template. At one later point in time we will really implement this as a template
		connSock_ptr = new CTCPSocket ( *this );

	}
	// store the new filedescriptor
	mSockFD = newSock;

	//  connSock_ptr->mBindAddr = ntohl(sin.sin_addr.s_addr);
	connSock_ptr->SetConnFrom ( sin );
	/* connSock_ptr->mRemoteAddr.IPAddress(ntohl(sin.sin_addr.s_addr));
	connSock_ptr->mRemoteAddr.Port(ntohs(sin.sin_port));
	connSock_ptr->mConnectedFrom = true; */
	return connSock_ptr;
}

//////////////////////////////////////////////////////////
/// recv data on an socket using select call before each read.
/// the select timer must be set with SetRecvTimeout()
/// @returns the amount of received data. Zero is returned, if read call
/// has been timed out. -1 is returned, if remote side has closet the connection
//////////////////////////////////////////////////////////
int
CTCPSocket::SRecv ( const char * msg, const size_t len )
{
	ssize_t bytes ( 0 );
	FD_SET ( mSockFD, &mFdSet );

	if ( ( ( select ( mSockFD+1, &mFdSet, NULL, NULL, &mTmpSelectTimer ) ) >0 ) &&
	        FD_ISSET ( mSockFD, &mFdSet ) )
	{
		if ( ( bytes = recv ( mSockFD, ( char * ) msg, len, 0 ) ) > 0 )
		{
			mTmpSelectTimer = mSelectTimer;
			return bytes;
		}
		else if ( bytes == 0 )
		{
			// remote side has been closed or resetted the connection
			return -1;
		}
		else /* bytes == -1: might by an connection close, network failure */
		{
			switch ( errno )
			{
				case EINTR: // the recv call is interrupted by a signal
#ifdef DEBUG_OUTPUT

					LOG_LU_DEBUG << "the recv call within " << __PRETTY_FUNCTION__
					<< " has been interrputed"      << endl;
#endif

					mTmpSelectTimer = mSelectTimer;
					return 0;
			}
			// all other errors means an brocken socket communication. We can't receive
			// data any more. Throw an exception
			throw CNetwork_exception ( "can't receive data on socket. Error is: "
			                           + Cerrno[errno].m_name + " " + Cerrno[errno].m_desc,
			                           2, errno );

		}
	}
	// we reset the timer after read call, becaouse we hope, we have now at least a few microseconds
	// time till the next packet will arrive
	mTmpSelectTimer = mSelectTimer;
	return 0;
}


/** blocking recv data on socket
   * @return the amount of bytes, if some data received, 0, if timed out.
 * -1, if an FIN or RST received from the remote side
 * It throws an   * CNetwork_exception, if an error has been occured
 */
int
CTCPSocket::Recv ( const char * msg, const size_t len )
{
	ssize_t bytes ( 0 );

	if ( ( bytes = recv ( mSockFD, ( char * ) msg, len, 0 ) ) > 0 )
	{
		return bytes;
	}
	else if ( bytes == 0 ) /* Remote side connection has been closed or resetted */
	{
		return -1;
	} else /* bytes == -1: might by an connection close, network failure */
	{
		switch ( errno )
		{
			case EINTR: // the recv call is interrupted by a signal
#ifdef DEBUG_OUTPUT

				LOG_LU_DEBUG << "the recv call within " << __PRETTY_FUNCTION__ << " has been interrputed." << endl;
#endif

				return 0;
			case EAGAIN:
#ifdef DEBUG_OUTPUT

				LOG_LU_DEBUG << "the recv call within " << __PRETTY_FUNCTION__ << " has been timed out" << endl;
#endif

				return 0;
		}
		// all other errors means an brocken socket communication. We can't receive
		// data any more. Throw an exception
		throw CNetwork_exception ( "can't receive data on socket. Error is: " +
		                           Cerrno[errno].m_name + " " + Cerrno[errno].m_desc, 2, errno );

	}
        LOG_INFO << "We never should arrive this point" << endl;
	return -1; /* should never be reached */
}

//////////////////////////////////////////////////////////////////////////
/// recv data from the socket using timer pTimer
/// returns the amount of bytes read or zero if select timed out. Returns -1, if the connection is closed
/// by the remote side
//////////////////////////////////////////////////////////////////////////
int
CTCPSocket::TRecv ( const char * msg, const size_t len,
                    HPTimer::HighPerTimer & pTimer )
{
	ssize_t bytes ( 0 );

	FD_SET ( mSockFD, &mFdSet );
	pTimer.SetTV ( mTmpSelectTimer );

	if ( ( ( select ( mSockFD+1, &mFdSet, NULL, NULL, &mTmpSelectTimer ) ) >0 ) &&
	        FD_ISSET ( mSockFD, &mFdSet ) )
	{
		if ( ( bytes = recv ( mSockFD, ( char * ) msg, len, 0 ) ) > 0 )
		{
			pTimer.SetTV ( mTmpSelectTimer );
			return bytes;
		}
		else if ( bytes == 0 ) // closed at the remote side
		{
			pTimer.SetTV ( mTmpSelectTimer ); // we need this, because subse
			return -1;
		}
	}
	else
	{
		mTmpSelectTimer = mSelectTimer;
		throw CNetwork_exception ( "can't receive data on socket. Error is: "
		                           + Cerrno[errno].m_name + " " + Cerrno[errno].m_desc,
		                           2, errno );
	}

	// we reset the timer after read call, becaouse we hope, we have now at least a few microseconds
	// time till the next packet will arrive
	pTimer.SetTV ( mTmpSelectTimer );
	return 0;
}


/** blocking recv data on socket using iovec structure
 * @return the amount of bytes, if some data received, 0, if timed out.
 * -1, if an FIN or RST received from the remote side
 * It throws an   * CNetwork_exception, if an error has been occured
 */
int
CTCPSocket::RecvV ( iovec * iov, const int vLen )
{
	ssize_t bytes ( 0 );
	mRecvMsg.msg_iov = iov;
	mRecvMsg.msg_iovlen = vLen;

	if ( ( bytes = recvmsg ( mSockFD, &mRecvMsg, MSG_WAITALL ) ) > 0 )
	{
		// error: at least one datagram has been truncated!
		if ( mRecvMsg.msg_flags & MSG_TRUNC )
		{
			throw CNetwork_exception ( "Read message has been truncated! Data are lost!: "
			                           + Cerrno[errno].m_name + " " + Cerrno[errno].m_desc,
			                           3, errno );
		}

		return bytes;
	}
	else if ( bytes == 0 ) /* Remote side connection has been closed or resetted */
	{
		return -1;
	} else /* bytes == -1: might by an connection close, network failure */
	{
		switch ( errno )
		{
			case EINTR: // the recv call is interrupted by a signal
#ifdef DEBUG_OUTPUT

				LOG_LU_DEBUG << "the recv call within " << __PRETTY_FUNCTION__ << " has been interrputed." << endl;
#endif

				return 0;
			case EAGAIN:
#ifdef DEBUG_OUTPUT

				LOG_LU_DEBUG << "the recv call within " << __PRETTY_FUNCTION__ << " has been timed out" << endl;
#endif

				return 0;
		}
		// all other errors means an brocken socket communication. We can't receive
		// data any more. Throw an exception
		throw CNetwork_exception ( "can't receive data on socket. Error is: " +
		                           Cerrno[errno].m_name + " " + Cerrno[errno].m_desc, 2, errno );

	}
        LOG_INFO << "We never should arrive this point" << endl;
	return -1; /* should never be reached */
}




/// ctor: get an UDP socket and bind to first free port
CUDPSocket::CUDPSocket ( bool pListen, unsigned char pTOS ) :
		CIPSocket ( SOCK_DGRAM, S_UDP, pListen, pTOS )
{
	// struct protoent* proto = getprotobyname("UDP");
	// mProto = proto->p_proto;
}

/// ctor: get an UDP socket and bind to the given Address and port
CUDPSocket::CUDPSocket ( bool pListen,
                         unsigned short pPort,
                         unsigned int pIPAddress,
                         unsigned char pTOS ) :
		CIPSocket ( SOCK_DGRAM, S_UDP, pPort, pIPAddress, pListen, pTOS )
{
	// struct protoent* proto = getprotobyname("UDP");
	// mProto = proto->p_proto;
}

////////////////////////////////////////////////////////////
///dtor:
////////////////////////////////////////////////////////////
CUDPSocket::~CUDPSocket()
{}

////////////////////////////////////////////////////////////
/// listen, only for interface conformance with tcp socket
////////////////////////////////////////////////////////////
bool
CUDPSocket::Listen ( const int pBacklog )
{
	return true;
}

////////////////////////////////////////////////////////////
/// accept, only for interface conformance with tcp socket
////////////////////////////////////////////////////////////
CIPSocket *
CUDPSocket::Accept ( bool pSingleton )
{
	return this;
}

/// accept. Set the remote address, datagrams should be sent to
CIPSocket *
CUDPSocket::Accept ( CIPAddress & pRemoteAddr, bool pSingleton )
{
	struct sockaddr_in sin;

	if ( pRemoteAddr.HasWildcard() )
	{
		throw CNetwork_exception ( ( string ) "Can't accept: a valid unicast network address and port number must be supplied "
		                           + ( string ) CIPAddress::atodd ( pRemoteAddr.IPAddress() ) + ":" +  CStringUtils::itos ( pRemoteAddr.Port() ), 2 );
	}
	memset ( ( char * ) &sin, 0, sizeof ( sin ) );
	sin.sin_port = htons ( pRemoteAddr.Port() );
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl ( pRemoteAddr.IPAddress() );
	mRemoteAddr = pRemoteAddr;
	if ( connect ( mSockFD, ( sockaddr * ) &sin, sizeof ( sin ) ) != -1 )
	{
		// mBindAddr = ntohl(sin.sin_addr.s_addr); /* TODO: must be changed! */
		// mBindPort = ntohs(sin.sin_port);

		struct sockaddr_in sockAddr;
		int size ( sizeof ( sockAddr ) );
		memset ( ( char * ) &sockAddr, 0, size );
		if ( getsockname ( mSockFD, ( struct sockaddr * ) &sockAddr, ( socklen_t * ) &size ) == 0 )
		{
			mLocalAddr = ntohl ( sockAddr.sin_addr.s_addr );
			mLocalPort = ntohs ( sockAddr.sin_port );
		}
		mConnected = true;
		return this;
	}
	return NULL;

}


//////////////////////////////////////////////////////////////
/// recv data on an UDP socket
//////////////////////////////////////////////////////////////
int
CUDPSocket::SRecv ( const char * msg, const size_t len )
{
	//        Stracker->TrackIn(1);
	int bytes ( 0 );
	FD_SET ( mSockFD, &mFdSet );
	if ( ( select ( mSockFD+1, &mFdSet, NULL, NULL, &mTmpSelectTimer ) ) >0 &&
	        FD_ISSET ( mSockFD, &mFdSet ) )
	{
		// if no data packets yet received from a peer, save the sender address
		if ( ! mConnectedFrom )
		{
			struct sockaddr_in from;
			socklen_t  addrLen ( sizeof ( from ) );
			//                        ->TrackOut(1);
			bytes = recvfrom ( mSockFD, ( char * ) msg, len, 0,
			                   ( struct sockaddr * ) &from, &addrLen );
			// fetch the sender address
			// Stracker->TrackIn(2);
			if ( bytes > 0 )
			{
				mRemoteAddr.IPAddress ( ntohl ( from.sin_addr.s_addr ) );
				mRemoteAddr.Port ( ntohs ( from.sin_port ) );
				mConnectedFrom = true;
			}
			//                        Stracker->TrackOut(2);

		}
		else
		{
			//                       Stracker->TrackOut(1);
			bytes = recv ( mSockFD, ( char * ) msg, len, 0 );
			//                        Stracker->TrackIn(2);
		}

		if ( bytes > 0 )
		{
			mTmpSelectTimer = mSelectTimer;
			//                        Stracker->TrackOut(2);
			return bytes;
		}
		else
		{
			mTmpSelectTimer = mSelectTimer;
			//                        Stracker->TrackOut(2);
			throw CNetwork_exception ( "can't receive data on socket " + Cerrno[errno].m_name + " "
			                           + Cerrno[errno].m_desc, 2, errno );
		}
	}
	//        Stracker->TrackOut(1);
	mTmpSelectTimer = mSelectTimer;
	return 0;
}

////////////////////////////////////////////////////////////
/// blocked recv data on an UDP socket
////////////////////////////////////////////////////////////
int
CUDPSocket::Recv ( const char * msg, const size_t len )
{
	// Stracker->TrackIn(1);
	int bytes ( 0 );
	// if no data packets yet received from a peer, save the sender address
	if ( !mConnectedFrom )
	{
		struct sockaddr_in from;
		socklen_t  addrLen ( sizeof ( from ) );
		// Stracker->TrackOut(1);
		bytes = recvfrom ( mSockFD, ( char * ) msg, len, 0, ( struct sockaddr * ) &from, &addrLen );
		// fetch the sender address
		// Stracker->TrackIn(2);
		if ( bytes > 0 )
		{
			mRemoteAddr.IPAddress ( ntohl ( from.sin_addr.s_addr ) );
			mRemoteAddr.Port ( ntohs ( from.sin_port ) );
			mConnectedFrom = true;
		}

	}
	else
	{
		// Stracker->TrackOut(1);
		bytes = recv ( mSockFD, ( char * ) msg, len, 0 );
		// Stracker->TrackIn(2);
	}

	if ( bytes > 0 )
	{
		// Stracker->TrackOut(2);
		return bytes;
	}
	else if ( bytes == 0 ) /* should never happen */
	{
          LOG_NOTICE << "zero returned on blocked recv call! This should never happen!" << endl;
		// Stracker->TrackOut(2);
		return -1;
	} else /* bytes == -1: might by an connection close, network failure */
	{
		switch ( errno )
		{
			case EINTR: // the recv call is interrupted by a signal
#ifdef DEBUG_OUTPUT

				LOG_LU_DEBUG << "the recv call within " << __PRETTY_FUNCTION__ << " has been interrputed"
				<< endl;
#endif

				// Stracker->TrackOut(2);
				return 0;
			case EAGAIN: // the recv call has timed out
#ifdef DEBUG_OUTPUT

				LOG_LU_DEBUG << "the recv call within " << __PRETTY_FUNCTION__ << " has been timed out"
				<< endl;
#endif

				// Stracker->TrackOut(2);
				return 0;
		}
		// all other errors means an brocken socket communication. We can't receive
		// data any more. Throw an exception
		// Stracker->TrackOut(2);
		throw CNetwork_exception ( "can't receive data on socket. Error is: "
		                           + Cerrno[errno].m_name + " " + Cerrno[errno].m_desc,
		                           2, errno );

	}
	/* We never should arrive this return */
	// Stracker->TrackOut(2);
	return -1;
}

////////////////////////////////////////////////////////////
/// recv data on an UDP socket
////////////////////////////////////////////////////////////
int
CUDPSocket::TRecv ( const char * msg, const size_t len,
                    HPTimer::HighPerTimer & pTimer )
{
	//        Stracker->TrackIn(1);
	int bytes ( 0 );
	// initialize file descriptoer set
	//fd_set FdSet;
	// FD_ZERO(&FdSet);
	FD_SET ( mSockFD, &mFdSet );
	pTimer.SetTV ( mTmpSelectTimer );

	if ( ( select ( mSockFD+1, &mFdSet, NULL, NULL, &mTmpSelectTimer ) ) >0 &&
	        FD_ISSET ( mSockFD, &mFdSet ) )
	{
		// if no data packets yet received from a peer, save the sender address
		if ( ! mConnectedFrom )
		{
			struct sockaddr_in from;
			socklen_t  addrLen ( sizeof ( from ) );
			//                        Stracker->TrackOut(1);
			bytes = recvfrom ( mSockFD, ( char * ) msg, len, 0, ( struct sockaddr * ) &from, &addrLen );
			// fetch the sender address
			//                        Stracker->TrackIn(2);
			if ( bytes > 0 )
			{
				mRemoteAddr.IPAddress ( ntohl ( from.sin_addr.s_addr ) );
				mRemoteAddr.Port ( ntohs ( from.sin_port ) );
				mConnectedFrom = true;
			}

		}
		else
		{
			bytes = recv ( mSockFD, ( char * ) msg, len, 0 );
		}

		if ( bytes > 0 )
		{
			return bytes;
		}
		else if ( bytes == 0 ) /* should never happen */
		{
                  LOG_NOTICE << "zero returned on blocked recv call! This should never happen!" << endl;
			//                Stracker->TrackOut(2);
			return -1;
		} else /* bytes == -1: might by an connection close, network failure */
		{
			switch ( errno )
			{
				case EINTR: // the recv call is interrupted by a signal
#ifdef DEBUG_OUTPUT

					LOG_LU_DEBUG << "the recv call within " << __PRETTY_FUNCTION__ << " has been interrputed"
					<< endl;
#endif

					return 0;
				case EAGAIN: // the recv call has timed out
#ifdef DEBUG_OUTPUT

					LOG_LU_DEBUG << "the recv call within " << __PRETTY_FUNCTION__ << " has been timed out"
					<< endl;
#endif

					return 0;
			}

			throw CNetwork_exception ( "can't receive data on socket. Error is: "
			                           + Cerrno[errno].m_name + " " + Cerrno[errno].m_desc,
			                           2, errno );
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////
/// blocked recv data on an UDP socket
////////////////////////////////////////////////////////////
int
CUDPSocket::RecvV ( iovec * iov, const int vLen )
{
	// Stracker->TrackIn(1);
	int bytes ( 0 );
	mRecvMsg.msg_iov = iov;
	mRecvMsg.msg_iovlen = vLen;

	// if no data packets yet received from a peer, save the sender address
	bytes = recvmsg ( mSockFD, &mRecvMsg, MSG_WAITALL );
	if ( !mConnectedFrom && NULL != mRecvMsg.msg_name )
	{
		// fetch the sender address
		if ( bytes > 0 )
		{ /* data packet received. Fill the remote address object from the message header*/
			sockaddr_in * sin_ptr = ( sockaddr_in * ) ( mRecvMsg.msg_name );
			mRemoteAddr.IPAddress ( ntohl ( sin_ptr->sin_addr.s_addr ) );
			mRemoteAddr.Port ( ntohs ( sin_ptr->sin_port ) );
			mConnectedFrom = true;
			mRecvMsg.msg_name = NULL;
			mRecvMsg.msg_namelen = 0;
		}

	}

	// error: at least one datagram has been truncated!
	if ( bytes > 0 && ( mRecvMsg.msg_flags & MSG_TRUNC ) )
	{
		throw CNetwork_exception ( "Read message has been truncated! Data are lost!: "
		                           + Cerrno[errno].m_name + " " + Cerrno[errno].m_desc,
		                           3, errno );
	}

	if ( bytes > 0 )
	{
		return bytes;
	}
	else if ( bytes == 0 ) /* should never happen */
	{
          LOG_NOTICE << "zero returned on blocked recv call! This should never happen!" << endl;
		// Stracker->TrackOut(2);
		return -1;
	} else /* bytes == -1: might by an connection close, network failure */
	{
		switch ( errno )
		{
			case EINTR: // the recv call is interrupted by a signal
#ifdef DEBUG_OUTPUT

				LOG_LU_DEBUG << "the recv call within " << __PRETTY_FUNCTION__ << " has been interrputed"
				<< endl;
#endif

				return 0;
			case EAGAIN: // the recv call has timed out
#ifdef DEBUG_OUTPUT

				LOG_LU_DEBUG << "the recv call within " << __PRETTY_FUNCTION__ << " has been timed out"
				<< endl;
#endif

				return 0;
		}
		// all other errors means an brocken socket communication. We can't receive
		// data any more. Throw an exception
		throw CNetwork_exception ( "can't receive data on socket. Error is: "
		                           + Cerrno[errno].m_name + " " + Cerrno[errno].m_desc,
		                           2, errno );

	}
	/* We never should arrive this return */
	return -1;
}


// the SDP socket implementation
#ifdef SDP_ENABLED

/// ctor: get a SDP socket and bind to first free port
CSDPSocket::CSDPSocket ( bool pListen, unsigned char pTOS ) :
		CIPSocket ( SOCK_STREAM, S_SDP, pListen, pTOS )
{
	// struct protoent* proto = getprotobyname("TCP");
	// mProto = proto->p_proto;
	if ( pTOS != 0 )
	{
          LOG_LU_NOTICE << "TOS not supported by SDP socket interface!" << endl;
	}
}

/// ctor: get a TCP socket and bind to the given Address and port
CSDPSocket::CSDPSocket ( bool pListen,
                         unsigned short pPort,
                         unsigned int pIPAddress,
                         unsigned char pTOS ) :
		CIPSocket ( SOCK_STREAM, S_SDP, pPort, pIPAddress, pListen, pTOS )
{
	// struct protoent* proto = getprotobyname("TCP");
	// mProto = proto->p_proto;
	if ( pTOS != 0 )
	{
          LOG_LU_NOTICE << "TOS not supported by SDP socket interface!" << endl;
	}
}

///dtor:
CSDPSocket::~CSDPSocket()
{}

/// listen on the port address
bool
CSDPSocket::Listen()
{
	if ( listen ( mSockFD, 1 ) != -1 )
		return true;
	return false;
}

/// accept an incoming connection
CIPSocket *
CSDPSocket::Accept ( bool pSingleton )
/**** implement new accept interface */
{
	struct sockaddr_in sin;
	memset ( ( char * ) &sin, 0, sizeof ( sin ) );
	socklen_t sin_size ( sizeof ( sin ) );
	int newSock ( -1 );

	newSock = accept ( mSockFD, ( sockaddr * ) &sin, &sin_size );

	if ( -1 == newSock )
	{
		switch ( errno )
		{
			case EINTR: // the recv call is interrupted by a signal
#ifdef DEBUG_OUTPUT

				LOG_LU_DEBUG << "the recv call within " << __PRETTY_FUNCTION__
				<< " has been interrputed"      << endl;
#endif

				return NULL;

			case ECONNABORTED: // the recv call is interrupted by a signal
#ifdef DEBUG_OUTPUT

				LOG_LU_DEBUG << "the recv call within " << __PRETTY_FUNCTION__
				<< " has been aborted"                 << endl;
#endif

				return NULL;
			case EAGAIN:
#ifdef DEBUG_OUTPUT

				LOG_LU_DEBUG << "the recv call within " << __PRETTY_FUNCTION__
				<< " has been timed out"        << endl;
#endif

				return NULL;

		}
		throw CNetwork_exception ( "Error on syscall accept() on socket with fd " +
		                           CStringUtils::itos ( mSockFD ) + ". Error is: " +
		                           Cerrno[errno].m_name + " " + Cerrno[errno].m_desc + ".",
		                           3, errno );
	} // end of newSock == -1

	CIPSocket * connSock_ptr; // the connected socket
	if ( pSingleton )
	{
		close ( mSockFD );
		mSockFD = newSock;
		connSock_ptr = this;
	}
	else
	{ // create a new socket, clone this to that socket and set the
		// connected state related data
		// TODO Since this pattern is common to all types of sockets, it cries for a
		// template. At one later point in time we will really implement this as a template
		connSock_ptr = new CSDPSocket ( *this );

	}
	// store the new filedescriptor
	mSockFD = newSock;

	//  connSock_ptr->mBindAddr = ntohl(sin.sin_addr.s_addr);
	connSock_ptr->mRemoteAddr.IPAddress ( ntohl ( sin.sin_addr.s_addr ) );
	connSock_ptr->mRemoteAddr.Port ( ntohs ( sin.sin_port ) );
	connSock_ptr->mConnectedFrom = true;
	return connSock_ptr;
}

////////////////////////////////////////////////////////////
/// recv data on an socket using select.
/// The select timeout must be set before with SetRecvTimeout()
/// @return amount of bytes, read by the call, 0 on time out and -1 on read on a
/// connection, closed on the remote side
////////////////////////////////////////////////////////////

int
CSDPSocket::SRecv ( const char * msg, const size_t len )
{
	ssize_t bytes ( 0 );
	FD_SET ( mSockFD, &mFdSet );

	if ( ( select ( mSockFD+1, &mFdSet, NULL, NULL, &mTmpSelectTimer ) ) >0
	        && FD_ISSET ( mSockFD, &mFDSet ) )
	{
		if ( ( bytes = recv ( mSockFD, ( char * ) msg, len, 0 ) ) > 0 )
		{
			mTmpSelectTimer = mSelectTimer;
			return bytes;
		}
		else if ( bytes == 0 )
		{ // remote side close
			return -1;
		}
		else
		{
			mTmpSelectTimer = mSelectTimer;
			throw CNetwork_exception ( "can't receive data on socket. Error is: "
			                           + Cerrno[errno].m_name + " " + Cerrno[errno].m_desc,
			                           2, errno );
		}
	}
	mTmpSelectTimer = mSelectTimer;
	return 0;
}

//////////////////////////////////////////////////////////////////////////
/// blocking recv data on an socket
/// returns the amount of bytes read or zero if select timed out. Returns -1, if the connection is closed
/// by the remote side
///////////////////////////////////////////////////////////////////////////
int
CSDPSocket::Recv ( const char * msg, const size_t len )
{
	ssize_t bytes ( 0 );

	if ( ( bytes = recv ( mSockFD, ( char * ) msg, len, 0 ) ) > 0 )
	{
		return bytes;
	}
	else if ( bytes == 0 ) /* remote side closed */
	{
		return -1;
	} else /* bytes == -1: might by an connection close, network failure */
	{
		switch ( errno )
		{
			case EINTR: // the recv call is interrupted by a signal
#ifdef DEBUG_OUTPUT

				LOG_LU_DEBUG << "the recv call within " << __PRETTY_FUNCTION__ << " has been interrputed"
				<< endl;
#endif

				return 0;
			case EWOULDBLOCK:
#ifdef DEBUG_OUTPUT

				LOG_LU_DEBUG << "the recv call within " << __PRETTY_FUNCTION__ << " has been timed out"
				<< endl;
#endif

				return 0;
		}
		// all other errors means an brocken socket communication. We can't receive
		// data any more. Throw an exception
		throw CNetwork_exception ( "can't receive data on socket. Error is: "
		                           + Cerrno[errno].m_name + " " + Cerrno[errno].m_desc,
		                           2, errno );

	}

	return -1; /* should never be reached */

}

/// recv data on an socket, using given timer

int
CSDPSocket::TRecv ( const char * msg, const size_t len,
                    HPTimer::HighPerTimer & pTimer )
{
	ssize_t bytes ( 0 );
	FD_SET ( mSockFD, &mFdSet );
	pTimer.setTV ( mTmpSelectTimer );

	if ( ( select ( mSockFD+1, &mFdSet, NULL, NULL, &mTmpSelectTimer ) ) >0
	        && FD_ISSET ( mSockFD, &mFdSet ) )
	{
		if ( ( bytes = recv ( mSockFD, ( char * ) msg, len, 0 ) ) > 0 )
		{
			pTimer.setTV ( mTmpSelectTimer );
			return bytes;
		}
		else if ( bytes == 0 )
		{ // remote side clased */
			return -1;
		}
	}
	else
	{
		pTimer.setTV ( mTmpSelectTimer );
		throw CNetwork_exception ( "can't receive data on socket. Error is: "
		                           + Cerrno[errno].m_name + " " + Cerrno[errno].m_desc,
		                           2, errno );
	}
}
pTimer.setTV ( mTmpSelectTimer );
return 0;
}



#endif // SDP_ENABLED


/// ctor: just dummies yet. To be real implemented
CFastTCPSocket::CFastTCPSocket ( bool pListen, unsigned char pTOS ) :
		CUDPSocket ( pListen, pTOS )
{}

/// ctor: get an UDP socket and bind to the given Address and port
CFastTCPSocket::CFastTCPSocket ( bool pListen,
                                 unsigned short pPort,
                                 unsigned int pIPAddress,
                                 unsigned char pTOS ) :
		CUDPSocket ( pListen, pPort, pIPAddress, pTOS )
{}

///dtor:
CFastTCPSocket::~CFastTCPSocket()
{}



/// ctor: just dummies yet. To be real implemented
CHSTCPSocket::CHSTCPSocket ( bool pListen, unsigned char pTOS ) :
		CUDPSocket ( pListen, pTOS )
{}


/// ctor: get an UDP socket and bind to the given Address and port
CHSTCPSocket::CHSTCPSocket ( bool pListen,
                             unsigned short pPort,
                             unsigned int pIPAddress,
                             unsigned char pTOS ) :
		CUDPSocket ( pListen, pPort, pIPAddress, pTOS )
{}

///dtor:
CHSTCPSocket::~CHSTCPSocket()
{}



/// convert string representation to an 32 bit ip v.4 Address
unsigned int
CIPAddress::htoa ( string & pHost )
{
	if ( CStringUtils::allDigits ( pHost ) )
		return ( unsigned int ) CStringUtils::stoi ( pHost );
	struct hostent * HostEnt;
	HostEnt = gethostbyname ( pHost.c_str() );
	if ( HostEnt != 0 )
	{
		unsigned int Addr;
		memcpy ( &Addr, HostEnt->h_addr_list[0], HostEnt->h_length );
		if ( HostEnt->h_length != 4 )
			return 0;
		return ntohl ( Addr );
	}
	return INADDR_ANY;
}

extern "C"  int h_errno;

/// convert ip v.4 Address to string representation of an address
string
CIPAddress::atoh ( const unsigned int pAddr, bool pHostOrder )
{
	unsigned long Addr = pHostOrder ? htonl ( pAddr ) : pAddr;
	string HostName ( "" );

	if ( pAddr == INADDR_ANY )
	{
		HostName = "0.0.0.0";
		return HostName;
	}

	struct hostent * HostEnt ( 0 );
	HostEnt = gethostbyaddr ( ( const char * ) &Addr, sizeof ( Addr ), AF_INET );
	if ( HostEnt != 0 )
	{
		HostName = ( string ) HostEnt->h_name;
		HostEnt = 0;
	}
	else
	{
          LOG_WARNING << "address resolution error " << endl;
		throw CNetwork_exception ( "Name lookup error. IP Address: " + atodd ( pAddr ), 7 );
	}
	return HostName;
}

/// convert ip v.4 Address to dotted decimal representation of an address
string
CIPAddress::atodd ( const unsigned int pAddr, bool pHostOrder )
{
	unsigned int Addr = pHostOrder ? pAddr : ntohl ( pAddr );
	string sAddr;
	unsigned char Byte;
	for ( int i ( 24 ); i>=0; i -= 8 )
	{
		Byte = ( ( Addr >> i ) & 0xff );
		sAddr.append ( CStringUtils::itos ( Byte ) );
		if ( i > 0 )
			sAddr.append ( "." );

	}
	return sAddr;
}

///
CIPAddress::CIPAddress() :
		mIPAddress ( 0 ),
		mPort ( 0 ),
		mHost ( "0.0.0.0" )
{}

///
CIPAddress::CIPAddress ( const std::string pAddr )
{

#ifdef DEBUG_OUTPUT
	LOG_LU_ERROR << __PRETTY_FUNCTION__ << " pAddr: " << pAddr << endl;
#endif

	CStringTokenizer tok ( pAddr, ":", false, false );
	int tokens = tok.countTokens();
	if ( tokens != 2 )
	{
		throw CNetwork_exception ( "IP Address format: " + pAddr +
		                           " HostName:Port expected" );
	}

	string Str ( tok.nextToken() );
	if ( !Str.length() )
	{
		Str = "0.0.0.0";
	}

	mIPAddress = htoa ( Str );
	mHost = Str;
	mPort = CStringUtils::stoi ( tok.nextToken() );

}

///
CIPAddress::CIPAddress ( const unsigned int pAddr, const unsigned short pPort ) :
		mIPAddress ( pAddr ),
		mPort ( pPort )
{
	try

	{
		mHost = CIPAddress::atoh ( pAddr );
	}
	catch ( CNetwork_exception & ex )
	{
          LOG_WARNING << ex.what() << " Code: " << ex.code() << endl;
		mHost = "";
	}
}

///
CIPAddress::CIPAddress ( const unsigned short pPort ) :
		mIPAddress ( 0 ),
		mPort ( pPort ),
		mHost ( "0.0.0.0" )
{}


/// accessor
unsigned int
CIPAddress::IPAddress() const
{
	return mIPAddress;
}

/// accessor
void
CIPAddress::IPAddress ( const unsigned int pAddr )
{
	mIPAddress = pAddr;

	try
	{
		mHost = CIPAddress::atoh ( pAddr );
	}
	catch ( const CNetwork_exception & ex )
	{
          LOG_WARNING << ex.what() << " Code: " << ex.code() << endl;
		mHost = "";
	}
}

/// accessor
unsigned short
CIPAddress::Port() const
{
	return mPort;
}

/// accessor
void
CIPAddress::Port ( const unsigned short Port )
{
	mPort = Port;
}

/// accessor
string
CIPAddress::Hostname() const
{
	return mHost;
}

/// accessor
void
CIPAddress::Hostname ( const string pHostname )
{
	mHost = pHostname;
	string Str ( pHostname );
	mIPAddress = CIPAddress::htoa ( Str );
}

bool
CIPAddress::HasWildcard()
{
	return ( mIPAddress == INADDR_ANY || mPort == 0 );
}

/// accessor
GsoapAddress::GsoapAddress() :
		CIPAddress()
{}

/// Corba addrss class
GsoapAddress::GsoapAddress ( const string pAddr )
{
	CStringTokenizer tok ( pAddr, ":", false, false );
	int tokens = tok.countTokens();
	if ( tokens == 3 )
	{
		if ( tok.nextToken() != string ( "inet" ) )
		{
			throw CNetwork_exception ( "wrong GSOAP URL format: " + pAddr +
			                           " inet:HostName:Port expected" );
		}
	}
	else if ( tokens != 2 )
	{
		throw CNetwork_exception ( "too few or too many tokens: " + pAddr +
		                           ":  Hostname:Port expected" );
	}
	string Str ( tok.nextToken() );
	if ( !Str.length() )
	{
		Str = "0.0.0.0";
	}
	mIPAddress = htoa ( Str );
	mHost = Str;

	mPort = CStringUtils::stoi ( tok.nextToken() );
	if ( !mPort )
	{
		throw CNetwork_exception ( "Gsoap port address can't be empty or zero" );
	}
}

/// ctor
GsoapAddress::GsoapAddress ( const unsigned int pAddr, const unsigned short pPort ) :
              CIPAddress ( pAddr, pPort )
{}

/// ctor
GsoapAddress::GsoapAddress ( const unsigned short pPort ) :
              CIPAddress ( pPort )
{}

/// accessor
string
GsoapAddress::GsoapURL() const
{
	return mHost + ":" + CStringUtils::itos ( mPort );
}


// E O F
