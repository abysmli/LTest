// Author:  Author Eduard Siemens <eduard.siemens@thomson.net>
//
// File:    TimeCorrect.cpp
//          Thu Dec 1 2005
//
// $Author:   siemense  $
// $Date:   03 Apr 2007 16:11:14  $
// $File$
// Copyright (C) 2004, 2005 Eduard Siemens All Rights Reserved.
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
// $Log:   //hanvsfile02/prog/Projects/2005/hipernet/Repository/hipernet/archives/HiperNet/Linux/Apps/ltest/TimeCorrect.cpp-arc  $
//
//   Rev 1.3.1.7   03 Apr 2007 16:11:14   siemense
//. 
//
//   Rev 1.3.1.6   03 Apr 2007 13:42:18   siemense
//GetTime stalling buf fixed
//
//   Rev 1.3.1.5   31 Jan 2007 09:11:36   siemense
//Efficiency issues solved. Interfaces to network classes has been cleaned up.
//
//   Rev 1.3.1.4   21 Dec 2006 14:15:24   siemense
//CIPSocket extended by BRecv and TRecv (not yet testsed)
//
//   Rev 1.3.1.3   13 Nov 2006 11:58:20   siemense
//Minor bug fixes.
//Version before CIPNetworkPerformance tunings.
//
//   Rev 1.3.1.1   17 Aug 2006 13:21:16   siemense
//TSC timer class has been implemented and rudimentary tested
//
//   Rev 1.3.1.0   Jun 27 2006 10:40:22   siemense
//New Protocols Branch
//
//   Rev 1.3   21 Jun 2006 13:51:46   siemense
//Improved Cristian Algorithm implemented properly
//
//   Rev 1.2   21 Jun 2006 13:16:08   siemense
//Cristian Algorithm corrected
//
//   Rev 1.1   27 Feb 2006 12:18:38   siemense
//Timesync in old fashion (Christian algorithm) added
//
//   Rev 1.0   02 Feb 2006 12:53:04   siemense
//Initial revision.
//
//   Rev 1.1   21 Dec 2005 16:42:32   siemense
//Time correction between data sender and data recei ver added
//
//   Rev 1.0   02 Dec 2005 12:59:56   siemense
//Initial revision.
//
//

#include <sys/stat.h>
#include <fcntl.h>
#include "strtok.h"
#include <unistd.h>
#include "TimeCorrect.h"

/**
 time correction struct handling routines
 */
//FIXME Modified casting of MinDevF, MinDevB, MinRTT
TCorrStats::TCorrStats() :
		Tries(0), ConsNumber(0), MaxRTT(static_cast<int64_t>(0),
				static_cast<int64_t>(0)), MaxRTTPos(-1), MaxDevF(
				static_cast<int64_t>(0), static_cast<int64_t>(0)), MaxDevB(
				static_cast<int64_t>(0), static_cast<int64_t>(0)), MinDevF(
				static_cast<int64_t>(10), static_cast<int64_t>(10000)), MinDevB(
				static_cast<int64_t>(10), static_cast<int64_t>(10000)), MinRTT(
				static_cast<int64_t>(10), static_cast<int64_t>(10000)), MinRTTPos(
				-1), MinF(static_cast<int64_t>(0xffffffffffffffff),
				static_cast<int64_t>(0)), MinFPos(-1), MinB(
				static_cast<int64_t>(0xffffffffffffffff),
				static_cast<int64_t>(0)), MinBPos(-1), VirtMinRTT(
				static_cast<int64_t>(0xffffffffffffffff),
				static_cast<int64_t>(0)), ICADelta(
				static_cast<int64_t>(0xffffffffffffffff),
				static_cast<int64_t>(0)), CAMinDelta(
				static_cast<int64_t>(0xffffffffffffffff),
				static_cast<int64_t>(0)), CADelta(
				static_cast<int64_t>(0xffffffffffffffff),
				static_cast<int64_t>(0))
{
}

