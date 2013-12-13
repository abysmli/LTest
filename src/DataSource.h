// This may look like C code, but it is really -*- C++ -*-
//
//
// Author:  Author Eduard Siemens <siemens@rvs.uni-hannover.de>
//
// File:    DataSource.h
//          Thue Sep 17 2002
//
// $Id: DataSource.h,v 2.12 2005/07/19 15:46:19 siemens Exp $
// $Author:   siemense  $
// $Date:   31 Jan 2007 09:10:38  $
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
//
// $Log:   //hanvsfile02/prog/Projects/2005/hipernet/Repository/hipernet/archives/hipernet/Linux/Apps/ltest/DataSource.h-arc  $
//
//   Rev 1.7.1.5   31 Jan 2007 09:10:38   siemense
//Efficiency issues solved. Interfaces to network classes has been cleaned up.
//
//   Rev 1.7.1.4   21 Dec 2006 14:15:08   siemense
//CIPSocket extended by BRecv and TRecv (not yet testsed)
//
//   Rev 1.7.1.3   13 Nov 2006 11:57:22   siemense
//Minor bug fixes.
//Version before CIPNetworkPerformance tunings.
//
//   Rev 1.7.1.1   17 Aug 2006 13:21:08   siemense
//TSC timer class has been implemented and rudimentary tested
//
//   Rev 1.7.1.0   Jun 27 2006 10:39:52   siemense
//New Protocols Branch
//
//   Rev 1.7   16 Feb 2006 16:10:12   siemense
//FullSpeed source added. Minor bugs fixed.
//
// Rev 1.6   08 Feb 2006 13:25:40   siemense
// Infiniband support added
//
// Rev 1.5   21 Dec 2005 16:42:28   siemense
// Time correction between data sender and data recei ver added
//
// Rev 1.4   15 Nov 2005 15:10:40   siemense
// zero duration and packet count bug fixed
// *
// *    Rev 1.3   14 Nov 2005 16:32:12   siemense
// * Data range of rate extended to long long. Minor bug fixes.
// *
// *    Rev 1.2   07 Oct 2005 09:11:26   siemense
// * unnecessary files removed
// *
// *    Rev 1.1   06 Oct 2005 17:06:56   siemense
// * data rate limit extended
// *
// *    Rev 1.0   28 Sep 2005 16:40:34   siemense
// * Initial revision.
//
// Revision 2.12  2005/07/19 15:46:19  siemens
// Final release from Eduard Siemens at the RVS
//
// Revision 2.11  2005/02/23 09:16:08  siemens
// allDigits bug fixed
//
// Revision 2.10  2005/02/07 14:43:14  siemens
// bereinigung von fremden Code
//
// Revision 2.8  2004/07/01 19:48:09  siemens
// intermediate version, tcp performance bug is still there
//
// Revision 2.7  2004/06/28 19:20:08  siemens
// corbaRemoteAddress bug fixed, RingBuffer class for TCP reassembling added
//
// Revision 2.6  2004/05/25 14:08:29  siemens
// BSD license copyright notice added
//
// Revision 2.5  2004/05/13 08:57:55  siemens
// dump file zero times bug fixed
//
// Revision 2.4  2004/03/26 06:46:30  siemens
// TCP segment buffer overflow bug fixed
//
// Revision 2.3  2004/03/12 21:09:34  siemens
// *** empty log message ***
//
// Revision 2.2  2004/02/04 16:21:52  siemens
// *** empty log message ***
//
// Revision 2.1  2003/12/11 13:32:58  siemens
// Working version with remote source
//
// Revision 2.0  2003/06/30 07:06:13  siemens
// beta release
//
// Revision 1.5  2003/06/20 13:07:05  siemens
// *** empty log message ***
//
// Revision 1.4  2003/06/16 14:02:24  siemens
// *** empty log message ***
//
// Revision 1.3  2003/06/11 07:50:00  siemens
// Alfha release for ltest
//
// Revision 1.2  2003/06/06 15:09:59  siemens
// *** empty log message ***
//
// Revision 1.1  2003/06/04 15:00:39  siemens
// *** empty log message ***
//
//


#ifndef DATASOURCE_H
#define DATASOURCE_H
#include <string>
#include <fstream>
#include <vector>
#include <queue>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "HighPerTimer.h"
#include "soapltestService.h"
#include "network.h"
#include "RingBuffer.h"

