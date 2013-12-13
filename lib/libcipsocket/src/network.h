// Author:  Author Eduard Siemens <siemens@rvs.uni-hannover.de>
//
// File:    network.h
//          Fri May 16 2003
//
// $Header:   //hanvsfile02/prog/Projects/2005/hipernet/Repository/hipernet/archives/hipernet/Linux/Apps/ltest/network.h-arc   1.8.1.11   10 Apr 2007 17:45:46   siemense  $
// $Author:   siemense  $
// $Date:   10 Apr 2007 17:45:46  $
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
// $Log:   //hanvsfile02/prog/Projects/2005/hipernet/Repository/hipernet/archives/hipernet/Linux/Apps/ltest/network.h-arc  $
//
//   Rev 1.8.1.11   10 Apr 2007 17:45:46   siemense
//performance monitor interface added
//
//   Rev 1.8.1.10   26 Mar 2007 16:04:50   siemense
//additional CLDUTSocket constructor added
//
//   Rev 1.8.1.9   22 Mar 2007 17:28:28   siemense
//buuuug fixed
//
//   Rev 1.8.1.8   22 Mar 2007 17:14:28   siemense
//const conversion bug fixed
//
//   Rev 1.8.1.7   15 Mar 2007 17:01:48   siemense
//New UDP::Accept interface addeed
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
//   Rev 1.6   26 Jan 2006 10:06:02   siemense
//Intermediate checkin
//
//   Rev 1.5   21 Dec 2005 16:42:30   siemense
//Time correction between data sender and data recei ver added
//
//   Rev 1.4   15 Nov 2005 15:10:40   siemense
//zero duration and packet count bug fixed
//
// *
// *    Rev 1.3   14 Nov 2005 16:32:14   siemense
// * Data range of rate extended to long long. Minor bug fixes.
// *
// *    Rev 1.2   07 Oct 2005 09:11:28   siemense
// * unnecessary files removed
// *
// *    Rev 1.1   06 Oct 2005 17:06:58   siemense
// * data rate limit extended
// *
// *    Rev 1.0   28 Sep 2005 16:40:40   siemense
// * Initial revision.
// Revision 2.6  2005/07/19 15:46:19  siemens
// Final release from Eduard Siemens at the RVS
//
// Revision 2.5  2005/02/23 09:16:08  siemens
// allDigits bug fixed
//
// Revision 2.4  2004/07/02 21:20:28  siemens
// TCP connection closing bug fixed
//
// Revision 2.3  2004/05/25 14:08:29  siemens
// BSD license copyright notice added
//
// Revision 2.2  2003/12/30 10:26:27  siemens
// rewritten config staff and command line parsing
//
// Revision 2.1  2003/09/04 13:30:16  siemens
// priority scheduling tuned
//
// Revision 2.0  2003/06/30 07:06:13  siemens
// beta release
//
// Revision 1.3  2003/06/04 14:47:04  siemens
// *** empty log message ***
//
// Revision 1.2  2003/05/27 11:47:02  siemens
// last checkin before alpha release
//


#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string>
#include <ostream>
#include <sstream>
#include <map>
#include "HighPerTimer.h"
//#include "udt.h"
#ifdef SDP_ENABLED
#include <linux/sdp_inet.h>
#endif //SDP_ENABLED


typedef std::map <std::string, unsigned int> SockOptsMap;

enum SocketProto_t {
    S_TCP = 0,
    S_UDP,
    S_SDP,
    S_JET,
    S_FastTCP,
    S_HSTCP
};

/// helper class to convert hostnames to ipv4 addresses
class CIPAddress
{
	public:
		/// convert from FQHN or dotted decimal to ip address in host order
		static unsigned int htoa ( std::string &pHost );
		/** convert from ip v.4 address to FQDN
		    @param pHostOrder represents
		    the host rsepective network byte order
		*/
		static std::string atoh ( const unsigned int pAddr, bool pHostOrder=true );
		/** convert from ip v.4 address to dotted decimal notation,
		    @param pHostOrder represents the host respective network byte order
		*/
		static std::string atodd ( const unsigned int pAddr, bool pHostOrder=true );
		/// standard ctor
		CIPAddress();
		CIPAddress ( const std::string pAddr );
		/// IP in host byte order and port number expected
		CIPAddress ( const unsigned int pAddr, const unsigned short pPort );
		/// bind to specified port on any interfaces
		CIPAddress ( const unsigned short pPort );
		/// @returns the IPAddress in host byte order
		/// set the IP address and hostname of the object
		unsigned int IPAddress() const;
		void IPAddress ( const unsigned int pAddr );
		unsigned short Port() const;
		/// set the Port address of the object
		void Port ( unsigned short Port );
		std::string Hostname() const;
		/// set the Hostname and IP address of the object
		void Hostname ( const std::string pHostname );
		/// @returns true, if IP address is a wildcard address or
		/// port number is zero
		bool HasWildcard();

