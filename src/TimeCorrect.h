// This may look like C code, but it is really -*- C++ -*-
//
//
// Author:  Author Eduard Siemens <eduard.siemens@thomson.net>
//
// File:    TimeCorrect.h
//          Thue Dec 1 2005
//
// $Header:   //hanvsfile02/prog/Projects/2005/hipernet/Repository/hipernet/archives/HiperNet/Linux/Apps/ltest/TimeCorrect.h-arc   1.2.1.6   03 Apr 2007 16:11:14   siemense  $
// $Version$
// $Author:   siemense  $
// $Date:   03 Apr 2007 16:11:14  $
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
// $Log:   //hanvsfile02/prog/Projects/2005/hipernet/Repository/hipernet/archives/HiperNet/Linux/Apps/ltest/TimeCorrect.h-arc  $
//
//   Rev 1.2.1.6   03 Apr 2007 16:11:14   siemense
//.
//
//   Rev 1.2.1.5   03 Apr 2007 13:42:18   siemense
//GetTime stalling buf fixed
//
//   Rev 1.2.1.4   31 Jan 2007 09:11:36   siemense
//Efficiency issues solved. Interfaces to network classes has been cleaned up.
//
//   Rev 1.2.1.3   13 Nov 2006 11:58:22   siemense
//Minor bug fixes.
//Version before CIPNetworkPerformance tunings.
//
//   Rev 1.2.1.1   17 Aug 2006 13:21:16   siemense
//TSC timer class has been implemented and rudimentary tested
//
//   Rev 1.2.1.0   Jun 27 2006 10:40:22   siemense
//New Protocols Branch
//
//   Rev 1.2   21 Jun 2006 13:51:46   siemense
//Improved Cristian Algorithm implemented properly
//
//   Rev 1.2   21 Jun 2006 13:16:08   siemense
//Cristian Algorithm corrected
//
//   Rev 1.1   27 Feb 2006 12:18:40   siemense
//Timesync in old fashion (Christian algorithm) added
//
//   Rev 1.0   02 Feb 2006 12:53:02   siemense
//Initial revision.
//
//   Rev 1.1   21 Dec 2005 16:42:32   siemense
//Time correction between data sender and data recei ver added
//
//   Rev 1.0   02 Dec 2005 12:59:56   siemense
//Initial revision.
//
//


#ifndef TIME_CORRECT_H
#define TIME_CORRECT_H

#include <sstream>
#include "network.h"
#include "soapltestProxy.h"

/**
time correction statistics struct
 */
struct TCorrStats{
  HPTimer::HighPerTimer StartTime;
  HPTimer::HighPerTimer FinishTime;
  unsigned int Tries;                // overall performed trees
  unsigned int ConsNumber;           // amount of consecutive tries within the bound
  HPTimer::HighPerTimer MaxRTT;       // maximum experienced RTT
  uint   MaxRTTPos;                  // position of the max RTT sample
  HPTimer::HighPerTimer MaxDevF;      // maximum experienced deviation
  HPTimer::HighPerTimer MaxDevB;      // maximum experienced deviation
  HPTimer::HighPerTimer MinDevF;      // minimum experienced deviation
  HPTimer::HighPerTimer MinDevB;      // minimum experienced deviation
  HPTimer::HighPerTimer MinRTT;       // minimum expirienced RTT
  uint   MinRTTPos;                  // position of the min RTT sample
  HPTimer::HighPerTimer MinF;         // minimum Forward Time
  uint   MinFPos;                    // position of the minF sample
  HPTimer::HighPerTimer MinB;         // minimum Backward Time
  uint   MinBPos;                    // position of the minB sample
  HPTimer::HighPerTimer VirtMinRTT;   // minimum calculated virtual RTT (minF + minB)
  HPTimer::HighPerTimer ICADelta;     // delta time, using improved Christian Algorithm
  HPTimer::HighPerTimer ICADeltaSendTime; // send time, which the delta time calculated | added by Li, Yuan
  HPTimer::HighPerTimer CAMinDelta;   // delta time, using Christian Algorithm on the sample with minimum RTT
  HPTimer::HighPerTimer CADelta;      // delta time, using Christian Algorithm on the first sample in the train
  HPTimer::HighPerTimer Sigma_1;      // deterministic sigma bound = VirtMinRTT/2
  HPTimer::HighPerTimer Sigma_2;      // stochastic sigma = (MinRTT - VirtMinRTT)/2
  HPTimer::HighPerTimer SigmaCA;      // deterministic sigma, unsing Christian Algorithm
  HPTimer::HighPerTimer SigmaCAMin;   // deterministic sigma, unsing Christian Algorithm, applied to the minimum sample
  TCorrStats();
  std::ostream & print_out (std::ostream & pStr) const;
  std::ostream & print_out_ext (std::ostream & pStr) const;

};