/// constants
static const unsigned int MAX_IP_SIZE = 65536;
// the scaling factor for maximal LTest PDUs with tcp high performance source
static const unsigned int TCP_PERFORMANCE_SCALE = 256;

/// PDU header class
struct CPDUHeader
{
    /// format is:  Bytes 0-15: packet header like in PDU, 16-23 receiving
    /// timestamp: seconds and microseconds in host byte order
    char Header [24];

    /// standard ctor for manipulation in the vector structure
    CPDUHeader()
    {
        memset ( Header, 0, sizeof ( Header ) );
    }

    /// constructor with initializing data to header of the pointed packet
    CPDUHeader ( char * pPacket )
    {
        memset ( Header, 0, sizeof ( Header ) );
        memcpy ( Header, pPacket, 16 );
    }

    /// get the packet length
    unsigned long Length()
    {
        return ntohl ( * ( uint32_t * ) Header );
    }

    /// get the Sequence number
    unsigned long SeqNo()
    {
        return ntohl ( * ( uint32_t * ) ( Header + 4 ) );
    }

    /// return the Sending timestamp as CTSCTimer
    HPTimer::HighPerTimer SendTime()
    {
        HPTimer::HighPerTimer Time ( static_cast<int32_t> ( ntohl ( * ( uint32_t * ) ( Header + 8 ) ) ),
                                 static_cast<int32_t> ( ntohl ( * ( uint32_t * ) ( Header + 12 ) ) *1000 ),
                                 false );
        return Time;
    }

    /// return the receiving timestamp as CTSCTimer
    HPTimer::HighPerTimer RecvTime()
    {
        HPTimer::HighPerTimer Time ( static_cast<int32_t> ( ntohl ( * ( uint32_t * ) ( Header + 16 ) ) ),
                                 static_cast<int32_t> ( ntohl ( * ( uint32_t * ) ( Header + 20 ) ) *1000 ),
                                 false );
        return Time;
    }
private:

};




/// print out a Sender description
inline std::ostream & operator<< ( std::ostream & pOstr, const tv_type & tv )
{
    pOstr << tv.tv_sec << "." << tv.tv_usec;
    return pOstr;
}

inline std::ostream & operator<< ( std::ostream & pOstr, const senderDesc_t & pDesc )
{
    pOstr << "local address " << CIPAddress::atodd ( pDesc.LocalAddr ) << ":" << pDesc.LocalPort << "\n"
    <<  "remote address " << CIPAddress::atodd ( pDesc.RemoteAddr ) << ":" << pDesc.RemotePort << "\n"
    << "RemoteURL " << pDesc.RemoteURL << "\n"
    << "Bytes "     << pDesc.Bytes    << "\n"
    << "Packets "    << pDesc.Packets   << "\n"
    << "Proto "     << pDesc.Proto     << "\n"
    << "StartTime " << pDesc.StartTime << "\n"
    << "Duration "  << pDesc.Duration  << "\n"
    << "FinishTime " << pDesc.FinishTime << "\n"
    << "Priority "  << pDesc.Priority  << "\n"
    << "SType " << pDesc.SType     << "\n"
    << "BPS "       << pDesc.BPS       << "\n"
    << "Size "      << pDesc.Size      << "\n"
    << "FifoName "  << pDesc.FifoName  << std::endl;
    return pOstr;
}



/// received pdu header class
struct CRPDUHeader
{
    /** the format of the Header field is: Bytes 0-3 Sequence number, 4-7 receiving
     time, seconds, 8-11 receiving time, microseconds, all all in network byte
     order
    */
    char  Header[12];
private:
    uint32_t * header_ptr;
public:
    /// standard ctor for manipulation in the vector structure
    CRPDUHeader() :
            header_ptr ( reinterpret_cast<uint32_t *> ( Header ) )
    {
        memset ( Header, 0, sizeof ( Header ) );
    }
    /// constructor with initializing data to header of the pointed packet
    CRPDUHeader ( char * pPacket, timeval & pTime )
    {
        memcpy ( Header, pPacket, 4 );
        * ( int * ) ( & ( Header[4] ) ) = htonl ( pTime.tv_sec );
        * ( int * ) ( & ( Header[8] ) ) = htonl ( pTime.tv_usec );
    }

    /// dtor
    virtual ~CRPDUHeader()
    {}

