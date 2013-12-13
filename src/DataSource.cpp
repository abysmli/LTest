// This may look like C code, but it is really -*- C++ -*-
//
//
// Author:  Author Eduard Siemens <siemens@rvs.uni-hannover.de>
//
// File:    DataSource.cpp
//          Tue Aug 17 2002
//
// $Id: DataSource.cpp,v 2.18 2005/07/19 15:46:19 siemens Exp $
// $Author:   siemense  $
// $Date:   08 Mar 2007 11:51:38  $
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
// $Log:   //hanvsfile02/prog/Projects/2005/hipernet/Repository/hipernet/archives/hipernet/Linux/Apps/ltest/DataSource.cpp-arc  $
//
//   Rev 1.12.1.5   08 Mar 2007 11:51:38   siemense
//minor statistics bugs fixed.
//improvement of measurement accuracy.
//Now the order within the data source dispatch is: assemble data packet -> get send time -> send data packet ->store header.
//
//   Rev 1.12.1.4   31 Jan 2007 09:10:38   siemense
//Efficiency issues solved. Interfaces to network classes has been cleaned up.
//
//   Rev 1.12.1.3   13 Nov 2006 11:57:20   siemense
//Minor bug fixes.
//Version before CIPNetworkPerformance tunings.
//
//   Rev 1.12.1.1   17 Aug 2006 13:21:08   siemense
//TSC timer class has been implemented and rudimentary tested
//
//   Rev 1.12.1.0   Jun 27 2006 10:39:50   siemense
//New Protocols Branch
//
//   Rev 1.12   21 Jun 2006 13:17:26   siemense
//Version befor the big time synchronization change
//
//   Rev 1.11   09 Mar 2006 15:18:36   siemense
//Remote sender mode uninitialized IP fragment buffer bug fixed
//
//   Rev 1.10   16 Feb 2006 16:10:12   siemense
//FullSpeed source added. Minor bugs fixed.
//
//   Rev 1.9   08 Feb 2006 13:25:40   siemense
//Infiniband support added
//
//   Rev 1.8   13 Jan 2006 14:53:40   siemense
//Local address assignment bug fixed
//
//   Rev 1.7   22 Dec 2005 16:39:06   siemense
//intermediate checkin. Not working version!
//
//   Rev 1.6   21 Dec 2005 16:42:28   siemense
//Time correction between data sender and data recei ver added
//
//   Rev 1.5   18 Nov 2005 10:15:46   siemense
//Working Version with fixed remote sender bugs
//
//   Rev 1.4   15 Nov 2005 15:10:38   siemense
//zero duration and packet count bug fixed
//
//   Rev 1.3   14 Nov 2005 16:32:12   siemense
//Data range of rate extended to long long. Minor bug fixes.
//
//   Rev 1.2   07 Oct 2005 09:11:26   siemense
//unnecessary files removed
//
//   Rev 1.1   06 Oct 2005 17:06:56   siemense
//data rate limit extended
//
//   Rev 1.0   28 Sep 2005 16:38:36   siemense
//Initial revision.
// Revision 2.18  2005/07/19 15:46:19  siemens
// Final release from Eduard Siemens at the RVS
//
// Revision 2.17  2005/02/23 09:16:08  siemens
// allDigits bug fixed
//
// Revision 2.16  2005/02/07 14:43:14  siemens
// bereinigung von fremden Code
//
// Revision 2.13  2004/07/03 10:44:59  siemens
// intermediate checkin
//
// Revision 2.12  2004/07/02 21:20:28  siemens
// TCP connection closing bug fixed
//
// Revision 2.11  2004/07/01 19:48:09  siemens
// intermediate version, tcp performance bug is still there
//
// Revision 2.10  2004/06/28 19:20:08  siemens
// corbaRemoteAddress bug fixed, RingBuffer class for TCP reassembling added
//
// Revision 2.9  2004/05/25 14:08:29  siemens
// BSD license copyright notice added
//
// Revision 2.8  2004/05/13 08:57:55  siemens
// dump file zero times bug fixed
//
// Revision 2.7  2004/04/02 14:30:35  siemens
// 100 percent error bug fixed
//
// Revision 2.6  2004/03/26 06:46:30  siemens
// TCP segment buffer overflow bug fixed
//
// Revision 2.5  2004/03/12 21:09:34  siemens
// *** empty log message ***
//
// Revision 2.4  2004/02/26 15:52:41  siemens
// corba uri bug fixed
//
// Revision 2.3  2004/02/04 16:21:52  siemens
// *** empty log message ***
//
// Revision 2.2  2003/12/11 13:32:58  siemens
// Working version with remote source
//
// Revision 2.1  2003/09/04 13:30:16  siemens
// priority scheduling tuned
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

#include <iostream>

#include <time.h>
#include <cmath>
#include <new>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <string>
#include <sstream>
#include "DataSource.h"
#include "cstats.h"
//#include "CipJetSocket.h"

/// default ctor, instatiate only the statistics part
CDataSource::CDataSource() :
		mSeqNo(0), mFinished(false), mSocket(0), mBytes(0), mSentPacketStat(0), mSentBytesStat(
				0), mRecvPacketStat(0), mRecvBytesStat(0), mDuplicates(0), mCorrupted(
				0), mDeltaTime_end(0)
{
	mBuff = new char[MAX_IP_SIZE];
}

/// ctor, used by CBR and FIFO source
CDataSource::CDataSource(unsigned int pLocalAddr, unsigned pLocalPort,
		unsigned int pRemoteAddr, unsigned pRemotePort, unsigned int pBytes,
		unsigned int pPackets, proto_t pProto,
		HPTimer::HighPerTimer & pStartTime, HPTimer::HighPerTimer & pDuration,
		HPTimer::HighPerTimer & pFinishTime, int pPriority, unsigned char pTOS,
		SockOptsMap pOptsMap) :
		mStartTime(pStartTime), mFinishTime(pFinishTime), mDuration(pDuration), mProto(
				pProto), mSeqNo(0), mFinished(false), mSocket(0), mBytes(
				pBytes), mPackets(pPackets), mpTimestamp(0), mPriority(0), mSentPacketStat(
				0), mSentBytesStat(0), mRecvPacketStat(0), mRecvBytesStat(0), mDuplicates(
				0), mCorrupted(0), mReordered(0), mTOS(pTOS), mLastSeqNo(0)
{
	// allocate memory space for the packet buffer
	mBuff = new char[MAX_IP_SIZE];
	// initialize the IP-Packet buffer
	struct timeval now;
	//gettimeofday ( &now, NULL );
	HPTimer::HighPerTimer HPnow;
	HPnow = HPTimer::HighPerTimer::Now();
	HPnow.SetTV(now);

	srand(now.tv_sec);
	// fill out the packet buffer with random bytes pattern
	for (char * Pos = mBuff; Pos < (mBuff + sizeof(mBuff)); Pos += sizeof(int))
	{
		*(int *) Pos = rand();
	}
	// create an new socket without listener and then connect the peer

	bool bind_to_port(false);
	if (pLocalPort != 0 || pLocalAddr != INADDR_ANY )
	{
		bind_to_port = true;
	}
	if (pProto == P_TCP)
	{
		mSocket = new CTCPSocket(bind_to_port, pLocalPort, pLocalAddr, pTOS);
	}
	else if (pProto == P_UDP)
	{
		mSocket = new CUDPSocket(bind_to_port, pLocalPort, pLocalAddr, pTOS);
	}
	else if (pProto == P_UDT)
	{
		//CIPAddress localAddr(pLocalAddr, pLocalPort);
		//mSocket = new CJETSocket ( bind_to_port, localAddr );
	}
	else if (pProto == P_FastTCP)
	{
		mSocket = new CFastTCPSocket(bind_to_port, pLocalPort, pLocalAddr,
				pTOS);
		std::cerr << "new FastTCP socket started !!!" << std::endl;
	}
	else if (pProto == P_HSTCP)
	{
		mSocket = new CHSTCPSocket(bind_to_port, pLocalPort, pLocalAddr, pTOS);
		std::cerr << "new High Speed TCP socket started !!!" << std::endl;
	}

#ifdef SDP_ENABLED
	else if ( pProto == P_SDP )
	{
		mSocket = new CSDPSocket ( bind_to_port, pLocalPort, pLocalAddr, pTOS );
	}
#endif //SDP_ENABLED
	else
	{
		std::cerr << "Not supported transport protocol. Currently TCP, UDP";

#ifdef SDP_ENABLED

		std::cerr << " and SDP";
#endif //SDP_ENABLED
		std::cerr << " protocols supported" << std::endl;
		exit(3);
	}
	mSocket->Connect(pRemoteAddr, pRemotePort);
	std::cerr << "connected to remote address:\t" << mSocket->RemoteAddr()
			<< "\nfrom local address:\t"
			<< CIPAddress::atodd(mSocket->LocalAddr()) << "\n" << std::endl;

	// get the start time and duration and calculate the finish time
	if (mStartTime < HPTimer::HighPerTimer::Now())
	{
		mStartTime.SetNow();
	}
	if (!mDuration.IsNil())
	{
		mFinishTime = mStartTime + mDuration;
	}

	// sleep, if more than 30 ms till start time should be waited
	HPTimer::HighPerTimer SleepResv(static_cast<int64_t>(0u),
			static_cast<int64_t>(030000000u));
	mNow.SetNow();
	if (mStartTime > (mNow + SleepResv))
	{
		HPTimer::HighPerTimer SleepInter(mStartTime - mNow - SleepResv);
		/*struct timespec SleepTime;
		 SleepTime.tv_sec = SleepInter.Seconds();
		 SleepTime.tv_nsec = SleepInter.NSeconds();
		 nanosleep ( &SleepTime, NULL );*/
		SleepInter.SleepToThis();
	}
}