// ostream & operator<<(ostream & pStr, const TCorrStats & stats);

class CTimeCorrect_exception: public std::exception {

public:
  CTimeCorrect_exception(){}
  CTimeCorrect_exception(const std::string & pMsg, int pCode=0):
    eMsg(pMsg),
    eCode(pCode){}
  virtual const char * what() const throw ()
  {
    return ("TimeCorrect Exception: " + eMsg).c_str();
  }
  virtual int code() const throw ()
  {
    return eCode;
  }

  virtual ~CTimeCorrect_exception() throw () {}

private:
    std::string eMsg;
  int eCode;

};


/**
 Object for storing sending (ST) receiver (RT) and receiving (RRT)times.
 Also calculates forward  one-way-delay, backward one-way delay as well as RTT.
 If one of the corresponding times are zero,
 throws an appropriated exception ( with codes 2 - ST == 0, 3 RT == 0, 4 RRT == 0
*/
class CMeasuredTimes {
public:
  /// ctor
  CMeasuredTimes();
  /// non virtual dtor, because should be final
  ~CMeasuredTimes();
  HPTimer::HighPerTimer ForwardTime() const throw (CTimeCorrect_exception);
  HPTimer::HighPerTimer BackwardTime() const throw (CTimeCorrect_exception);
  HPTimer::HighPerTimer RTTime() const throw (CTimeCorrect_exception);
  HPTimer::HighPerTimer ST;
  HPTimer::HighPerTimer RT;
  HPTimer::HighPerTimer RRT;
};

/**
   Implementation of the time correction.
   The class acts as a base class for UDP and CORBA based time correction against the LTest
   server.
*/

class CTimeCorrection
{
public:
  /**
     ctor
   */
  CTimeCorrection();
  /**
     dtor
  */
  virtual ~CTimeCorrection();
  /**
     The worker routine, performs the probing one-way delays
   */
  virtual bool PerformCorrection();
  /**
     calculate the offset between data receiver and data sender
     using the improved Christian algorithm. Returns the calculated offset.
   */
  virtual HPTimer::HighPerTimer CalcDelta();

  /**
     calculate the offset between data receiver and data sender
     using Christian algorithm. Returns the calculated offset.
   */
  virtual HPTimer::HighPerTimer CalcDeltaCA();

  /**
     calculate the offset between data receiver and data sender
     using Christian algorithm ob the minimum RTT sample.
     Returns the calculated offset.
   */
  virtual HPTimer::HighPerTimer CalcDeltaCAMin();

  /**
     reset time correction reset for a new measurement on the same socket.
     Necessary for next PerformCorrection calls without
     reinstantiation of the CTimeCorrection object
  */
  virtual void ResetTCorr();

  TCorrStats mCorrStats;

protected:
  /**
     ctor.
     @param pRemoteAddr is the address, the remote servers resides on
     @param pMaxDev points to the maximal accepted deviation of measured one-way delays-
     @param pTimeout specifies the timeout for the connection to the peer (epsecially
     usefull for time correction via UDP
     @param pMinConsecResults specifies the minimum amount of consecutive tries within the
     maximum deviation bound
  */
  CTimeCorrection(ltestProxy *pProxy, unsigned int pMaxDev,
		  unsigned int pMinConsecResults, unsigned int pMaxTries,
		  unsigned int pTimeout);