		friend std::ostream & operator<< ( std::ostream & pPstr, const CIPAddress & Addr );
	protected:
		unsigned int mIPAddress; // in host byte order
		unsigned short mPort;
		std::string mHost;
};


inline std::ostream & operator<< ( std::ostream & pOstr, const CIPAddress & Addr )
{
	if ( Addr.mHost.length()  == 0 )
	{
		pOstr << CIPAddress::atodd ( Addr.mIPAddress );
	}
	else
	{
		pOstr << Addr.mHost;
	}

	pOstr << ":" <<  Addr.mPort;
	return pOstr;
}


/// The high level interface to IP Sockets.
/// Sould be instantiated only via derived class
class CIPSocket
{
	public:
		/** ctor, for the interface only, no BSD socket will be initialized.
		* all the variables are initialized by the specific protocol implementation
		* i.e. @a CTCPSocket() or @a CJETSocket. This constructor also be used
		* for a pre-initialization of @a CIPSocket objects, i.e. in vectors or arrays.
		* This is the only public constructor because real communication sockets
		* are created by successor classes
		*/
		CIPSocket();
		/**
		  dtor. If the socket is already active, the socket will be closed.
		 */
		virtual ~CIPSocket();


		/**
		 * @return the  IP address, the socket is bound to
		 */
		unsigned long BindAddr(); /* TODO to be changed to BindAddr() */
		/**
		 * @return the port number, the socket is bound to
		 */
		unsigned short BindPort();

		/** get peer's IP address. On the client side this is the address, the client has
		* to be connected to or already connected to. On the server side it is the address of
		* the remote client, if conneted, a wildcard address (INADDR_ANY:0) otherwies
		*/
		CIPAddress RemoteAddr();

		/**
		    get the IP address of the origin of already received data packets -
		    in case of TCP it is the address of the connected peer
		    in case of UDP it is the first data packet received on this socket
		*/
		/* CIPAddress OriginAddr(); */



		/**
		   returns the socket descriptor
		*/
		inline int FD()
		{
			return mSockFD;
		}
		/// accessor to the mLocalAddr attrbute
		unsigned long LocalAddr()
		{
			return mLocalAddr;
		};
		/// accessor to the mLocalPort attrbute
		unsigned short LocalPort()
		{
			return mLocalPort;
		};
		/**
		   accessor to the connected flag - true, if connection to remote peer is
		   already established
		*/
		inline bool Connected()
		{
			return mConnected;
		}
		/**
		accessor to the ConnectedFrom flag - true, if a remote side has been connected
		*/
		inline bool ConnectedFrom()
		{
			return mConnectedFrom;
		}



		/// close the socket connection
		virtual void Close();

		/** connect to the server. Return value is true if successfully connected,
		    false otherwise
		 */
		virtual bool Connect ( const unsigned int pServer, const unsigned short pPort );
		/** connect to the server. Return value is true if successfully connected,
		    false otherwise
		 */
		virtual bool Connect ( CIPAddress & pAddress );
		/** set the connected from flag as well as the IP address of the connection originator
		*/
		virtual void SetConnFrom ( sockaddr_in & pSAddr );

		/// send a data packet
		virtual int Send ( const char * msg, const size_t len );

		/** Sends an amount of data packets blocking. The data are scattered into places,
		 * passed throogh as a iovec array.
		 * NOTE: SendV makes sence only with stream oriented protocols like TCP and SDP !!!
		 * @param vec is the pointer to the array of iovecs. @param vLen is the number of
		 * elements within the array. @return the amount of bytes really read from buffer,
		 * -1, if connection has been closed at the remote side.
		 */
		virtual int SendV ( iovec * vec, const int vLen );