/// dtor, deletes the buffer array
CDataSource::~CDataSource()
{
	delete[] mBuff;

	if (mSocket)
	{
		delete mSocket;
		mSocket = 0;
	}
	std::cerr << "\n" << CStringUtils::decades_formatted(mHeaderVec.size())
			<< " packets sent" << std::endl;
}

/**
 routine for assembling pdu with given length. The actual time is
 written to the pdu
 */
void CDataSource::AssemblePDU(const int pLen)
{
	// set the header of PDU, all fields are stored in network byte order
	struct timeval now;
	//gettimeofday ( &now, NULL ); // TODO: optimize code here
	HPTimer::HighPerTimer HPnow;
	HPnow = HPTimer::HighPerTimer::Now();
	HPnow.SetTV(now);

	char * Pos = mBuff;
	*(uint32_t *) Pos = htonl(pLen);
	Pos += 4;
	*(uint32_t *) Pos = htonl(mSeqNo++);
	Pos += 4;
	*(uint32_t *) Pos = htonl(now.tv_sec);
	Pos += 4;
	*(uint32_t *) Pos = htonl(now.tv_usec);
	//  mHeaderVec.push_back(mBuff);	
}

/**
 routine for storing received data
 */
void CDataSource::StoreRSample(char * pHeader)
{
	uint32_t SeqNo(ntohl(*(uint32_t *) pHeader));
	// test of reorderings
	if (SeqNo < mLastSeqNo)
	{
		mReordered++;
#ifdef DEBUG_OUTPUT

		std::cout << "REORD POS:\t" << SeqNo << std::endl;
#endif

	}
	else
	{
		mLastSeqNo = SeqNo;
	}

	if (SeqNo > mHeaderVec.size())
	{
		mCorrupted++;
		return;
	}
	if (*(uint32_t *) &(mHeaderVec[SeqNo].Header[16]) != 0)
	{
		mDuplicates++;

		std::cerr << "Duplicated packet SeqNo is: " << SeqNo
				<< "\tduplicates count is: " << mDuplicates
				<< "\ncontent of receiver field is: "
				<< ntohl(*(uint32_t *) &(mHeaderVec[SeqNo].Header[16]))
				<< std::endl;
	}

	memcpy(&(mHeaderVec[SeqNo].Header[16]), pHeader + 4, 8);
	mRecvPacketStat++;
	mRecvBytesStat += mHeaderVec[SeqNo].Length();
	return;
}

/**
 close the socket
 */
void CDataSource::CloseSocket()
{
	if (mSocket)
		mSocket->Close();
}
/**
 store human readable statistics in a string
 */
std::string CDataSource::PrintStats()
{
	std::stringstream Str;
	HPTimer::HighPerTimer Dur(mFinishTime - mStartTime);

	double rate = (double) mSentBytesStat * 8
			/ HPTimer::HighPerTimer::HPTimertoD(Dur);
	// rate, including TCP/UDP, IP as well as Ethernet Header (rough approximation)
	// L2 = 18 Bytes, L3 = 20 Bytes, L4 : TCP: 20 Bytes, UDP: 8 Bytes, SDP:
	double Overhead = 18 + 20;
	switch (mProto)
	{
	case P_TCP:
		Overhead += 20;
		break;
	case P_UDP:
	case P_UDT:
		Overhead += 8;
		break;
	case P_SDP:
	case P_FastTCP:
	case P_HSTCP:
		break;
#ifdef SDP_ENABLED

		case P_SDP:
		Overhead += 1;
		break;
#endif // SDP_ENABLED
	}

	// not really pretty calc, because padding not taken into account
	double meanSize = (double) mSentBytesStat / mSentPacketStat;

	double bruttoRate = rate * (Overhead + meanSize) / meanSize;
	Str << "Session duration, seconds:\t"
			<< CStringUtils::decades_formatted(Dur.Seconds()) << "."
			<< Dur.NSeconds() << "\n";
	Str << "Packets sent:\t" << CStringUtils::decades_formatted(mSentPacketStat)
			<< "\n";
	Str << "Bytes sent:\t" << CStringUtils::decades_formatted(mSentBytesStat)
			<< "\n";
	Str << "Sending data rate, bit/s:\t" << rate << " (estimated L2-rate: "
			<< bruttoRate << ")\n";
	Str << "Packets received:\t"
			<< CStringUtils::decades_formatted(mRecvPacketStat) << "\n";
	Str << "Bytes received:\t"
			<< CStringUtils::decades_formatted(mRecvBytesStat) << "\n";

	rate = (double) mRecvBytesStat * 8 / HPTimer::HighPerTimer::HPTimertoD(Dur);
	bruttoRate = rate * (Overhead + meanSize) / meanSize;

	Str << "Receiving data rate, bit/s:\t" << rate << " (estimated L2-rate: "
			<< bruttoRate << ")\n";
	Str << "Duplicated packets:\t" << mDuplicates << "\n";
	Str << "Corrupted packets:\t" << mCorrupted << "\n";
	Str << "Reordered packets:\t" << mReordered << "\n";
	Str << "Lost Packets:\t"
			<< CStringUtils::decades_formatted(
					mSentPacketStat - mRecvPacketStat) << "\n";
	Str << "Packet Loss:\t"
			<< (double) (mSentPacketStat - mRecvPacketStat) / mSentPacketStat
					* 100 << " %" << std::endl;
	return Str.str();
}

/**
 calculate basic statistisc for the transmission and store result
 in human readable form in the string.
 Calculations are corrected on the fly by mDeltaTime
 */