// print out the correction statistics struct
std::ostream &
TCorrStats::print_out(std::ostream & pStr) const
{
	pStr << "time syncrhonization start time:\t" << StartTime << " ( "
			<< StartTime.PrintTime(false) << " )\n"
			<< "time synchronization finish time:\t" << FinishTime << " ( "
			<< FinishTime.PrintTime(false) << " )\n"
			<< "performed time tries:\t" << Tries << "\n"
			<< "number of consecutive tests withhin the deviation bound:\t"
			<< ConsNumber << "\n" << "maximum RTT:\t" << MaxRTT
			<< " expirienced at the position:\t" << MaxRTTPos << "\n"
			<< "maximum experienced deviation in forward direction:\t"
			<< MaxDevF << "\n"
			<< "maximum experienced deviation in backward direction:\t"
			<< MaxDevB << "\n"
			<< "minimum experienced deviation in forward direction:\t"
			<< MinDevF << "\n"
			<< "minimum experienced deviation in backward direction:\t"
			<< MinDevB << "\n" << "minimum RTT:\t" << MinRTT
			<< " experienced at the position:\t" << MinRTTPos << "\n"
			<< "calculated ICA delta:\t" << ICADelta << "\n" <<"send time of ICA delta:\t"<<ICADeltaSendTime<<"\n"<< "minF:\t"
			<< MinF << " at position:\t" << MinFPos << "\n" << "minB:\t" << MinB
			<< " at position:\t" << MinBPos << "\n" << "Virtual minimum RTT:\t"
			<< VirtMinRTT << "\n" << "Sigma_1:\t" << Sigma_1 << "\n"
			<< "Sigma_2:\t" << Sigma_2 << "\n" << std::endl;
	return pStr;
}

//print out extended data statistics
std::ostream &
TCorrStats::print_out_ext(std::ostream & pStr) const
{
	print_out(pStr);

	pStr << "\n" << "Extended time correction parameters\n"
			<< "calculated CA delta on the first sample:\t" << CADelta << "\n"
			<< "calculated CA delta on minRTT sample:\t" << CAMinDelta << "\n"
			<< "SigmaCA:\t" << SigmaCA << "\n" << "SigmaCAMin:\t" << SigmaCAMin
			<< std::endl;
	return pStr;

}

CMeasuredTimes::CMeasuredTimes()
{
}

CMeasuredTimes::~CMeasuredTimes()
{
}

// get measured forward time
HPTimer::HighPerTimer CMeasuredTimes::ForwardTime() const
		throw (CTimeCorrect_exception)
{
	// throw excstpions if one of the timers not set
	if (ST == HPTimer::HighPerTimer::Nil())
	{
		throw CTimeCorrect_exception("sender time not set", 2);
	}
	if (RT == HPTimer::HighPerTimer::Nil())
	{
		throw CTimeCorrect_exception("receiver time not set", 3);
	}

	return RT - ST;
}

// get measured backward time
HPTimer::HighPerTimer CMeasuredTimes::BackwardTime() const
		throw (CTimeCorrect_exception)
{
	// throw excstpions if one of the timers not set
	if (RT == HPTimer::HighPerTimer::Nil())
	{
		throw CTimeCorrect_exception("receiver time not set", 3);
	}
	if (RRT == HPTimer::HighPerTimer::Nil())
	{
		throw CTimeCorrect_exception("received time (at the sender) not set",
				4);
	}
	return RRT - RT;
}

// calculate the RTT from the forward and backward times
HPTimer::HighPerTimer CMeasuredTimes::RTTime() const
		throw (CTimeCorrect_exception)
{
	/* return (ForwardTime() + BackwardTime()); */
	return (RRT - ST);
}

// ctor
CTimeCorrection::CTimeCorrection() :
		mMinConsecResults(5), mMaxTries(100), mMaxDev(0), mTimeout(0), mWithinBound(
				false), mProxy(NULL), mSampleCount(0)
{
}

// ctor
CTimeCorrection::CTimeCorrection(ltestProxy *pProxy, unsigned int pMaxDev,
		unsigned int pMinConsecResults, unsigned int pMaxTries,
		unsigned int pTimeout) :
		mMinConsecResults(pMinConsecResults), mMaxTries(pMaxTries), mMaxDev(
				static_cast<int64_t>(0), static_cast<int64_t>(pMaxDev) * 1000), mWithinBound(
				false), mProxy(pProxy), mSampleCount(0)

{
	mTimeout.SetUSeconds(pTimeout * 1000);
}

// dtor
CTimeCorrection::~CTimeCorrection()
{
}