		/** receives a data packet blocking. Normally this call blocks forever, if no data
		* are received.
		 * Hovever, with the method SetRecvTimeout, a timeout for the Recv call can be installed.
		 * This method set the SO_RCVTIMEO oftion of the socket
		 * @return the amount of bytes, if some data received, 0, if timed out,
		 * -1, if the remote side has closed the connection. Throws an
		 * CNetwork_exception, if an error has been occured
		 */
		virtual int Recv ( const char * msg, const size_t len ) = 0;

		/** receives a data packet blocking. Blocks until a data packet received. Hovewer,
		 * we use select instead of SO_RCVTIMEO, since the lattest option is not supported
		* on some systems
		 */
		virtual int SRecv ( const char * msg, const size_t len ) = 0;

		/** receives a data packet blocking using a timer. Waits maximum given
		 * time till packet received. Returns zero if timeout ocurred.
		 * This timer is select based one, because not all systems supoort  SO_RCVTIMEO option
		 * NOTE: with the default value the timeout is set to infinite.
		* Use SetRecvTimeout to set the timeout
		 */
		virtual int TRecv ( const char * msg, const size_t len,
		                    HPTimer::HighPerTimer & pTimer ) = 0;

		/** Receives an amount of  data packets blocking. The data are passed throogh as a iovec array.
		 * NOTE: RecvV makes sence only with stream oriented protocols like TCP and SDP !!!
		 * @param iov is the pointer to the array of iovecs. @param vLen is the number of
		 * elements within the array. @return the amount of bytes really read from buffer,
		 * -1, if connection has been closed at the remote side.
		     */
		virtual int RecvV ( iovec * iov, const int vLen ) = 0;
		/// accessor to the listening flag
		virtual bool Listen ( const int pBacklog = 10 ) = 0;
		/**
		   accept incoming connection.
		   in case of TCP, if pSingleton == true, don't accept any more incoming
		    connectoins
		* @return a pointer to an accepted socket object
		* In the case of a singletone listening socket, @em this pointer will be returned.
		* Hovewer the listenig soclket will be closed, mSockFD will point to the new connected
		* socket. On failure, @a NULL will be returned.
		 */
		virtual CIPSocket * Accept ( bool pSingleton=true ) = 0;
		/**
		 * set the receive   - the mSelectTimer as well as the SO_RECVTIMEO option of
		* the socket
		 */
		virtual bool SetRecvTimeout ( const HPTimer::HighPerTimer & pTimeout );
		/**
		* set the packet send timer using SO_SENDTIMEO option of the socket
		* could be set only after a connect because timeouted connect could be a problem
		*/
		virtual bool SetSendTimeout ( const HPTimer::HighPerTimer & pTimeout );

		/**
		 * Get the type of the socket object: S_TCP, S_UDP, S_JET, S_SDT supportet at this time
		 */
		inline SocketProto_t SockProto()
		{
			return   mProto;
		}

	protected:


		/** ctor, starts the socket
		* this ctor creates a socket that uses an ephemeral port
		* @param pType means the socket type: @a SOCK_STREAM or @a SOCK_DGRAM
		* @param pProto is the transport protocol
		* @param listen says if the socket should be a listening one (server side of the socket)
		* @param TOS is the type of service of the socket
		*/
		CIPSocket ( int pType, SocketProto_t pProto, bool listen, unsigned char TOS=0 );
		/** ctor, starts the socket
		* @param pType means the socket type: @a SOCK_STREAM or @a SOCK_DGRAM
		* @param pProto is the transport protocol
		* @param pPort is the port, socket shuld be started on. 0 means a port,
		* dynamicly assigned by the OS
		* @param pIPAddress means the IP address, the socket should listen or
		* send data from
		* @param listen says if the socket should be a listening one (server side of the socket).
		* If true, the socket will be taken into the listening state
		* @param TOS is the type of service of the socket
		*/
		CIPSocket ( int pType, SocketProto_t pProto, unsigned short pPort, unsigned int  pIPAddress,
		            bool listen, unsigned char TOS=0 );
		// member variables
		/// the socket file descriptor
		int mSockFD;

		/// Socket type. Possible types are SOCK_STREAM or SOCK_DGRAM
		int mSockType;
		/** protocol type. Currently TCP and UDP supported. For SDP (infiniband) support,
		* SDP_ENABLED  shouuld be defined
		*/
		SocketProto_t mProto;
		/// Address, the socket should be bound to, defaults, to INADDR_ANY
		unsigned long mBindAddr;
		/// Port, the socket should be bound to, defaults to 0
		unsigned short mBindPort;
		/// address and port of the connected peer or the peer to be connected
		CIPAddress mRemoteAddr;
		/// local address, assigned after creating a socket
		unsigned long  mLocalAddr;                   // go towards CIPAddress
		/** local port, used for the connection after
		connecting remote side
		*/
		unsigned short mLocalPort;
		/// is the  the socket descriptor really used ?
		bool mOpen;
		/**
		   connection status of the socket - true if connected to the remote host,
		   false otherwise, especially used for bidirectional udp connections
		*/
		bool mConnected;