std::string CDataSource::PrintBasicStats()
{
	CBasicStats basicStats;

	std::vector<CPDUHeader>::iterator it(mHeaderVec.begin());

	for (; it != mHeaderVec.end(); it++)
	{
		HPTimer::HighPerTimer S(it->SendTime()), R(it->RecvTime()), D;
		if (!R.IsNil())
		{
			D = R - S;
		}
		else
		{
			D = HPTimer::HighPerTimer::Nil();
		}
		uint32_t SeqNo(it->SeqNo()), Len(it->Length());
		basicStats.procLine(SeqNo, S, R, D, Len);
	}

	basicStats.postProc();
	basicStats.CorrectDelays(mDeltaTime);
	basicStats.showResults(std::cout);

	return "";
}

/**
 store human readable statistics in a string
 */
void CDataSource::DumpSamples(std::ostream * pDStream)
{
	if (mDeltaTime_end.IsNil())
	{
		mDeltaTime_end = mDeltaTime;
		mDeltaTime_end_pos.Nil();
	}
	double x1 = HPTimer::HighPerTimer::HPTimertoD(mDeltaTime_pos);
	double x2 = HPTimer::HighPerTimer::HPTimertoD(mDeltaTime_end_pos);
	double y1 = HPTimer::HighPerTimer::HPTimertoD(mDeltaTime);
	double y2 = HPTimer::HighPerTimer::HPTimertoD(mDeltaTime_end);
	double summe(0);
	uint64_t i(0);
	std::vector<CPDUHeader>::iterator it(mHeaderVec.begin());
	*pDStream << "# Format of the dump file is:\n";
	*pDStream << "# <x number>\\t<sentTime_sec>.<sentTime_microsec>";
	*pDStream << "\\t<recvTime_sec>.<recvTime_microces>\\t<Delay_sec>.";
	*pDStream << "<Delay_microsec>\\t<Packet_length>\n";
	*pDStream
			<< "# in bytes\t <correctionTime_sec>.<correctionTime_microsec>\\t<Corrected_Delay_sec>.<Corrected_Delay_microsec> # to be applied\n";
	for (; it != mHeaderVec.end(); it++)
	{
		HPTimer::HighPerTimer S, R, D;
		S = it->SendTime();
		R = it->RecvTime();
		if (!R.IsNil())
		{
			D = R - S;
		}
		else
		{
			// virtually infinity
			D = HPTimer::HighPerTimer(static_cast<int64_t>(0xFFFFFFu),
					static_cast<int64_t>(0u));
		}
		*pDStream << it->SeqNo() << "\t" << S << "\t" << R << "\t" << D << "\t"
				<< it->Length() << "\t"
				<< HPTimer::HighPerTimer::DtoHPTimer(
						((y2 - y1) * HPTimer::HighPerTimer::HPTimertoD(S)
								+ (x2 - x1) * y1 - (y2 - y1) * x1) / (x2 - x1))
				<< "\t"
				<< D
						- HPTimer::HighPerTimer::DtoHPTimer(
								((y2 - y1)
										* HPTimer::HighPerTimer::HPTimertoD(S)
										+ (x2 - x1) * y1 - (y2 - y1) * x1)
										/ (x2 - x1)) << "\n";
		summe += HPTimer::HighPerTimer::HPTimertoD(
				D
						- HPTimer::HighPerTimer::DtoHPTimer(
								((y2 - y1)
										* HPTimer::HighPerTimer::HPTimertoD(S)
										+ (x2 - x1) * y1 - (y2 - y1) * x1)
										/ (x2 - x1)));
		i++;
	}
//#ifdef DEBUG_OUTPUT
	std::cout << "\nDumpsamples' Processing\n";
	std::cout << "TimeCorrection at begin           : " << mDeltaTime
			<< std::endl;
	std::cout << "TimeCorrection at end             : " << mDeltaTime_end
			<< std::endl;
	std::cout << "TimeCorrection at begin position  : " << mDeltaTime_pos
			<< std::endl;
	std::cout << "TimeCorrection at end position    : " << mDeltaTime_end_pos
			<< std::endl;
	std::cout<<"i: "<<i<<std::endl;
	std::cout<<"Mean delay: "<<HPTimer::HighPerTimer::DtoHPTimer(summe/i)<<std::endl;
//	if (it->SeqNo() == 0)
//	{
//		std::cout << "First sample corrected value      : " << D - mDeltaTime << std::endl;
//	}
//	if (it->SeqNo() == (mSeqNo - 1))
//	{
//		std::cout << "Last sample corrected value       : "
//		<< D - mDeltaTime
//		- HPTimer::HighPerTimer::DtoHPTimer(
//				dDeltaTimeCorrection * it->SeqNo())
//		<< std::endl;
//	}
//#endif
}

/**
 returns Basic Data source stats, gathered during the runtime
 */
BasicSourceStats_t CDataSource::getBasicStats()
{
	BasicSourceStats_t Stats;
	Stats.StartTime.tv_sec = mStartTime.Seconds();
	Stats.StartTime.tv_usec = mStartTime.USeconds();
	Stats.Duration.tv_sec = mDuration.Seconds();
	Stats.Duration.tv_usec = mDuration.USeconds();
	Stats.FinishTime.tv_sec = mFinishTime.Seconds();
	Stats.FinishTime.tv_usec = mFinishTime.USeconds();
	Stats.Duplicates = mDuplicates;
	Stats.Corrupted = mCorrupted;
	Stats.Reordered = mReordered;
	Stats.SentPackets = mSentPacketStat;
	Stats.SentBytes = mSentBytesStat;
	Stats.RecvPackets = mRecvPacketStat;
	Stats.RecvBytes = mRecvBytesStat;
	Stats.DeltaTime = HPTimer::HighPerTimer::HPTimertoNSec(mDeltaTime) / 1000;
	return Stats;

}

HPTimer::HighPerTimer&
CDataSource::getDeltaTime()
{
	return mDeltaTime;
}

std::vector<CPDUHeader> CDataSource::getHeader()
{
	return mHeaderVec;
}

/**
 freeze the sender data structure, set the mCur iterator to the begin of
 the sender samples vector
 */
void CDataSource::Freeze()
{
	mCur = mHeaderVec.begin();
}

void CDataSource::SetDeltaTime(HPTimer::HighPerTimer & pDelta,
		HPTimer::HighPerTimer & pDelta_pos)
{
	mDeltaTime = pDelta;
	mDeltaTime_pos = pDelta_pos;
}

void CDataSource::SetDeltaTime_end(HPTimer::HighPerTimer & pDelta,
		HPTimer::HighPerTimer & pDelta_pos)
{
	mDeltaTime_end = pDelta;
	mDeltaTime_end_pos = pDelta_pos;
}
/**
 get the pointer to the next Header element in the array
 */
CPDUHeader *
CDataSource::getNextHeader()
{
	if (mCur == mHeaderVec.end())
	{
		return 0;
	}
	return &(*mCur++);
}

/**
 standard ctor, starts the real source
 */