/*
 the time correction implementation
 the CMeasuredTimes saves the three timestamps - sending time, receiver time and the time, the answer
 packet received (recv time)
 */bool CTimeCorrection::PerformCorrection()
{
	// counter for consecutive timers within the MaxDev range
	unsigned int consecCounter(0);
	unsigned int consecNErrors(0);
	// run throw the delay gathering
	CMeasuredTimes times;

	mCorrStats.StartTime = HPTimer::HighPerTimer::Now();
	for (unsigned int i(0); i < mMaxTries; i++)
	{
		mSampleCount++;
		// get actual time
		times.ST = HPTimer::HighPerTimer::Now();
		try
		{
			times.RT = GetTime(times.ST, times.RT, i);
			times.RRT = HPTimer::HighPerTimer::Now();
			std::cerr << "." << std::flush;
		} catch (CNetwork_exception & ex)
		{
			// till 3 consecutive timeout errors are accepted. In that case, only return false because other
			// should be performed
			if (ex.code() == 10)
			{
				if (consecNErrors++ >= 3)
				{
					std::cerr << std::endl;
					std::string emsg(
							ex.what()
									+ std::string(
											" This error occured at least 3 times"));
					std::cerr << emsg << " Code: " << ex.code() << std::endl;
					return false;
				}
				else
				{
					// reset consecCounter and go to the next iteration step
					consecCounter = 0;
					continue;
				}
			}
			else
			{
				// other network exceptions should not be filtered - we don't know what to do then
				throw;
			}
		} // -- end of catch CNetwork_exception

		mCorrStats.FinishTime = HPTimer::HighPerTimer::Now();
		consecNErrors = 0;

		// save result to the vector
		if (mTimerV.empty())
		{
			mTimerV.push_back(times);
			mCorrStats.Tries = 1;
			continue;
		}
		// test if within the max deviation range and count or reset consecCounter
		if (TestDeviation(times, mTimerV.back()))
		{
			consecCounter++;
			mCorrStats.ConsNumber = consecCounter;
		}
		else
		{
			consecCounter = 0;
		}
		mTimerV.push_back(times);

		mCorrStats.Tries = i + 1;

		if (consecCounter >= mMinConsecResults)
		{
			mWithinBound = true;
			std::cerr << std::endl;
			return true;
		}
	} // --end for
	std::cerr << std::endl;
	return false;
}

// calculate deviation using improved Christian Algorithm (ICA)
HPTimer::HighPerTimer CTimeCorrection::CalcDelta()
{
	uint posCount(0);
	if (!mWithinBound || (mTimerV.size() < mMinConsecResults))
	{
		throw CTimeCorrect_exception(
				"To few samples within the maximum deviation bound. Can't count Delta",
				5);
	}
	TimerVector::iterator It(mTimerV.end()), minRTTIt;

	// find the entry with minimum RTT within the correction windows,
	// set minRTTIt to that entry. BTW calculate the minimum forward time
	// and minRTT
	// FIXME: after implementation of CTSCTimer_MAX in CTSCTimer, we have to use this constant
	HPTimer::HighPerTimer minF(0x7FFFFFFFFFFFFFFFLL, false), minB(
			0x7FFFFFFFFFFFFFFFLL, false), minRTT(0x7FFFFFFFFFFFFFFFLL, false);
	posCount = mTimerV.size() - mMinConsecResults;
	double mFirstSendTime, mLastSendTime;
	for (It -= mMinConsecResults; It != mTimerV.end(); It++)
	{
		if (It == (mTimerV.end() - mMinConsecResults))
		{
			mFirstSendTime=HPTimer::HighPerTimer::HPTimertoD(It->ST);
		}
		if(It==mTimerV.end()-1)
		{
			mLastSendTime=HPTimer::HighPerTimer::HPTimertoD(It->ST);
		}
		minF = std::min(minF, It->ForwardTime());
		if (minF == It->ForwardTime())
		{
			mCorrStats.MinFPos = posCount;
			mCorrStats.MinF = minF;
		}
		minB = std::min(minB, It->BackwardTime());
		if (minB == It->BackwardTime())
		{
			mCorrStats.MinBPos = posCount;
			mCorrStats.MinB = minB;
		}

		HPTimer::HighPerTimer RTTime(It->RTTime());
		if (RTTime <= minRTT)
		{
			minRTTIt = It;
			mCorrStats.MinRTTPos = posCount;
			minRTT = RTTime;
		}
//#ifdef DEBUG_OUTPUT
		//-------------------------------Debug-------------------------------- | added by Li, Yuan
		std::cout << "pos: " << posCount << "  FW: " << It->ForwardTime()
				<< "  BW: " << It->BackwardTime() << "  RRT: " << It->RTTime()
				<< "  ST: " << It->ST << "  RT: " << It->RT << "  RRT: "
				<< It->RRT << std::endl;
//#endif
		posCount++;
	}
	// The delta is (minF - minB)/2
	mCorrStats.ICADeltaSendTime=HPTimer::HighPerTimer::DtoHPTimer((mFirstSendTime+mLastSendTime)/2);
	HPTimer::HighPerTimer Delta = HPTimer::HighPerTimer::NSectoHPTimer(
			HPTimer::HighPerTimer::HPTimertoNSec(minF) / 2
					- HPTimer::HighPerTimer::HPTimertoNSec(minB) / 2);
	mCorrStats.VirtMinRTT = minF + minB;
	mCorrStats.Sigma_1 = HPTimer::HighPerTimer::NSectoHPTimer(
			HPTimer::HighPerTimer::HPTimertoNSec(mCorrStats.VirtMinRTT) / 2);
	mCorrStats.Sigma_2 = HPTimer::HighPerTimer::NSectoHPTimer(
			HPTimer::HighPerTimer::HPTimertoNSec(minRTT - mCorrStats.VirtMinRTT)
					/ 2);
#ifdef DEBUG_OUTPUT
	std::cerr << "!!!!! minRTT:" << minRTT << " minF: " << minF << " Delta: " << Delta << std::endl;
#endif
	mCorrStats.ICADelta = Delta;
	return Delta;
}