    /// set all values of the RPDUHeader
    void  SetHeader ( char * pPacket, timeval & pTime )
    {
        header_ptr[0] = * ( reinterpret_cast<uint32_t *> ( pPacket ) );
        header_ptr[1] =  htonl ( pTime.tv_sec );
        header_ptr[2] = htonl ( pTime.tv_usec );
        return;
    }
 
    /// get the sequence number of the data packet
    unsigned long SeqNo()
    {
        return ntohl ( * ( int * ) ( Header ) );
    }
};


/// PDU_Desc entry for sending PDUs via a FIFO interface
struct PDU_Desc
{
    HPTimer::HighPerTimer SendT;
    unsigned int Length;
    PDU_Desc ( HPTimer::HighPerTimer & pTime, unsigned int pLength ) :
            SendT ( pTime ),
            Length ( pLength )
{}};


/// Abstract class for a generic data source
class CDataSource
{
    public:
        /// default ctor, only for the dummy Source
        CDataSource();
        CDataSource ( unsigned int pLocalAddr, unsigned pLocalPort,
                      unsigned int pRemoteAddr, unsigned pRemotePort,
                      unsigned int pBytes, unsigned int pPackets, proto_t pProto,
                      HPTimer::HighPerTimer & pStartTime, HPTimer::HighPerTimer & pDuration, HPTimer::HighPerTimer & pFinishTime,
                      int pPriority, unsigned char pTOS, SockOptsMap pOptsMap = SockOptsMap() );
        /// dtor
        virtual ~CDataSource();

        /// dispatch routine for the source
        virtual bool Dispatch() = 0;

        /// check, if the session is finished
        inline bool Finished()
        {
            return mFinished;
        }
        /// store received sample
        void StoreRSample ( char* pHeader );

        /// print the data source description
        virtual std::string PrintSDesc() = 0;
        /// get the packet sequence number of last sent packet
        unsigned int LastSeqNo()
        {
            return mSeqNo -1;
        }
        /// close the socket;
        virtual void CloseSocket();
        /// print out session statistics
        virtual std::string PrintStats();
        /// calculate and print basic statistics for the transmission
        virtual std::string PrintBasicStats();

        /// dump csv formatted samples dump to a file
        void DumpSamples ( std::ostream * pDStream );
        /** freeze the sample vector - especially, set mCur to the correct begin()
        iterator
        */
        void Freeze();

        /**
           set the time correction value (constant time offset between data sender and
           data receiver) and the position of delta | changed by Li, Yuan
        */
        void SetDeltaTime ( HPTimer::HighPerTimer & pDelta, HPTimer::HighPerTimer & pDelta_pos );

        /**
           set the time correction value at end of measurement (constant time offset between data sender and
           data receiver) and the position of delta | added by li, yuan
        */
        void SetDeltaTime_end ( HPTimer::HighPerTimer & pDelta, HPTimer::HighPerTimer & pDelta_pos );

        /// get next Data sender Header with information about the DataSource
        CPDUHeader * getNextHeader();
        /// returns basics statistics of the source
        BasicSourceStats_t getBasicStats();
        /// returns the pointer to the socket structure
	
	///IF
	HPTimer::HighPerTimer & getDeltaTime();
	std::vector <CPDUHeader> getHeader();
	///*IF

    protected:
        /// start time of a session
        HPTimer::HighPerTimer mStartTime;
        /// finish time of a session
        HPTimer::HighPerTimer mFinishTime;
        /// Duration of a session
        HPTimer::HighPerTimer mDuration;
        /// transport protocol of a session
        proto_t mProto;
        /// sequence number
        unsigned long      mSeqNo;
        /// flag for finished Session
        bool           mFinished;
        /// pointer to the IP socket
        CIPSocket *    mSocket;
        /// bytes to be transmitted
        unsigned int   mBytes;
        /// packets to be transmitted
        unsigned int   mPackets;
        /// timestamp of the previous packet
        HPTimer::HighPerTimer        mpTimestamp;
        /// priority base for sender threads
        unsigned int  mPriority;
        /// temporary variable, as member, because often used by different methods
        HPTimer::HighPerTimer mNow;
        timeval mNowTV;
        /// Time correction value
        HPTimer::HighPerTimer mDeltaTime;
        /// Time correction value at which send time position | added by Li, Yuan
        HPTimer::HighPerTimer mDeltaTime_pos;
        /// Time correction value at the end of measurement | added by li, yuan
        HPTimer::HighPerTimer mDeltaTime_end;
        /// Time correction value at which send time position | added by Li, Yuan
        HPTimer::HighPerTimer mDeltaTime_end_pos;
        char * mBuff;
        // char mBuff[MAX_IP_SIZE];