CCBRSource::CCBRSource(unsigned int pLocalAddr, unsigned short pLocalPort,
		unsigned int pRemoteAddr, unsigned short pRemotePort,
		unsigned int pBytes, unsigned int pPackets, proto_t pProto,
		unsigned long long pRate, unsigned int pSize,
		HPTimer::HighPerTimer & pStartTime, HPTimer::HighPerTimer & pDuration,
		HPTimer::HighPerTimer & pFinishT, int pPriority, unsigned char pTOS,
		SockOptsMap pOptsMap) :
		CDataSource(pLocalAddr, pLocalPort, pRemoteAddr, pRemotePort, pBytes,
				pPackets, pProto, pStartTime, pDuration, pFinishT, pPriority,
				pTOS, pOptsMap), mRate(pRate), mNextTime(mStartTime), mInterPacketTics(
				0)
{

	// for debugging purposes only
#ifdef DEBUG_OUTPUT
	std::cout << "DataSource parameters:\n"
	<< "pLocalAddr " << CIPAddress::atodd ( pLocalAddr ) << ":" << pLocalPort << "\n"
	<< "pRemoteAddr " << CIPAddress::atodd ( pRemoteAddr ) << ":" << pRemotePort << "\n"
	<< "pBytes " << pBytes << "\n"
	<< "pPackets " << pPackets << "\n"
	<< "pProto " << pProto << "\n"
	<< "pRate " << pRate << "\n"
	<< "pSize " << pSize << "\n"
	<< "pStartTime " << pStartTime << "\n"
	<< "pDuration" << pDuration << "\n"
	<< "pFinishTime " << pFinishT << "\n"
	<< "pPriority " << pPriority << "\n"
	<< "pTOS " << pTOS << "\n"
	<< "__ End of DataSource parameters ___" << std::endl;
#endif

	// set the packet size within the minimum/maximum bound
	// adjust the size to by within the maximum/minimum boundary size
	mSize = pSize > 16 ? pSize : 16;
	mSize = pSize > MAX_IP_SIZE - 30 ? MAX_IP_SIZE - 30 : pSize;

	mNextTime = mStartTime;
	if (mRate != 0)
	{
		mInterPacketTics = static_cast<long long>(mSize) * 8 * 1000000
				* HPTimer::HighPerTimer::GetHPFrequency() / mRate;
	}
	// initialize the vector with sent pdu headers

	if (mPackets != 0)
	{
		mHeaderVec.reserve(mPackets);
		// finish time shold be in the very far future (for avoiding exceptions)
		mFinishTime = HPTimer::HighPerTimer::Nil();
		// mFinishTime.SecAdd ( 0xFFFFFFFF); // Fedotova
		//mFinishTime.SecAdd ( HPTimer::HighPerTimer::HPTimer_MAX.Seconds());
		mFinishTime = HPTimer::HighPerTimer::HPTimer_MAX;
	}
	else if (mBytes != 0)
	{
		mHeaderVec.reserve(mBytes / mSize + 1);
		// finish time shold be in the very far future (for avoiding exceptions)
		mFinishTime = HPTimer::HighPerTimer::Nil();
		// mFinishTime.SecAdd ( 0xFFFFFFFF); 
		//mFinishTime.SecAdd ( HPTimer::HighPerTimer::HPTimer_MAX.Seconds());
		mFinishTime = HPTimer::HighPerTimer::HPTimer_MAX;

	}
	// wet try to estimate the amount of reserved space. But limit it by
	// 5 millions currently
	else
	{
		HPTimer::HighPerTimer duration(mFinishTime - mStartTime);

		if (mInterPacketTics != 0)
		{
			unsigned long long packets = duration.HPTics() / mInterPacketTics;
			packets = std::min(packets, (unsigned long long) 5000000);
			mHeaderVec.reserve(packets + 2);
		}
	}

	// set the priority of the data source thread
	if (getuid() == 0)
	{
		pPriority == -1 ? sched_get_priority_min(SCHED_RR) : pPriority;
		mPriority = pPriority;
		struct sched_param tParam;
		memset(&tParam, 0, sizeof(tParam));
		sched_getparam(0, &tParam);
		tParam.sched_priority = pPriority;
		sched_setscheduler(0, SCHED_RR, &tParam);
	}
}

/// dtor
CCBRSource::~CCBRSource()
{
}

/**
 dispatch routine for CBR source, return true, if packet sent, false
 otherwise
 */bool CCBRSource::Dispatch()
{

	if ((mPackets != 0 && mSentPacketStat >= mPackets)
			|| (mBytes != 0 && mSentBytesStat >= mBytes)
			|| (mBytes == 0 && mPackets == 0
					&& HPTimer::HighPerTimer::Now() >= mFinishTime))
	{
#ifdef DEBUG_OUTPUT
		std::cerr << "Data source packet statistics debug :\n"
		<< "mPackets " << mPackets << "\n"
		<< "mSentPacketStat " << mSentPacketStat << "\n"
		<< "mBytes " << mBytes << "\n"
		<< "mSentBytesStat " << mSentBytesStat << "\n..." << std::endl;

#endif

		mFinished = true;
	}

	if (mFinished)
	{
		// store real finish time
		mFinishTime.SetNow();
		// mSentPacketStat = mHeaderVec.size();
		return false;
	}
	if (HPTimer::HighPerTimer::Now() >= mNextTime)
	{
		// prepare and send a PDU
		AssemblePDU(mSize);
		mSocket->Send(mBuff, mSize);
		// store the PDU to the header vector
		mHeaderVec.push_back(mBuff);
		mSentBytesStat += mSize;
		mSentPacketStat++;
		calcNextTime(mNextTime);
		return true;
	}
	return false;
}

/**
 print out the data source description
 */
std::string CCBRSource::PrintSDesc()
{
	std::stringstream Str;
	Str << "CBR source description" << std::endl;
	Str << "Session start time:\t" << mStartTime << " seconds ( "
			<< mStartTime.PrintTime(false, false) << " ) " << "\n";
	Str << "Session duration, s:\t" << mDuration << "\n";
	Str << "Session finish time:\t" << mFinishTime << " seconds ( "
			<< mFinishTime.PrintTime(false, false) << " ) " << "\n";
	std::string Proto;
	if (mProto == P_UDP)
	{
		Proto = "UDP";
	}
	else if (mProto == P_TCP)
	{
		Proto = "TCP";
	}
	else if (mProto == P_SDP)
	{
		Proto = "SDP";
	}
	else if (mProto == P_UDT)
	{
		Proto = "UDT";
	}
	else if (mProto == P_HSTCP)
	{
		Proto = "HSTCP";
	}
	else if (mProto == P_FastTCP)
	{
		Proto = "FastTCP";
	}
	else
	{
		Proto = "Unknown protocol";
	}

	Str << "Transport protocol:\t" << Proto << "\n";

	Str << "Sending data rate, bit/s:\t"
			<< CStringUtils::decades_formatted(mRate) << "\n";
	Str << "Packet size, Bytes:\t" << CStringUtils::decades_formatted(mSize)
			<< "\n";
	return Str.str();
}

/**
 calculate the time for next packet
 */
HPTimer::HighPerTimer &
CCBRSource::calcNextTime(HPTimer::HighPerTimer & Now)
{
	return Now.TicAdd(mInterPacketTics);
}

/// wrapper for starting line reading tread
extern "C"
{
void *fifo_reader(void *pClass)
{
	std::cout << "starting FIFO reader in a new thread ..." << std::endl;
	((CFIFOSource *) pClass)->ReadFifo();
	std::cout << "finishes FIFO reader thread..." << std::endl;
	return pClass;
}
}

