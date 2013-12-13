// This may look like C code, but it is really -*- C++ -*-
//
// $Id: cstats.h,v 2.3 2005/07/19 15:46:19 siemens Exp $
//
// $Header:   //hanvsfile02/prog/Projects/2005/hipernet/Repository/hipernet/archives/hipernet/Linux/Apps/ltest/cstats.h-arc   1.5.1.4   31 Jan 2007 09:10:38   siemense  $
//
// Author:  Igor Olkhovsky, Eduard Siemens <siemens@rvs.uni-hannover.de>
//
// File:    cstats.h
//          Wed Jun 30 2004
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
// $Log:   //hanvsfile02/prog/Projects/2005/hipernet/Repository/hipernet/archives/hipernet/Linux/Apps/ltest/cstats.h-arc  $
//
//   Rev 1.5.1.4   31 Jan 2007 09:10:38   siemense
//Efficiency issues solved. Interfaces to network classes has been cleaned up.
//
//   Rev 1.5.1.3   13 Nov 2006 11:57:20   siemense
//Minor bug fixes.
//Version before CIPNetworkPerformance tunings.
//
//   Rev 1.5.1.1   17 Aug 2006 13:21:08   siemense
//TSC timer class has been implemented and rudimentary tested
//
//   Rev 1.5.1.0   Jun 27 2006 10:39:50   siemense
//New Protocols Branch
//
//   Rev 1.5   21 Dec 2005 16:42:28   siemense
//Time correction between data sender and data recei ver added
//
//   Rev 1.4   15 Nov 2005 15:10:38   siemense
//zero duration and packet count bug fixed
// *
// *    Rev 1.3   14 Nov 2005 16:32:12   siemense
// * Data range of rate extended to long long. Minor bug fixes.
// *
// *    Rev 1.2   07 Oct 2005 09:11:26   siemense
// * unnecessary files removed
// *
// *    Rev 1.1   06 Oct 2005 17:06:56   siemense
// * data rate limit extended
//  *
//  *    Rev 1.0   28 Sep 2005 16:34:02   siemense
//  * Initial revision.
//
// Revision 2.3  2005/07/19 15:46:19  siemens
// Final release from Eduard Siemens at the RVS
//
// Revision 2.2  2005/02/07 14:43:14  siemens
// bereinigung von fremden Code
//
// Revision 2.1  2004/07/01 19:48:09  siemens
// intermediate version, tcp performance bug is still there
//
// stats class
// parent class for statistics

#include "HighPerTimer.h"
#include "strtok.h"

//#include "ctsend.h"


#ifndef CSTATS_H
#define CSTATS_H



/**
Superclass for statistics, this is only an interface (abstract) class
*/


class CStats
{
public:
  CStats();
  CStats(std::string FileName, int wh, int ht, std::string FileName2);

  virtual ~CStats();

  /**
     This method is called every time, an sample with results is processed
  */
  virtual void procLine( unsigned long & pNum, HPTimer::HighPerTimer & pSentT, HPTimer::HighPerTimer & pRecvT, HPTimer::HighPerTimer & pDelay, const unsigned int pSize) = 0;

  /**
     This method is called after all samples are processed
   */
  virtual void postProc() = 0;

  /**
     This method is for displayind, showing resulting data (print to stderr, svg files, etc.)
   */
  virtual void showResults(std::ostream & pStr) = 0;
};

class CBasicStats
{
public:
  /**
     default constructor
   */
  CBasicStats();
   
  /**
     default destructor
   */
  virtual ~CBasicStats();

  /**
       This method counts statistical values and square of statistical values
     */

  void procLine( uint32_t & pNum, HPTimer::HighPerTimer & pSentT, HPTimer::HighPerTimer & pRecvT, HPTimer::HighPerTimer & pDelay,
                 const uint32_t pSize);

  /**
     This method gather median and standard deviation of interpacket times as well as durations and packet sizes
   */
  void postProc();

  /**
     This method print results to stdout
   */
  void showResults(std::ostream & pStr);

  /**
     Correct delay related statistics by Delta (the constant time offset between the
     data sender and data receiver
   */
  virtual void CorrectDelays(HPTimer::HighPerTimer & pDelta);

  // BasicStats myGrafBasis;
protected:

  ///SentTimeVariables
  HPTimer::HighPerTimer mSentTMin;  // min sent interpacket time
  HPTimer::HighPerTimer mSentTMax;  // max sent interpacket time
  unsigned long mSentMaxPos; // position of maximum sent interpacket time
  unsigned long mSentMinPos; // position of minimum sent interpacket time

  // CTSCTimer mSent_im1T;
// timers for calculation of interpacket times (currently sending and receiving intertimes)
HPTimer::HighPerTimer mPrevSentT; // sent time of the previous valid sent data packet
HPTimer::HighPerTimer mPrevRecvT; // receivint time of the previous received data packet
  // CTSCTimer mSent_i0;
  // CTSCTimer mSentDT;

  ///firtst and last sending times
  HPTimer::HighPerTimer mFirstSentT;
  HPTimer::HighPerTimer mLastSentT;

  double mSentTSum;   // sum of sending interpacket times
  double mSentTSumqv; // square sum of sending interpacket times
  double mSentAvg; // mean of sending intetrpacket times
  double mSentStdv;  // standard deviation of interpacket times

  ///RecvTimeVariables
  HPTimer::HighPerTimer mRecvTMin; // min receiving interpacket time
  HPTimer::HighPerTimer mRecvTMax; // max receiving interpacket time
  unsigned long mRecvMaxPos; // position of maximum receiving interpacket time
  unsigned long mRecvMinPos; // position of minimum receiving interpacket time

  HPTimer::HighPerTimer mRecv_im1T;
  HPTimer::HighPerTimer mRecv_i0;
  HPTimer::HighPerTimer mRecvDT;

  double mRecvTSum;
  double mRecvTSumqv;
  double mRecvAvg;
  double mRecvStdv;

  /// Delay variables
  HPTimer::HighPerTimer mDelayMin;  // min delay
  HPTimer::HighPerTimer mDelayMax;  // max delay
  double mDelayAvg;     // mean delay



  double hDel;

  double mDelaySum;
  double mDelaySumqv;
  double mDelayStdv;

  /// Packet size and bytes count variables
  unsigned int mPktSizeMin; // min packet size
  unsigned int mPktSizeMax; // max packet size
unsigned int mSentPackets;  // sent packets counter
unsigned long long mSentBytes; // sent bytes counter

  double mPktBytesSum;      // overall bytes sum (sum of processed bytes)
  double mPktBytesSumqv;    // square sum of bytes processed
  double mPackAvg;          // average packet size
  double mSizeStdv;        // stdv of packet sizes
  double mPktBytesSumLoss;  // sum of bytes lost


  unsigned long mLineCount; //overall counter for procLine calls
  unsigned long mSentCount; // sending packets counter
  unsigned long mRecvCount; // receiving packets counter


  unsigned long mDelayMaxPos;  // position of maximum delay
  unsigned long mDelayMinPos;  // position of minimum delay
  unsigned long mSizeMaxPos; // position of maximum packet size
  unsigned long mSizeMinPos; // position of minimum packet size
  // the performed time correction
  HPTimer::HighPerTimer mDeltaTime;
// Temporary timer. Used as a local variable. Only the construction time should be saved
HPTimer::HighPerTimer mTempT;
private:
/// process delay calculations.
// void calcDelays();
};

#endif // CSTATS_H