        /** the format for stored samples is exactly the same as stored in the header
         of PDU (in network byte order)
        */
        /// vector with source samples
        std::vector <CPDUHeader> mHeaderVec;
        /// iterator to the current position
        std::vector <CPDUHeader>::iterator mCur;

        /**
           assemble PDU in the mBuff buffer.
           Format of the pdu header is:
           LENGH SEQN TIMESTAMP
           LENGTH is the packet length as 4 bytes integer
           SEQN is the sequence number of the packet as 4 bytes unsigned long
           time stamp is a 4 Bytes integer for seconds and consequtive 4 Bytes
           integer for microseconds. All in network byte order (big endian)
        */
        void AssemblePDU ( const int pLen );

        // variables for statistics counts
        unsigned int mSentPacketStat;
        long long mSentBytesStat;
        unsigned int mRecvPacketStat;
        long long mRecvBytesStat;
        unsigned long mDuplicates; // packet duplications counter
        unsigned long mCorrupted;  // corrupted counter
        unsigned long mReordered; // reordering counter
        unsigned char mTOS;
    private:
        unsigned long mLastSeqNo; //highest seen sequence number
}
; // ----CDataSource----


/// the CBR Source
class CCBRSource: public CDataSource
{
    public:
        /// standard ctor, ctor without transmission parameters doesn't sence
        CCBRSource ( unsigned int pLocalAddr, unsigned short pLocalPort,
                     unsigned int pRemoteAddr, unsigned short pRemotePort,
                     unsigned int pBytes, unsigned int pPackets, proto_t pProto,
                     unsigned long long pRate, unsigned int pSize, HPTimer::HighPerTimer & pStartTime,
                     HPTimer::HighPerTimer & pDuration, HPTimer::HighPerTimer & pFinishT, int pPriority,
                     unsigned char pTOS,  SockOptsMap pOptsMap = SockOptsMap() );
        /// dtor
        virtual ~CCBRSource();
        /// the dispatch routine of the data source scheduler
        virtual bool Dispatch();
        /// print out source description
        virtual std::string PrintSDesc();
    private:

        unsigned long long mRate;
        unsigned int mSize;
        HPTimer::HighPerTimer mNextTime;
        /// time distance between two
        /// packets - for faster calculation
        long long  mInterPacketTics;
        /// save creation time of the packet in the vector structure
        /// calculate the next time for dispatching a PDU
        HPTimer::HighPerTimer & calcNextTime ( HPTimer::HighPerTimer & Now );

};


/** A data source like CBR tuned for maximum data throughput.
   Results shuold be comparable with iperf (using TCP).
   With TCP PDU sizes up to 1024x64 kBytes are permitted
*/
class CFullSpeedSource: public CDataSource
{
    public:
        // standard ctor. The rate parameter is not permitted (because of full speed)
        CFullSpeedSource ( unsigned int pLocalAddr, unsigned short pLocalPort,
                           unsigned int pRemoteAddr, unsigned short pRemotePort,
                           unsigned int pBytes, unsigned int pPackets, proto_t pProto,
                           unsigned int pSize, HPTimer::HighPerTimer & pStartTime,
                           HPTimer::HighPerTimer & pDuration, HPTimer::HighPerTimer & pFinishT, int pPriority,
                           unsigned char pTOS,  SockOptsMap pOptsMap = SockOptsMap() );
        virtual ~CFullSpeedSource();
        // the dipatch routine. Performance tuned
        virtual bool Dispatch();
        virtual std::string PrintSDesc();

    private:
        unsigned int mSize;

};