/// ctor, start listener and connect to the peer, open FIFO file
CFIFOSource::CFIFOSource(unsigned int pLocalAddr, unsigned short pLocalPort,
		unsigned int pRemoteAddr, unsigned short pRemotePort,
		unsigned int pBytes, unsigned int pPackets, proto_t pProto,
		HPTimer::HighPerTimer & pStartTime, HPTimer::HighPerTimer & pDuration,
		HPTimer::HighPerTimer & pFinishT, int pPriority,
		std::string & pFifoName, unsigned char pTOS, SockOptsMap pOptsMap) :
		CDataSource(pLocalAddr, pLocalPort, pRemoteAddr, pRemotePort, pBytes,
				pPackets, pProto, pStartTime, pDuration, pFinishT, pPriority,
				pTOS, pOptsMap), mFifoName(pFifoName), mFifoStream_ptr(0), mNextPDU_ptr(
				0), mReaderFinished(false)
{
	mFifoStream_ptr = new std::ifstream(mFifoName.c_str());
	pthread_t tID;
	pthread_attr_t tAttr;
	pthread_attr_init(&tAttr);
	pthread_attr_t * tAttr_ptr = &tAttr;
	if (getuid() == 0)
	{
		// set the priority of the data source thread as well as the reader thread
		pPriority == -1 ? sched_get_priority_max(SCHED_RR) : pPriority;
		mPriority = pPriority;
		// the priority of the sender thread schuld be lower than that of the
		// reader one
		if (mPriority > 0)
			--mPriority;
		struct sched_param tParam;
		memset(&tParam, 0, sizeof(tParam));
		sched_getparam(0, &tParam);
		tParam.sched_priority = pPriority;
		sched_setscheduler(0, SCHED_RR, &tParam);

		if (mPriority >= 0)
			++mPriority;
		// set priority for main thread as well as to the reader thread
		pthread_attr_setschedpolicy(&tAttr, SCHED_RR);
		memset(&tParam, 0, sizeof(tParam));
		pthread_attr_getschedparam(&tAttr, &tParam);
		tParam.sched_priority = pthread_attr_setschedparam(&tAttr, &tParam);
	}
	else
	{

		tAttr_ptr = NULL;
	}

	pthread_create(&tID, tAttr_ptr, &fifo_reader, (void*) this);

	mFinishedM = ( pthread_mutex_t
			) PTHREAD_MUTEX_INITIALIZER;
	mPDUQueueM = ( pthread_mutex_t
			) PTHREAD_MUTEX_INITIALIZER;

}

/// dtor
CFIFOSource::~CFIFOSource()
{
	if (mFifoStream_ptr != 0)
	{
		delete mFifoStream_ptr;
	}
	mFifoStream_ptr = 0;
}

/**
 dispatch routine for sending data packets on socket, returns true if
 if packet sent, false otherwise
 */bool CFIFOSource::Dispatch()
{
	if ((mPackets != 0 && mSentPacketStat >= mPackets)
			|| (mBytes != 0 && mSentBytesStat >= mBytes)
			|| (mBytes == 0 && mPackets == 0 && (!mFinishTime.IsNil())
					&& HPTimer::HighPerTimer::Now() >= mFinishTime))
	{
		pthread_mutex_lock(&mFinishedM);
		mFinished = true;
		pthread_mutex_unlock(&mFinishedM);
	}
	if (mFinished)
	{
		// store real finish time
		mFinishTime.SetNow();
		return false;
	}
	/** test next sending time, assemble and send pdu,
	 * get next pdu description, set next pdu time
	 * test constructors */

	pthread_mutex_lock(&mPDUQueueM);
	if (!mPDU_Queue.empty())
	{
		mNextPDU_ptr = &(mPDU_Queue.front());
		if (HPTimer::HighPerTimer::Now() >= mNextPDU_ptr->SendT)
		{
			unsigned long Length = mNextPDU_ptr->Length;
			pthread_mutex_unlock(&mPDUQueueM);
			// prepare and send a PDU
			AssemblePDU(Length);
			mSocket->Send(mBuff, Length);
			// store the PDU to the header vector
			mHeaderVec.push_back(mBuff);

			// set session start time to now(), if first packet
			if (mSentPacketStat == 0)
			{
				mStartTime.SetNow();
			}
			mSentPacketStat++;
			mSentBytesStat += Length;
			pthread_mutex_lock(&mPDUQueueM);
			mPDU_Queue.pop();
			pthread_mutex_unlock(&mPDUQueueM);
			return true;
		}
	}
	else
	{
		mNextPDU_ptr = 0;
		pthread_mutex_lock(&mFinishedM);
		// if reader finished and queue ist empty, finishes the source
		if (mReaderFinished == true)
		{
			mFinished = true;
			// store real finish time
			mFinishTime.SetNow();
		}
		pthread_mutex_unlock(&mFinishedM);
	}
	pthread_mutex_unlock(&mPDUQueueM);
	return false;
}

/// routine for parsing FIFO pipe and stroring results in the PDU Queue,
PDU_Desc CFIFOSource::ParseLine(std::string & pLine)
{
	CStringTokenizer tok(pLine, "|");
	if (tok.countTokens() != 3)
	{
		std::cerr << "parse error in FIFO source !!!" << std::endl;
		exit(-1);
	}
	unsigned int sec, usec, length;
	sec = CStringUtils::stoi(tok.nextToken());
	usec = CStringUtils::stoi(tok.nextToken());
	length = CStringUtils::stoi(tok.nextToken());
	HPTimer::HighPerTimer Time(static_cast<int64_t>(sec),
			static_cast<int64_t>(usec) * 1000);
	PDU_Desc PDU(Time, length);
	return PDU;
}

/**
 routine for reading FIFO line by line
 as this routine blocks during reading, it will run as separate high
 priority thread
 */
void CFIFOSource::ReadFifo()
{
	char Buff[1024];
	int EOFCounter(0);
	int usleepTime(10000);
	HPTimer::HighPerTimer hptimer;
	memset(Buff, 0, sizeof(Buff));
	hptimer.USecSleep(100000);
	//usleep ( 100000 );
	while (!mFifoStream_ptr->bad())
	{
		pthread_mutex_lock(&mFinishedM);
		if (mFinished)
		{
			break;
		}
		if (mFifoStream_ptr->eof())
		{
			hptimer.USecSleep(usleepTime);
			//usleep ( usleepTime );
			EOFCounter++;
			usleepTime *= 2;
			if (EOFCounter > 7)
			{
				break;
			}
			mFifoStream_ptr->clear();
		}
		pthread_mutex_unlock(&mFinishedM);
		mFifoStream_ptr->getline(Buff, sizeof(Buff) - 1);

		std::string Line(Buff);
		if (Line.length() != 0)
		{
			EOFCounter = 0;
			pthread_mutex_lock(&mPDUQueueM);
			mPDU_Queue.push(ParseLine(Line));
			pthread_mutex_unlock(&mPDUQueueM);
			mLines++;
		}
	}

	if (mFifoStream_ptr->eof())
	{
		std::cerr << " end of file occured" << std::endl;
	}
	if (mFifoStream_ptr->bad())
	{
		std::cerr << "file stream is bad !" << std::endl;
	}
	pthread_mutex_unlock(&mFinishedM);
	pthread_mutex_lock(&mFinishedM);
	mReaderFinished = true;
	pthread_mutex_unlock(&mFinishedM);
	std::cout << "Read " << mLines << " lines from the FIFO file" << std::endl;
}

/// print out the data source description
std::string CFIFOSource::PrintSDesc()
{
	std::stringstream Str;
	Str << "Session start time:\t" << mStartTime << "\n";
	Str << "Session duration:\t" << mDuration << "\n";
	Str << "Session finish time:\t" << mFinishTime << "\n";
	std::string Proto("UDP");
	if (mProto == P_TCP)
	{
		Proto = "TCP";
	}
	else if (mProto == P_SDP)
	{
		Proto = "SDP";
	}
	Str << "Transport protocol:\t" << Proto << "\n";
	Str << "FIFO name:\t" << mFifoName << "\n";
	return Str.str();
}

/// ctor  for dummy Source
CDummySource::CDummySource() :
		CDataSource()
{
}

/// dtor
CDummySource::~CDummySource()
{
}

/// do nothing, only for interface conformance
bool CDummySource::Dispatch()
{
	return true;
}