		/**
		   the status of a incoming connection and the origin of the received data packet
		*/
		bool mConnectedFrom; // the socket is connected from a remote side (server side socket)
		// CIPAddress mOriginAddr; // the address of the remote side, connected to this socket
		/**
		   TOS field
		*/
		unsigned char mTOS;
		// the fdset for the socket selects
		fd_set mFdSet;
		// the select timer
		timeval mSelectTimer;
		/** the temporary selcet timer - this one will be really passed
		 * to the select call an can be changed from the posix select routine
		*/
		timeval mTmpSelectTimer;

		/** message header. contains space for ip address of the peer as well as space for
		 * the iovec and control staff
		 */
		char mInAddrBuff[128];                // space for the peer address
		msghdr mSendMsg, mRecvMsg;            // the message header
	private:
		// the common socket initialization routine
		void init_socket();

};


/// The interface to TCP Sockets.
class CTCPSocket: public CIPSocket
{
	public:
		/**
		   ctor: get an socket,
		   @param pListen is a flag about starting an listener,
		   defaults to no
		   @param pTOS means the value of TOS field
		*/
		CTCPSocket ( bool pListen=false, unsigned char pTOS=0 );
		/**
		   ctor: get an socket
		   @param pListen is a flag about starting an listener. Defaults to false
		   @param pPort means port to be used
		   @param pIPAddress, defaults to INADDR_ANY
		   @param pTOS means the value of TOS field
		*/
		CTCPSocket ( bool pListen, unsigned short pPort,
		             unsigned int pIPAddress = INADDR_ANY, unsigned char pTOS=0 );
		/// dtor
		virtual ~CTCPSocket();

		/** put the socket into the listen state.
		 * @param pBacklog gives the maximum amount of connection requests to be queued
		 * on a server socket
		 * @returns true if the listen call was successful, on error a
		 * @a exception Network_exception will be raised
		 */
		virtual bool Listen ( const int pBacklog = 10 );

		/**
		accept incoming connection.
		in case of TCP, if pSingleton == true, don't accept any more incoming
		connectoins
		* @return a pointer to an accepted socket object
		* In the case of a singletone listening socket, @em this pointer will be returned.
		* Hovewer the listenig soclket will be closed, mSockFD will point to the new connected
		* socket. On failure, @a NULL will be returned.
		*/
		virtual CIPSocket * Accept ( bool pSintleton=true );

		/** receives a data packet blocking. Normally this call blocks forever,
		* if no data are received.
		 * Hovever, with the method SetRecvTimeout, a timeout for the Recv call can be installed.
		 * This method set the SO_RCVTIMEO oftion of the socket
		 */
		virtual int Recv ( const char * msg, const size_t len );
		/** Receives an amount of  data packets blocking. The data are passed throogh as a iovec array.
		 * NOTE: RecvV makes sence only with stream oriented protocols like TCP and SDP !!!
		 * @param iov is the pointer to the array of iovecs. @param vLen is the number of
		 * elements within the array. @return the amount of bytes really read from buffer,
		 * -1, if connection has been closed at the remote side.
		 */
		virtual int RecvV ( iovec * iov, const int vLen );

		/** receives a data packet blocking. Blocks until a data packet received. Hovewer,
		 * we use select instead of SO_RCVTIMEO, since the lattest option is not supported on
		* some systems
		 */
		virtual int SRecv ( const char * msg, const size_t len );

		/** receives a data packet blocking using a timer. Waits maximum given
		 * time till packet received. Returns zero if timeout ocurred.
		 * This timer is select based one, because not all systems supoort  SO_RCVTIMEO option
		 * NOTE: with the default value the timeout is set to infinite. Use SetRecvTimeout to
		* set the timeout
		 */
		virtual int TRecv ( const char * msg, const size_t len,
		                    HPTimer::HighPerTimer & pTimer );

	private:
		HPTimer::HighPerTimer mReadTimer;

};

#ifdef SDP_ENABLED