// calculate delta using Christian Algorithm on the minimum RTT sample
HPTimer::HighPerTimer CTimeCorrection::CalcDeltaCAMin()
{
	if (!mWithinBound || (mTimerV.size() < mMinConsecResults))
	{
		throw CTimeCorrect_exception(
				"To few samples within the maximum deviation bound. Can't count Delta",
				5);
	}
	TimerVector::iterator It(mTimerV.end()), minRTTIt;

	// find the entry with min RTT, calc the min RTT
	// FIXME: after implementation of CTSCTimer_MAX in CTSCTimer, we have to use this constant
	HPTimer::HighPerTimer minRTT(0x7FFFFFFFFFFFFFFFLL, false);
	for (It -= mMinConsecResults; It != mTimerV.end(); It++)
	{
		HPTimer::HighPerTimer RTTime(It->RTTime());
		minRTT = std::min(minRTT, RTTime);

		if (RTTime <= minRTT)
		{
			minRTTIt = It;
		}
	}

	// calculate the delta with Christian algorithm: Delta = RT - (ST + minRTT/2) =
	// minRTTIt->ForwardTime - minRTT/2
	HPTimer::HighPerTimer Delta = minRTTIt->ForwardTime()
			- HPTimer::HighPerTimer::NSectoHPTimer(
					HPTimer::HighPerTimer::HPTimertoNSec(minRTT) / 2);
	mCorrStats.CAMinDelta = Delta;
	mCorrStats.SigmaCAMin = HPTimer::HighPerTimer::NSectoHPTimer(
			HPTimer::HighPerTimer::HPTimertoNSec(minRTT) / 2);
	return Delta;
}

// calculate delta using Christian Algorithm on the first sample in the train
HPTimer::HighPerTimer CTimeCorrection::CalcDeltaCA()
{
	if (!mWithinBound || (mTimerV.size() < mMinConsecResults))
	{
		throw CTimeCorrect_exception(
				"To few samples within the maximum deviation bound. Can't count Delta",
				5);
	}
	TimerVector::iterator It(mTimerV.begin());

	// calculate the delta with Christian algorithm: Delta = RT - (ST + minRTT/2) =
	// minRTTIt->ForwardTime - minRTT/2
	HPTimer::HighPerTimer Delta = It->ForwardTime()
			- HPTimer::HighPerTimer::NSectoHPTimer(
					HPTimer::HighPerTimer::HPTimertoNSec(It->RTTime()) / 2);

	mCorrStats.CADelta = Delta;
	mCorrStats.SigmaCA = HPTimer::HighPerTimer::NSectoHPTimer(
			HPTimer::HighPerTimer::HPTimertoNSec(It->RTTime()) / 2);
	return Delta;
}