/// dummy source description, noting to describa
std::string CDummySource::PrintSDesc()
{
	return (std::string) "This is only a dummy for gathering statistics, no real packets will be sent\n";

}

/// store received sample to the source Headers array
void CDummySource::StoreSample(char * pHeader)
{
	CPDUHeader Header;
	memcpy(Header.Header, pHeader, 24);
	mHeaderVec.push_back(Header);
}

/// imports Basic Data source statistics from an external source
void CDummySource::ImportBasicStats(BasicSourceStats_t & pStats)
{
	mStartTime = HPTimer::HighPerTimer(
			static_cast<int64_t>(pStats.StartTime.tv_sec),
			static_cast<int64_t>(pStats.StartTime.tv_usec * 1000));
	mDuration = HPTimer::HighPerTimer(
			static_cast<int64_t>(pStats.Duration.tv_sec),
			static_cast<int64_t>(pStats.Duration.tv_usec) * 1000);

	mFinishTime = HPTimer::HighPerTimer(
			static_cast<int64_t>(pStats.FinishTime.tv_sec),
			static_cast<int64_t>(pStats.FinishTime.tv_usec) * 1000);
	mDuplicates = pStats.Duplicates;
	mCorrupted = pStats.Corrupted;
	mReordered = pStats.Reordered;
	mSentPacketStat = pStats.SentPackets;
	mSentBytesStat = pStats.SentBytes;
	mRecvPacketStat = pStats.RecvPackets;
	mRecvBytesStat = pStats.RecvBytes;
	mDeltaTime = HPTimer::HighPerTimer::NSectoHPTimer(pStats.DeltaTime * 1000);
	return;
}

/**
 Initialize data source. Essentially set up the parant DataSource class
 */
CFullSpeedSource::CFullSpeedSource(unsigned int pLocalAddr,
		unsigned short pLocalPort, unsigned int pRemoteAddr,
		unsigned short pRemotePort, unsigned int pBytes, unsigned int pPackets,
		proto_t pProto, unsigned int pSize, HPTimer::HighPerTimer & pStartTime,
		HPTimer::HighPerTimer & pDuration, HPTimer::HighPerTimer & pFinishT,
		int pPriority, unsigned char pTOS, SockOptsMap pOptsMap) :
		CDataSource(pLocalAddr, pLocalPort, pRemoteAddr, pRemotePort, pBytes,
				pPackets, pProto, pStartTime, pDuration, pFinishT, pPriority,
				pTOS, pOptsMap)
{

	// for debugging purposes only
#ifdef DEBUG_OUTPUT
	std::cout << "DataSource parameters:\n"
	<< "pLocalAddr " << CIPAddress::atodd ( pLocalAddr ) << ":" << pLocalPort << "\n"
	<< "pRemoteAddr " << CIPAddress::atodd ( pRemoteAddr ) << ":" << pRemotePort << "\n"
	<< "pBytes " << pBytes << "\n"
	<< "pPackets " << pPackets << "\n"
	<< "pProto " << pProto << "\n"
	<< "pRate " << "infinite" << "\n"
	<< "pSize " << pSize << "\n"
	<< "pStartTime " << pStartTime << "\n"
	<< "pDuration" << pDuration << "\n"
	<< "pFinishTime " << pFinishT << "\n"
	<< "pPriority " << pPriority << "\n"
	<< "pTOS " << pTOS << "\n"
	<< "__ End of DataSource parameters ___" << std::endl;
#endif

	// in case of TCP reallocate the mBuff memory for very big PDUs (up to 64 MBytes)
	delete[] mBuff;
	mBuff = new char[TCP_PERFORMANCE_SCALE * MAX_IP_SIZE];

	// adjust the size to by within the maximum/minimum boundary size
	mSize = pSize > 16 ? pSize : 16;
	mSize = pSize > MAX_IP_SIZE ? MAX_IP_SIZE : pSize;

	if (mPackets != 0)
	{
		mHeaderVec.reserve(mPackets);
		// finish time shold be in the very far future (for avoiding exceptions)
		mFinishTime = HPTimer::HighPerTimer::Nil();
		//mFinishTime.SecAdd ( 0xFFFFFFFFFFFFFFFF );
		//mFinishTime.SecAdd ( HPTimer::HighPerTimer::HPTimer_MAX.Seconds());
		mFinishTime = HPTimer::HighPerTimer::HPTimer_MAX;
	}
	else if (mBytes != 0)
	{
		mHeaderVec.reserve(mBytes / mSize + 1);
		// finish time shold be in the very far future (for avoiding exceptions)
		mFinishTime = HPTimer::HighPerTimer::Nil();
		//mFinishTime.SecAdd ( 0xFFFFFFFFFFFFFFFF );
		//mFinishTime.SecAdd ( HPTimer::HighPerTimer::HPTimer_MAX.Seconds());
		mFinishTime = HPTimer::HighPerTimer::HPTimer_MAX;

	}
	// we assume, at least 500 000 packets will be transmitted otherwise
	else
	{
		mHeaderVec.reserve(500000);
	}

	// set the priority of the data source thread
	if (getuid() == 0)
	{
		pPriority == -1 ? sched_get_priority_min(SCHED_RR) : pPriority;
		mPriority = pPriority;
		struct sched_param tParam;
		memset(&tParam, 0, sizeof(tParam));
		sched_getparam(0, &tParam);
		tParam.sched_priority = pPriority;
		sched_setscheduler(0, SCHED_RR, &tParam);
	}
}

/**
 dtor
 */
CFullSpeedSource::~CFullSpeedSource()
{
}

/**
 dispatch routine for CBR source, return true, if packet sent, false
 otherwise. NOTE!. The finish test is performed only all 100 data packets.
 So if you advice to send 101 data packets, 200 will really be sent.
 */bool CFullSpeedSource::Dispatch()
{
	if ((mSentPacketStat % 100) == 0)
	{
		if ((mPackets != 0 && mSentPacketStat >= mPackets)
				|| (mBytes != 0 && mSentBytesStat >= mBytes)
				|| (mBytes == 0 && mPackets == 0
						&& HPTimer::HighPerTimer::Now() >= mFinishTime))
		{
#ifdef DEBUG_OUTPUT
			std::cerr << "Data source packet statistics debug :\n"
			<< "mPackets " << mPackets << "\n"
			<< "mSentPacketStat " << mSentPacketStat << "\n"
			<< "mBytes " << mBytes << "\n"
			<< "mSentBytesStat " << mSentBytesStat << "\n..." << std::endl;

#endif

			mFinished = true;
		}
	}

	if (mFinished)
	{
		// store real finish time
		mFinishTime.SetNow();
		// mSentPacketStat = mHeaderVec.size();
		return false;
	}
	// prepare and send a PDU
	AssemblePDU(mSize);
	mSocket->Send(mBuff, mSize);
	// store the PDU to the header vector
	mHeaderVec.push_back(mBuff);

	mSentBytesStat += mSize;
	mSentPacketStat++;
	return true;
}

/**
 print out the data source description
 */
std::string CFullSpeedSource::PrintSDesc()
{
	std::stringstream Str;
	Str << "Type: FullSpeed source\n";
	Str << "Session start time:\t" << mStartTime << "\n";
	Str << "Session duration, s:\t" << mDuration << "\n";
	Str << "Session finish time:\t" << mFinishTime << "\n";
	std::string Proto("UDP");
	if (mProto == P_TCP)
	{
		Proto = "TCP";
	}
	else if (mProto == P_SDP)
	{
		Proto = "SDP";
	};
	Str << "Transport protocol:\t" << Proto << "\n";
	Str << "Sending data rate, bit/s:\t" << "infinite" << "\n";
	Str << "Packet size, Bytes:\t" << CStringUtils::decades_formatted(mSize)
			<< "\n";
	return Str.str();
}