/// The interface and ipmlementation of the SDP socket is essentially the same as with TCP
class CSDPSocket: public CIPSocket
{
	public:
		/**
		   ctor: get an socket,
		   @param pListen is a flag about starting an listener,
		   defaults to no
		   TOS is not supported by SDP
		*/
		CSDPSocket ( bool pListen=false, unsigned char pTOS=0 );
		/**
		   ctor: get an socket
		   @param pListen is a flag about starting an listener. Defaults to false
		   @param pPort means port to be used
		   @param pIPAddress, defaults to INADDR_ANY
		   @param pTOS means the value of TOS field
		*/
		CSDPSocket ( bool pListen, unsigned short pPort,
		             unsigned int pIPAddress = INADDR_ANY, unsigned char pTOS=0 );
		/// dtor
		virtual ~CSDPSocket();
		/// start listener
		virtual bool Listen ( const int pBacklog = 10 );
		/**
		accept incoming connection.
		in case of TCP, if pSingleton == true, don't accept any more incoming
		connectoins
		* @return a pointer to an accepted socket object
		* In the case of a singletone listening socket, @em this pointer will be returned.
		* Hovewer the listenig soclket will be closed, mSockFD will point to the new connected
		* socket. On failure, @a NULL will be returned.
		*/
		virtual CIPSocket * Accept ( bool pSintleton=true );
		/** receives a data packet blocking. Normally this call blocks forever,
		* if no data are received.
		* Hovever, with the method SetRecvTimeout, a timeout for the Recv call
		* can be installed.
		* This method set the SO_RCVTIMEO oftion of the socket
		*/
		virtual int Recv ( const char * msg, const size_t len );

		/** receives a data packet blocking. Blocks until a data packet received. Hovewer,
		 * we use select instead of SO_RCVTIMEO, since the lattest option is not supported
		* on some systems
		 */
		virtual int SRecv ( const char * msg, const size_t len );

		/** receives a data packet blocking using a timer. Waits maximum given
		 * time till packet received. Returns zero if timeout ocurred.
		 * This timer is select based one, because not all systems supoort  SO_RCVTIMEO option
		 * NOTE: with the default value the timeout is set to infinite.
		* Use SetRecvTimeout to set the timeout
		 */
		virtual int TRecv ( const char * msg, const size_t len,
		                    HPTimer::HighPerTimer & pTimer );
};

#endif //DSP_ENABLED

class CUDPSocket: public CIPSocket
{
	public:
		/** ctor: get an socket,
		    @param pListen is a flag about starting an listener,
		    defaults to no
		*/
		CUDPSocket ( bool pListen=false, unsigned char pTOS=0 );
		/** ctor: get an socket on a specified port
		    @param listen is a flag about starting an listener,
		    defaults to no
		*/
		CUDPSocket ( bool listen, unsigned short pPort,
		             unsigned int pIPAddress = INADDR_ANY, unsigned char pbTOS=0 );
		/// dtor
		virtual ~CUDPSocket();
		/// start listener, only for interface compatibility with tcp socket
		virtual bool Listen ( const int pBacklog = 10 );
		/**
		 * accept incoming connection.
		 * in case of TCP, if pSingleton == true, don't accept any more
		 * incoming connectoins
		* @return a pointer to an accepted socket object
		* In the case of a singletone listening socket, @em this pointer will be returned.
		* Hovewer the listenig soclket will be closed, mSockFD will point to the new connected
		* socket. On failure, @a NULL will be returned.
		*/
		virtual CIPSocket * Accept ( bool pSingleton=true );
		/**
		* accept incoming connection.
		* set remote address of the socket to pRemoteAddr, so
		 * all datagrams will be sent to that address
		* @return a pointer to an accepted socket object
		    * In the case of a singletone listening socket, @em this pointer will be returned.
		    * Hovewer the listenig soclket will be closed, mSockFD will point to the new connected
				  * socket. On failure, @a NULL will be returned.
		*/
		virtual CIPSocket * Accept ( CIPAddress & pRemoteAddr, bool pSingleton=true );
		/** receives a data packet blocking.
		* Usually, this call blocks forever, if no data are received.
		 * Hovever, with the method SetRecvTimeout, a timeout for the Recv call can be installed.
		 * This method set the SO_RCVTIMEO oftion of the socket
		 */
		virtual int Recv ( const char * msg, const size_t len );