/// the FIFO Source
class CFIFOSource: public CDataSource
{
    public:
        /// standard ctor, ctor without transmission parameters doesn't sence
        CFIFOSource ( unsigned int pLocalAddr, unsigned short pLocalPort,
                      unsigned int pRemoteAddr, unsigned short pRemotePort,
                      unsigned int pBytes, unsigned int pPackets, proto_t pProto,
                      HPTimer::HighPerTimer & pStartTime, HPTimer::HighPerTimer & pDuration, HPTimer::HighPerTimer & pFinishT,
                      int pPriority, std::string & pFifoName, unsigned char pTOS,
                      SockOptsMap pOptsMap = SockOptsMap() );
        /// dtor
        virtual ~CFIFOSource();
        /// the dispatch routine of the data source scheduler
        virtual bool Dispatch();
        /// print measurement results to the standard output
        virtual std::string PrintSDesc();
        /** parse line, fetched from the FIFO. The Token format is:
         sendingTime_sec|sendingTime_usec|packetSize all values are interpreted
         as 4 Bytes unsigned long in network byte order (big endian)
        */
        virtual PDU_Desc ParseLine ( std::string & pLine );
        // fifo reader method
        void ReadFifo();
    private:
        unsigned long mLines;
        std::string mFifoName;
        std::istream * mFifoStream_ptr;
        std::queue <PDU_Desc> mPDU_Queue;
        pthread_mutex_t mFinishedM;
        pthread_mutex_t mPDUQueueM;
        /// pointer to next pdu to be sent
        PDU_Desc * mNextPDU_ptr;
        /// Finished to read lines from Reader
        bool mReaderFinished;
};

/**
   the Dummy Source, only for gathering results from the remote source,
   build statistics and print out results
*/
class CDummySource: public CDataSource
{
    public:
        /// ctor
        CDummySource();
        /// dtor
        ~CDummySource();
        /// dummy dispatch routine (to be interface-conform
        virtual bool Dispatch();
        /// print out source description
        virtual std::string PrintSDesc();
        /// store received (whole) sample
        void StoreSample ( char* pHeader );
        /// imports basic statistics from an external source
        void ImportBasicStats ( BasicSourceStats_t & pStats );

};


/**
   class for storing packet headers, packet headers are assumed to be
   of size 12 Bytes
*/
class CRHeaderContainer
{
    public:
        /**
         * ctor. @param pPrefetchElements gives
         * the number of elements to be preallocated
        */
        CRHeaderContainer ( long long pPreallocElements = 0 );
        /// dtor
        virtual ~CRHeaderContainer();
        void saveHeader ( CRPDUHeader & pHeader );
        CRPDUHeader * getNextHeader();
        /// returns stored packet counter
        unsigned int size();
        /// accessor to the n-th element
        CRPDUHeader & at ( unsigned int pPos );

    private:
        /// arrays with 100 * 1k headers are saved in the vector
        static const unsigned long BUFFER_CONTAINER_SIZE = 100 * 1024;
        CRPDUHeader * RPDUHeaderArray;
        /// vector, that stores arrays of 100 * 1k headers
        typedef std::vector <CRPDUHeader *> PDUHeaderVector;
        typedef PDUHeaderVector::iterator PDUHeaderIterator;
        PDUHeaderVector mHeaderVec;
        /// used vector  index for write access
        unsigned long mWVectorIdx;
        unsigned long mWArrayIdx;
        /// used vector  index for read access
        unsigned long mRVectorIdx;
        unsigned long mRArrayIdx;
        /// number of currently stored headers within the header container
        uint64_t mSize;

};

/**
   Data Receiver basis class
*/
class CDataReceiver
{
    public:
        CDataReceiver ( CIPSocket * pSocket, long pSampleNum );
        virtual ~CDataReceiver();
        /**
           if a complete massage reassambled, returns a pointer to that message,
        null pointer otherwise
        */
        virtual  CRPDUHeader * getNextHeader();
        virtual long Recv() = 0;     // disassemble the packet from Buffer

        /// save header to  the vector
        void SaveHeader ( char * pHeader );
        /**
           freeze the sample vector - especially, set mCur to the correct begin()
           iterator
        */
        void Freeze ( unsigned long pLastSeqNo, HPTimer::HighPerTimer & pBackLogT );
	

	
    protected:

      	/// container for stored headers
        CRHeaderContainer mRHeaderContainer;
        /// pointer to the IP socket
        CIPSocket * mSocket;
        unsigned long mBytesRecv;
        /// temporary timer variables
        HPTimer::HighPerTimer mNow;
        struct timeval mNowTV;
        CRPDUHeader mRHeader;
};