/// Data Receiver base class ctor
CDataReceiver::CDataReceiver(CIPSocket * pSocket, long pSampleNum) :
		mRHeaderContainer(pSampleNum), mSocket(pSocket), mBytesRecv(0)
{
	// cerr << __PRETTY_FUNCTION__ << endl;

}

/// Data Receiver base class dtor
CDataReceiver::~CDataReceiver()
{
	// cerr << __PRETTY_FUNCTION__ << endl;
	std::cout << mRHeaderContainer.size() << " Packets received" << std::endl;

}

/**
 save relevant header fields as well as the  receiving timestamp to
 the vector
 */
void CDataReceiver::SaveHeader(char * pHeader)
{
	struct timeval now;
	//gettimeofday ( &now, NULL ); // TODO: optimize code here!!!!
	HPTimer::HighPerTimer HPnow;
	HPnow = HPTimer::HighPerTimer::Now();
	HPnow.SetTV(now);

	mRHeader.SetHeader(pHeader + 4, now);
	mRHeaderContainer.saveHeader(mRHeader);

}
;

/**
 wait for the last sent data packet, but maximal timeout time
 after that
 */
void CDataReceiver::Freeze(unsigned long pLastSeqNo,
		HPTimer::HighPerTimer & pBackLogT)
{
	if (pBackLogT.Negative())
	{
		pBackLogT = pBackLogT.InvertSign();
	}
	HPTimer::HighPerTimer TimeOut(HPTimer::HighPerTimer::Now() + pBackLogT);
	if (this == 0)
	{
		return;
	}
	// if the socket is connected, receive the data till timeout or LastSeqNo received
	TimeOut.SleepTo(TimeOut);
	if (mSocket->ConnectedFrom())
	{
		this->Recv();
		if (mRHeaderContainer.size() > 0
				&& mRHeaderContainer.at(mRHeaderContainer.size() - 1).SeqNo()
						>= pLastSeqNo)
		{

			return;
		}
	}

}

/// get next data packet header entry
CRPDUHeader *
CDataReceiver::getNextHeader()
{
	return mRHeaderContainer.getNextHeader();
}

/// ctor, for UDP receiver
CUDPReceiver::CUDPReceiver(CIPSocket * pSocket, long pSampleNum) :
		CDataReceiver(pSocket, pSampleNum)
{
}

/// dtor:
CUDPReceiver::~CUDPReceiver()
{
}

/// Reassembling routine for UDP
long CUDPReceiver::Recv()
{
	unsigned long bytes(mSocket->Recv(mBuffer, sizeof(mBuffer)));

	if (bytes > 0)
	{
		// get receiving time, store packet statistics
		this->SaveHeader(mBuffer);
		mBytesRecv += bytes;

	}
	return bytes;
}

/// ctor, for TCP receiver
CTCPReceiver::CTCPReceiver(CIPSocket * pSocket, long pSampleNum,
		unsigned long pMaxPDU) :
		CDataReceiver(pSocket, pSampleNum), mRingBuffer(4 * pMaxPDU, pMaxPDU), mPacketSize(
				0), mMaxPDU(pMaxPDU)
{
	mTCPSegBuffer = new char[pMaxPDU];
	// cerr << __PRETTY_FUNCTION__ << endl;
}

/// dtor:
CTCPReceiver::~CTCPReceiver()
{
	delete[] mTCPSegBuffer;
}

/// Reassembling routine for TCP, returns the length of received packet
long CTCPReceiver::Recv()
{
	// first, read a PDU from the ring buffer
	int result;

	if ((result = mRingBuffer.Read(mPDUHeader, 16)) == 16)
	{
		mPacketSize = ntohl(*((unsigned long *) mPDUHeader));

		// try to fetch and delete the whole PDU
		int result;
		if ((result = mRingBuffer.Fetch(mPDUHeader, mPacketSize, false, true))
				> 0)
		{
			this->SaveHeader(mPDUHeader);
			return result;
		}
		else if (result == -1)
		{
			std::cerr
					<< " ERROR while fetching LTest-PDU from the tcp desegmentation buffer"
					<< std::endl;
			return -1;
		}
	}
	else if (result == -1)
	{
		std::cerr << "ERORR while reading from  TCP desegmenation buffer"
				<< std::endl;
		return -1;
	}

	// read from socket
	long bytes(mSocket->Recv(mTCPSegBuffer, mMaxPDU));
	if (bytes > 0)
	{
		mBytesRecv += bytes;
		if (mRingBuffer.Store(mTCPSegBuffer, bytes) == -1)
		{
			std::cerr
					<< "ERROR: Buffer overrun while storing segment in the TCP desegmentation buffer"
					<< std::endl;
		}

	}
	else if (bytes == -1)
	{
		// cerr << "*** Socket closed on the sender side" << endl;
		HPTimer::HighPerTimer hptimer;
		hptimer.USecSleep(200000);
		//usleep ( 200000 );
	}
	return 0;
}

CUDTReceiver::CUDTReceiver(CIPSocket * pSocket, long pSampleNum,
		unsigned long pMaxPDU) :
		CTCPReceiver(pSocket, pSampleNum, pMaxPDU)
{
}

CUDTReceiver::~CUDTReceiver()
{
}

CFastTCPReceiver::CFastTCPReceiver(CIPSocket * pSocket, long pSampleNum,
		unsigned long pMaxPDU) :
		CTCPReceiver(pSocket, pSampleNum, pMaxPDU)
{
	// cerr << __PRETTY_FUNCTION__ << endl;
}

CFastTCPReceiver::~CFastTCPReceiver()
{
}

CHSTCPReceiver::CHSTCPReceiver(CIPSocket * pSocket, long pSampleNum,
		unsigned long pMaxPDU) :
		CTCPReceiver(pSocket, pSampleNum, pMaxPDU)
{
	// cerr << __PRETTY_FUNCTION__ << endl;
}

CHSTCPReceiver::~CHSTCPReceiver()
{
}

CSDPReceiver::CSDPReceiver(CIPSocket * pSocket, long pSampleNum,
		unsigned long pMaxPDU) :
		CTCPReceiver(pSocket, pSampleNum, pMaxPDU)
{
	// cerr << __PRETTY_FUNCTION__ << endl;
}

CSDPReceiver::~CSDPReceiver()
{
}

/// Header Container handling, ctor, initialize the header container
CRHeaderContainer::CRHeaderContainer(long long pPreallocElements) :
		mWVectorIdx(0), mWArrayIdx(0), mRVectorIdx(0), mRArrayIdx(0), mSize(0)
{
	long long NumSlots(1);
	// prealloc
	if (0 < pPreallocElements)
	{
		NumSlots = ceil(
				static_cast<double>(pPreallocElements)
						/ static_cast<double>(BUFFER_CONTAINER_SIZE));
	}

	for (; NumSlots > 0; NumSlots--)
	{
		RPDUHeaderArray = new CRPDUHeader[BUFFER_CONTAINER_SIZE];
		if (NULL == RPDUHeaderArray)
		{
			std::cerr << "couldn't pre allocate receiver PDU headers"
					<< std::endl;
			break;
		}
		mHeaderVec.push_back(RPDUHeaderArray);
	}
}

/// dtor, header Container handling, dtor, free the buffer
CRHeaderContainer::~CRHeaderContainer()
{
	PDUHeaderIterator it(mHeaderVec.begin());
	int counter(0);
	for (; it != mHeaderVec.end(); it++)
	{
		counter++;
		delete[] (*it);
	}
}