  /**
     Test, if delay deviations lies within the permitted bounds (mMaxDev)hprt
  */
  virtual bool TestDeviation(const CMeasuredTimes & pFirst,
			     const CMeasuredTimes & pSecond);
  /**
     Fetch the time from the remote side. Return value is passed by reference as
     pRemoteTime, (passed to the routine as parameter for performance purposes).
     @param pSeqNo is the sequence number of the time request - used by UDP time
     syncronization routine
  */
  virtual HPTimer::HighPerTimer & GetTime(const HPTimer::HighPerTimer & pMyTime,
                                         HPTimer::HighPerTimer & pRemoteTime,
                                         unsigned int pSeqNo) = 0;


  /* amount of minimum consecutive results, with a deviation less or equal the pMaxDev
     bound
  */
  const unsigned int mMinConsecResults;
  const unsigned int mMaxTries; // try maximal mMaxTries times
  HPTimer::HighPerTimer mMaxDev;
  HPTimer::HighPerTimer mTimeout;
  bool mWithinBound;

  ltestProxy *mProxy;

  // sample counter
  uint mSampleCount;

  /**
     vector of timer arrays. Each element consits of a array of 3 timers - sender time
     receiver time, receiving time (back to the sender)
  */
  typedef std::vector <CMeasuredTimes> TimerVector;
  TimerVector mTimerV;


};


/**
   Implementation of the time correction via CORBA
   The time will be fetched via the CORBA channel
*/

class GSOAPTimeCorrection: public CTimeCorrection
{
public:
  /**
     only the session interface reference is the mandatory parameter,
     all other can be set to defaults. It is assumed, the CORBA machinery is initialized
 * @param timeout timeout of the packet reception, in milliseconds
   */
    GSOAPTimeCorrection(ltestProxy *pProxy,
		       unsigned int pMaxDev = 10000,
		       unsigned int pMinConsecResults = 15,
		       unsigned int pMaxTries = 200,
		       unsigned int pTimeout = 500);
  /**
     dtor
  */
  virtual ~GSOAPTimeCorrection();
  /**
     Fetch the time from the remote side. Return value is passed by reference, (passed to
     the routine as parameter for performance purposes)
  */
  HPTimer::HighPerTimer & GetTime(const HPTimer::HighPerTimer & pMyTime, 
                                 HPTimer::HighPerTimer & pRemoteTime, 
                                 unsigned int pSeqNo);
};

///////////////////////////////////////////////////////////////////////////
/**
   Implementation of the time correction via a UDP channel.
   Packets will be sent to the remote side via a UDP connection
   and the listener will wait for timestamps, responsed from the server
*/
class CUDPTimeCorrection: public CTimeCorrection
{
public:
  /**
      Only the session reference as well as theIP address of the UDP time
      receiver  interface reference are mandatory
   * @param timeout timeout of the packet reception, in milliseconds
  */
    CUDPTimeCorrection(ltestProxy *pProxy, CIPAddress & pRemoteAddr,
		     unsigned int pMaxDev = 10000,
		     unsigned int pMinConsecResults = 15,
		     unsigned int pMaxTries = 200,
		     unsigned int pTimeout = 500);
  /**
     dtor
  */
  virtual ~CUDPTimeCorrection();

  /**
     Fetch the time from the remote side. Return value is passed by reference, (passed to
     the routine as parameter for performance purposes)
  */
  HPTimer::HighPerTimer & GetTime(const HPTimer::HighPerTimer & pMyTime, 
                                 HPTimer::HighPerTimer & pRemoteTime, unsigned int pSeqNo);


protected:
  /**
     Connect to the remote side
  */
    bool ConnectTServer(const CIPAddress & pAddress);
  CUDPSocket mTimeSocket;

};


#endif // ----- TIME_CORRECT_H -------