/// Data receiver class for TCP receiver
class CTCPReceiver: public CDataReceiver
{
    public:
        /** ctor.
            @param pSocket is the socket, the receiver have to listen on
            @param pMaxPDU fives the maximum size of LTest PDUs the receiver should
            be prepared to receive
        */
        CTCPReceiver ( CIPSocket * pSocket, long pSampleNum, unsigned long pMaxPDU = MAX_IP_SIZE );
        /// dtor
        virtual ~CTCPReceiver();
        /// receive tcp data from the socket
        long Recv();
    protected:
        /**
           buffer for assembling TCP segments
           it is assumed, the reaed pointer is always on the beginning of
           a LTest-PDU
        */
        ltest::CRingBuffer mRingBuffer;
        /// packet size
        unsigned long mPacketSize;
        /// received TCP segment buffer
        /* char mTCPSegBuffer[MAX_IP_SIZE]; */
        char * mTCPSegBuffer;
        /// LTest-PDU-Header buffer
        char mPDUHeader[16];
        // maximum permitted size of received PDUs
        unsigned long mMaxPDU;
};

/// Data receiver class for UDP receiver
class CUDPReceiver: public CDataReceiver
{
    public:
        /// ctor
        CUDPReceiver ( CIPSocket * pSocket, long pSampleNum );
        /// dtor
        virtual ~CUDPReceiver();
        /// receive a udp data packet from the socket
        long Recv();
    private:
        /// buffer for storing received packets
        char mBuffer[MAX_IP_SIZE];

};

/// UDT receiver - essentially it shuould be an UDP receiver
class CUDTReceiver: public CTCPReceiver
{
    public:
        /// ctor
        CUDTReceiver ( CIPSocket * pSocket, long pSampleNum, unsigned long pMaxPDU = MAX_IP_SIZE );
        virtual ~CUDTReceiver();

    private:
};

// FastTCP receiver - essentially it is an TCP receiver
class CFastTCPReceiver: public CTCPReceiver
{
    public:
        /// ctor
        CFastTCPReceiver ( CIPSocket * pSocket, long pSampleNum, unsigned long pMaxPDU = MAX_IP_SIZE );
        /// dtor
        virtual ~CFastTCPReceiver();
    private:
};


// High Speed TCP receiver - essentially it is an TCP receiver
class CHSTCPReceiver: public CTCPReceiver
{
    public:
        /// ctor
        CHSTCPReceiver ( CIPSocket * pSocket, long pSampleNum, unsigned long pMaxPDU = MAX_IP_SIZE );
        /// dtor
        virtual ~CHSTCPReceiver();
    private:
};



/** Data receiver class for SDP receiver, currently is one-by-one passed to the TCP receiver because of the same receiving
 technique
*/

class CSDPReceiver: public CTCPReceiver
{
    public:
        /// ctor
        CSDPReceiver ( CIPSocket * pSocket, long pSampleNum, unsigned long pMaxPDU = MAX_IP_SIZE );
        /// dtor
        virtual ~CSDPReceiver();
    private:
};


/// the Poisson data source modell
class CPoissonSource: public CDataSource
{
    public:
        /// standard ctor, ctor without transmission parameters doesn't sence
        CPoissonSource ( unsigned int pLocalAddr, unsigned short pLocalPort,
                         unsigned int pRemoteAddr, unsigned short pRemotePort,
                         unsigned int pBytes, unsigned int pPackets, proto_t pProto,
                         unsigned long long pRate, unsigned int pSize, HPTimer::HighPerTimer & pStartTime,
                         HPTimer::HighPerTimer & pDuration, HPTimer::HighPerTimer & pFinishT, int pPriority,
                         unsigned char pTOS, SockOptsMap pOptsMap = SockOptsMap() );
        /// dtor
        virtual ~CPoissonSource();
        /// the dispatch routine of the source scheduler
        virtual bool Dispatch();
        /// print out source description
        virtual std::string PrintSDesc();
    private:

        unsigned long long mRate;
        unsigned int mSize;
        float mLambda;
        /// time for sending next data packet
        HPTimer::HighPerTimer mNextTime;
        /// time distance in TSC tics between two
        /// packets - for faster calculation
        long long  mInterPacketTics;

        /// save creation time of the packet in the vector structure
        /// calculate the next time for dispatching a PDU
        HPTimer::HighPerTimer & calcNextTime ( HPTimer::HighPerTimer & Now );
};



#endif // ---DATASOURCE_H----