/// store given buffer
void CRHeaderContainer::saveHeader(CRPDUHeader & pHeader)
{
	if (mWArrayIdx == BUFFER_CONTAINER_SIZE)
	{
		if (mHeaderVec.size() * BUFFER_CONTAINER_SIZE == mSize)
		{
			std::cerr << "NOTICE: receiver header counter is growing"
					<< std::endl;
			RPDUHeaderArray = new CRPDUHeader[BUFFER_CONTAINER_SIZE];
			mHeaderVec.push_back(RPDUHeaderArray);
		}
		mWVectorIdx++;
		mWArrayIdx = 0;
	}
	mHeaderVec[mWVectorIdx][mWArrayIdx++] = pHeader;
	mSize++;
}

/// get next packet in the buffer
CRPDUHeader *
CRHeaderContainer::getNextHeader()
{
	// test, if the last element arrived
	if (mRVectorIdx == mWVectorIdx && mRArrayIdx == mWArrayIdx)
	{
		return NULL;
	}
	if (mRArrayIdx == BUFFER_CONTAINER_SIZE)
	{
		mRVectorIdx++;
		mRArrayIdx = 0;
	}
	return &(mHeaderVec[mRVectorIdx][mRArrayIdx++]);
}

/// get packet size
unsigned int CRHeaderContainer::size()
{
	// return ( ( mHeaderVec.size() - 1 ) * BUFFER_CONTAINER_SIZE + mWArrayIdx );
	return mSize;
}

// access the n-th element, begins with 0
CRPDUHeader &
CRHeaderContainer::at(unsigned int pPos)
{
	int VectorIdx = pPos / BUFFER_CONTAINER_SIZE;
	int ArrayIdx = pPos % BUFFER_CONTAINER_SIZE;
	return (mHeaderVec[VectorIdx][ArrayIdx]);
}

/**
 ctor
 */
CPoissonSource::CPoissonSource(unsigned int pLocalAddr,
		unsigned short pLocalPort, unsigned int pRemoteAddr,
		unsigned short pRemotePort, unsigned int pBytes, unsigned int pPackets,
		proto_t pProto, unsigned long long pRate, unsigned int pSize,
		HPTimer::HighPerTimer & pStartTime, HPTimer::HighPerTimer & pDuration,
		HPTimer::HighPerTimer & pFinishT, int pPriority, unsigned char pTOS,
		SockOptsMap pOptsMap) :
		CDataSource(pLocalAddr, pLocalPort, pRemoteAddr, pRemotePort, pBytes,
				pPackets, pProto, pStartTime, pDuration, pFinishT, pPriority,
				pTOS, pOptsMap), mRate(pRate), mNextTime(mStartTime), mInterPacketTics(
				0)
{
	// adjust the size to by within the maximum/minimum boundary size
	mSize = pSize > 16 ? pSize : 16;
	mSize = pSize > MAX_IP_SIZE ? MAX_IP_SIZE : pSize;

	// set mNextTimer to the start time
	mNextTime = mStartTime;
	if (mRate != 0)
	{
		mInterPacketTics = static_cast<long long>(mSize) * 8 * 1000000
				* HPTimer::HighPerTimer::GetHPFrequency() / mRate;
	}

	mLambda = mRate / (8 * mSize);

	// initialize the vector with sent pdu headers
	if (!mBytes)
	{
		HPTimer::HighPerTimer duration(mFinishTime - mStartTime);

		if (mInterPacketTics != 0)
		{
			unsigned long packets(duration.HPTics() / mInterPacketTics);
			mHeaderVec.reserve(packets + 2);

		}
	}
	else
	{
		// finish time shold be in the very far future (for avoiding exceptions)
		// mFinishTime = HPTimer::HighPerTimer (  static_cast<int32_t>(0xFFFFFFFF), //fedotova
		mFinishTime = HPTimer::HighPerTimer(
				HPTimer::HighPerTimer::HPTimer_MAX.Seconds(),
				static_cast<int32_t>(0u), false);
		if (pSize != 0)
		{
			unsigned long packets = mBytes / pSize + 1;
			mHeaderVec.reserve(packets + 2);
		}
	}
	if (getuid() == 0)
	{
		// set the priority of the data source thread
		pPriority == -1 ? sched_get_priority_min(SCHED_RR) : pPriority;
		mPriority = pPriority;
		struct sched_param tParam;
		memset(&tParam, 0, sizeof(tParam));
		sched_getparam(0, &tParam);
		tParam.sched_priority = pPriority;
		sched_setscheduler(0, SCHED_RR, &tParam);
	}
}

/// dtor
CPoissonSource::~CPoissonSource()
{
}

/**
 dispatch routine for CBR source, return true, if packet sent, false
 otherwise
 */bool CPoissonSource::Dispatch()
{
	if ((mBytes == 0 && HPTimer::HighPerTimer::Now() >= mFinishTime)
			|| (mBytes != 0 && mSentBytesStat >= mBytes)
			|| (mPackets == 0 && mBytes == 0
					&& HPTimer::HighPerTimer::Now() >= mFinishTime))
	{
		mFinished = true;
	}

	if (mFinished)
	{
		// store real finish time
		mFinishTime.SetNow();
		return false;
	}
	if (HPTimer::HighPerTimer::Now() >= mNextTime)
	{
		// prepare and send a PDU
		AssemblePDU(mSize);
		mSocket->Send(mBuff, mSize);
		// store the PDU to the header vector
		mHeaderVec.push_back(mBuff);

		mSentBytesStat += mSize;
		mSentPacketStat++;
		calcNextTime(mNextTime);
		return true;
	}
	return false;
}

/// print out the data source description
std::string CPoissonSource::PrintSDesc()
{
	std::stringstream Str;
	Str << "Poisson source description" << std::endl;
	Str << "Session start time:\t" << mStartTime << "\n";
	Str << "Session duration:\t" << mDuration << "\n";
	Str << "Session finish time:\t" << mFinishTime << "\n";
	std::string Proto("UDP");
	if (mProto == P_TCP)
	{
		Proto = "TCP";
	}
	else if (mProto == P_SDP)
	{
		Proto = "SDP";
	}
	Str << "Transport protocol:\t" << Proto << "\n";
	Str << "Sending data rate, bit/s:\t"
			<< CStringUtils::decades_formatted(mRate, ",") << "\n";
	Str << "Packet size, Bytes:\t"
			<< CStringUtils::decades_formatted(mSize, ",") << std::endl;
	return Str.str();
}

/// calculate the time for next packet

HPTimer::HighPerTimer &
CPoissonSource::calcNextTime(HPTimer::HighPerTimer & Now)
{

	// Random PacketInterval (0-1000000 usec)

	double rnd = (double) rand() / RAND_MAX;
	rnd = (-1) * log(rnd) / mLambda;
	// translate rnd from seconds to TSC tics
	rnd *= 1000000 * HPTimer::HighPerTimer::GetHPFrequency();
	mInterPacketTics = static_cast<long long>(rnd);

	return Now.TicAdd(mInterPacketTics);
}

///////////////////////////////////////////////////////////////////////////
// wrapper for starting line reading tread
///////////////////////////////////////////////////////////////////////////
/*extern "C" {
 void *fifo_reader(void *pClass){
 cout << "starting FIFO reader in a new thread ..." << endl;
 ((CFIFOSource *)pClass)->ReadFifo();
 cout << "finishes FIFO reader thread..." << endl;
 return pClass;
 }
 }
 */
//////////////==CPoisson==/////////////////////////////////////////////////////
//////////////==CPoisson==/////////////////////////////////////////////////////
//////////////==CPoisson==/////////////////////////////////////////////////////