// reset the time correction object for reuse - only data structures are resetted,
// not the socket connection
void CTimeCorrection::ResetTCorr()
{
	mCorrStats = TCorrStats();
	mWithinBound = false;
	mSampleCount = 0;
	mTimerV.clear();
	mWithinBound = false;
	return;
}

/*
 Test the deviation
 */bool CTimeCorrection::TestDeviation(const CMeasuredTimes & pFirst,
		const CMeasuredTimes & pSecond)
{
	HPTimer::HighPerTimer devF, devB;

	try
	{
		// test the abs of forward deviation
		HPTimer::HighPerTimer timer1 = pSecond.ForwardTime();
		timer1.InvertSign();
		devF = pFirst.ForwardTime() + timer1;
		if (devF.Negative())
		{
			devF.InvertSign();
		}

		// test the abs of backward deviation
		HPTimer::HighPerTimer timer2 = pSecond.BackwardTime();
		devB = pFirst.BackwardTime() + timer2;
		if (devB.Negative())
		{
			devB.InvertSign();
		}
		// fill out statistics
		mCorrStats.MaxDevF = std::max(mCorrStats.MaxDevF, devF);
		mCorrStats.MaxDevB = std::max(mCorrStats.MaxDevB, devB);
		mCorrStats.MinDevF = std::min(mCorrStats.MinDevF, devF);
		mCorrStats.MinDevB = std::min(mCorrStats.MinDevB, devB);

		HPTimer::HighPerTimer RTT1(pFirst.RTTime());
		HPTimer::HighPerTimer RTT2(pSecond.RTTime());
		mCorrStats.MaxRTT = std::max(mCorrStats.MaxRTT, std::max(RTT1, RTT2));
		mCorrStats.MinRTT = std::min(mCorrStats.MinRTT, std::min(RTT1, RTT2));
		if (mCorrStats.MaxRTT <= RTT1)
		{
			mCorrStats.MaxRTTPos = mSampleCount;
		}

		// if the first packet will have the maximum RTT, the above variant will not work
		if (mCorrStats.MaxRTT <= RTT2)
		{
			mCorrStats.MaxRTTPos = mSampleCount - 1;
		}

		if (std::max(devF, devB) <= mMaxDev)
		{
			return true;
		}

	} // ---- end try

	catch (const CTimeCorrect_exception & ex)
	{
		return false;

	} catch (const CNetwork_exception & ex)
	{
		throw;
		return false;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////
// the time correction class, implemented by means of CORBA
///////////////////////////////////////////////////////////////////////////

// ctor
GSOAPTimeCorrection::GSOAPTimeCorrection(ltestProxy *pProxy,
		unsigned int pMaxDev, unsigned int pMinConsecResults,
		unsigned int pMaxTries, unsigned int pTimeout) :
		CTimeCorrection(pProxy, pMaxDev, pMinConsecResults, pMaxTries, pTimeout)
{
}

// dtor
GSOAPTimeCorrection::~GSOAPTimeCorrection()
{
}

// get time from the ltest server
// the parameter MyTime is currently unsused - in future,
// the two-way algorithm will use the param MyTime
HPTimer::HighPerTimer &
GSOAPTimeCorrection::GetTime(const HPTimer::HighPerTimer & pMyTime,
		HPTimer::HighPerTimer & pRemoteTime, unsigned int pSeqNo)
{

	tv_type tv_time;
	if (SOAP_OK != mProxy->get_time_by_gsoap(tv_time))
		mProxy->soap_stream_fault(std::cerr);
	pRemoteTime.SetTimer(static_cast<int64_t>(tv_time.tv_sec),
			static_cast<int64_t>(tv_time.tv_usec) * 1000);
	return (pRemoteTime);
}

///////////////////////////////////////////////////////////////////////////
// the time correction class, impelemted via the UDP channel
///////////////////////////////////////////////////////////////////////////

// ctor
CUDPTimeCorrection::CUDPTimeCorrection(ltestProxy *pProxy,
		CIPAddress & pRemoteAddress, unsigned int pMaxDev,
		unsigned int pMinConsecResults, unsigned int pMaxTries,
		unsigned int pTimeout) :
		CTimeCorrection(pProxy, pMaxDev, pMinConsecResults, pMaxTries,
				pTimeout), mTimeSocket(true)
{
	uint16_t Port;
	try
	{
		if (SOAP_OK != pProxy->start_UDP_time_port(pRemoteAddress.Port(), Port))
			pProxy->soap_stream_fault(std::cerr);
		if (Port == 0)
		{
			throw CTimeCorrect_exception(
					"Can't get UDP port at the server side. Returned port "
							+ CStringUtils::itos(Port), Port);
		}
	} catch (...)
	{
		throw;
	}
	pRemoteAddress.Port(Port);

	try
	{
		if (!mTimeSocket.Connect(pRemoteAddress))
		{
			throw CTimeCorrect_exception(
					"Can't connect the Time correction serer at the address "
							+ CIPAddress::atodd(pRemoteAddress.IPAddress())
							+ ":" + CStringUtils::itos(Port), 1);
		}
		mTimeSocket.SetRecvTimeout(mTimeout);
	} catch (const CTimeCorrect_exception & ex)
	{
		throw ex;
	}

}

// dtor
CUDPTimeCorrection::~CUDPTimeCorrection()
{
	if (mProxy != NULL)
	{
		struct ReleaseUDPTimePortResponse udptimeportres;
		mProxy->release_UDP_time_port(udptimeportres);
		std::cerr << "UDP time port released at the server" << std::endl;
	}
	else
	{
		std::cerr
				<< "the UDP time port couldn't be released, because the mSession object doesn't"
				<< " exist any more" << std::endl;
	}
}

// get one time sample from the ltest server via the established UDP channel

HPTimer::HighPerTimer &
CUDPTimeCorrection::GetTime(const HPTimer::HighPerTimer & pMyTime,
		HPTimer::HighPerTimer & pRemoteTime, unsigned int pSeqNo)
{

	char Buff[12]; // timestamp buffer 0..3 - sequence number, 4..7 seconds, 8..11 microseconds
	char * pos(Buff);

	// send a packet with sequence number and timestamp
	unsigned int sec(pMyTime.Seconds()), usec(pMyTime.USeconds()), SeqNo;

	*(uint32_t *) pos = htonl(pSeqNo);
	pos += 4;
	*(uint32_t *) pos = htonl(sec);
	pos += 4;
	*(uint32_t *) pos = htonl(usec);

	// use timer for receiving the answer from the server

	mTimeSocket.Send(Buff, sizeof(Buff));
	memset(Buff, 0, sizeof(Buff));

	pos = Buff;
	// read data packet with the given sequence number
	// especially, skip packet duplications, throw away all data packets with different
	// sequence numbers
	while (true)
	{
		unsigned int rByte(0), readLen(sizeof(Buff));
		while (rByte < sizeof(Buff))
		{
			rByte += mTimeSocket.Recv(pos, readLen);

			if (rByte <= 0)
			{
				// timeout occured
				std::stringstream str;
				str << mTimeout;
				throw(CNetwork_exception(
						"Timeout during GetTime! Packet not received within "
								+ str.str() + " seconds.", 10));
			}

			pos += rByte;
			readLen -= rByte;

		}

#ifdef DEBUG_OUTPUT
//  std::cerr << "Read " << rByte << " bytes from UDP time port.\n"
//       << "Hex dump: 0x" << hex;
//  for (uint i(0); i< rByte; i++){
//      std::cerr << (int) (unsigned char)Buff[i] << " ";
//  }
//  std::cerr << dec << std::endl;
#endif

		pos = Buff;
		SeqNo = ntohl(*(uint32_t *) pos);

		// if received data packet with different sequence number, look for the next data packet
		if (SeqNo != pSeqNo)
		{
			continue;
		}
		pos += 4;
		sec = ntohl(*((uint32_t *) pos));
		pos += 4;
		usec = ntohl(*((uint32_t *) pos));
		pRemoteTime.SetTimer(static_cast<int64_t>(sec),
				static_cast<int64_t>(usec) * 1000);
// pRemoteTime.setutimer(usec);

#ifdef DEBUG_OUTPUT
		std::cerr << "Parsed receiver time is: " << "Seconds: " << sec << " Microseconds: "
		<< usec << " CTimer: " << pRemoteTime << std::endl;
#endif
		break;
	}
	return pRemoteTime;
}

// E O F