		/** receives a data packet blocking. Blocks until a data packet received. Hovewer,
		 * we use select instead of SO_RCVTIMEO, since the lattest option is not supported
		* on some systems.
		 */
		virtual int SRecv ( const char * msg, const size_t len );

		/** Receives an amount of  data packets blocking. The data are passed throogh as a iovec array.
		 * NOTE: RecvV makes sence only with stream oriented protocols like TCP and SDP !!!
		 * @param iov is the pointer to the array of iovecs. @param vLen is the number of
		 * elements within the array. @return the amount of bytes really read from buffer,
		 * -1, if connection has been closed at the remote side.
		 */
		virtual int RecvV ( iovec * iov, const int vLen );

		/** receives a data packet blocking using a timer. Waits maximum given
		 * time till packet received. Returns zero if timeout ocurred.
		 * This timer is select based one, because not all systems supoort  SO_RCVTIMEO option
		 * NOTE: with the default value the timeout is set to infinite.
		* Use SetRecvTimeout to set the timeout
		 */
		virtual int TRecv ( const char * msg, const size_t len,
		                    HPTimer::HighPerTimer & pTimer );

	private:
};

///////////////////////////////////////////////////////////////////////////
// FastTCP protocol implementation
///////////////////////////////////////////////////////////////////////////

class CFastTCPSocket: public CUDPSocket
{
	public:
		/** ctor: get an socket,
		    @param pListen is a flag about starting an listener,
		    defaults to no
		*/
		CFastTCPSocket ( bool pListen=false, unsigned char pTOS=0 );
		/** ctor: get an socket on a specified port
		    @param listen is a flag about starting an listener,
		    defaults to no
		*/
		CFastTCPSocket ( bool listen, unsigned short pPort,
		                 unsigned int pIPAddress = INADDR_ANY, unsigned char pTOS=0 );
		/// dtor
		virtual ~CFastTCPSocket();
};


///////////////////////////////////////////////////////////////////////////
// HSTCP protocol implementation
///////////////////////////////////////////////////////////////////////////

class CHSTCPSocket: public CUDPSocket
{
	public:
		/** ctor: get an socket,
		    @param pListen is a flag about starting an listener,
		    defaults to no
		*/
		CHSTCPSocket ( bool pListen=false, unsigned char pTOS=0 );
		/** ctor: get an socket on a specified port
		    @param listen is a flag about starting an listener,
		    defaults to no
		*/
		CHSTCPSocket ( bool listen, unsigned short pPort,
		               unsigned int pIPAddress = INADDR_ANY, unsigned char pTOS=0 );
		/// dtor
		virtual ~CHSTCPSocket();
};


/// CORBA addres container
class GsoapAddress: public CIPAddress
{
	public:
		///  standard ctor
		GsoapAddress();
		/// addr:port or gsoap URL expected
		GsoapAddress ( const std::string pAddr );
		/// IP in host byte order and port number expected
		GsoapAddress ( const unsigned int pAddr, const unsigned short pPort );
		/// bind to specified port on any interfaces
		GsoapAddress ( const unsigned short pPort );
		/// returns the GSOAP URL string
		std::string GsoapURL() const;
	private:

};

/// left shift operator for the CORBA address
inline std::ostream & operator<< ( std::ostream & pOstr, const GsoapAddress & Addr )
{
	pOstr << Addr.GsoapURL();
	return pOstr;
}




/// class for throwing errors in conjunction with network failures
class CNetwork_exception: public std::exception
{
	protected:
		/// exception message
		std::string emsg;
		/// error code
		int eval;
		/// the system error code
		int syserrno;
	public:
		///ctor
		CNetwork_exception() :
				emsg ( "" ),
				eval ( 0 ),
				syserrno ( 0 )
		{}
		/// ctor with predefined error message
		CNetwork_exception ( const std::string & _emsg ) :
				emsg ( _emsg ),
				eval ( 0 ),
				syserrno ( 0 )
		{}
		/// ctor with predefined error message and error value
		CNetwork_exception ( const std::string & _emsg, int _eval, int _sys_errno = 0 ) :
				emsg ( _emsg ),
				eval ( _eval ),
				syserrno ( _sys_errno )
		{}
		virtual const char * what() const throw ()
		{
			return emsg.c_str();
		}
		virtual int code() const
		{
			return eval;
		}
		virtual int sys_errno() const
		{
			return syserrno;
		}
		/// dtor
		virtual ~CNetwork_exception() throw ()
	{}}
;



#endif  //__NETWORK_H__
