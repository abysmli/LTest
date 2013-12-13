// stats class

#include "cstats.h"
#include "strtok.h"
#include <iomanip>
#include <iostream>
#include <cmath>
//!!!
#include <cstdlib>


/// ctor. Basis class for statistics
CStats::CStats()
{}

/// ctor. Basis class for statistics
CStats::CStats(std::string FileName, int wt, int ht, std::string FileName1)
{}


/// dtor
CStats::~CStats()
{}

/// dtor. Basic statistics class. Prints out statistics to the standard output
CBasicStats::CBasicStats():
    mSentTMin(HPTimer::HighPerTimer::HPTimer_MAX), // minimum sending interpacket time
    mSentTMax(HPTimer::HighPerTimer::HPTimer_MIN), // maximum sending interpacket time
    mSentMaxPos(0), // position of maximum sending interpacket time
    mSentMinPos(0), // position of minimum sending interpacket time
    mPrevSentT(HPTimer::HighPerTimer::Nil()), // sending time stamp of the previous data packet
    mPrevRecvT(HPTimer::HighPerTimer::Nil()), // receivint time stamp of the previous data packet
    mFirstSentT(HPTimer::HighPerTimer::Nil()),   // first sending time
    mLastSentT(HPTimer::HighPerTimer::Nil()),    // last sending time
    mSentTSum(0),   // sum of sending interpacket times
    mSentTSumqv(0),
    mSentAvg(0), // mean sending interpacket time
    mSentStdv(0),  // standard deviation
    mRecvTMin(HPTimer::HighPerTimer::HPTimer_MAX),
    mRecvTMax(HPTimer::HighPerTimer::HPTimer_MIN),
    mRecvMaxPos(0), // position of maximum receiving interpacket time
    mRecvMinPos(0), // position of minimum receiving interpacket time
    mRecvTSum(0),    // sum of receiving interpacket times
    mRecvTSumqv(0),  // square sum of receiving interpacket times
    mRecvAvg(0),  // mean receiving interpacket time
    mRecvStdv(0),   // stamdard deviation
    mDelayMin(HPTimer::HighPerTimer::HPTimer_MAX),
    mDelayMax(HPTimer::HighPerTimer::HPTimer_MIN),
    mDelayAvg(0),    // delay mean
    mDelaySum(0),     // sum of delays
    mDelaySumqv(0),   // square sum of packet delays
    mDelayStdv(0),    // standard deviation of delays
    mPktSizeMin(0xFFFFFFFF),
    mPktSizeMax(0),
    mSentPackets(0),
    mPktBytesSum(0),
    mPktBytesSumqv(0),
    mPackAvg(0),  // mean of packet sizes
    mSizeStdv(0),   // standard deviation of packet sizes
    mPktBytesSumLoss(0), //sum of lost packets
    mLineCount(0), // overall counter of processed packets
    mSentCount(0), // sent packets counter
    mRecvCount(0), // received packets counter
    mDelayMaxPos(0), // position of maximum delay
    mDelayMinPos(0), // position of minium delay
    mSizeMaxPos(0),  // position of maximum packet size
    mSizeMinPos(0)   // positino of minimum packet size
{
  /*std::cout<< "mSentTMin: "<< mSentTMin << std::endl;
  std::cout<< "mSentTMax: "<< mSentTMax<< std::endl;
  std::cout<< "mRecvTMin: "<< mRecvTMin << std::endl;
  std::cout<< "mRecvTMax: "<< mRecvTMax<< std::endl;
  std::cout<< "mDelayMin: "<< mDelayMin << std::endl;
  std::cout<< "mDelayMax: "<< mDelayMax<< std::endl;*/
}

/// dtor
CBasicStats::~CBasicStats()
{}

/// calculations, performed at each line of the measurement results
void
CBasicStats::procLine( uint32_t & pNum, HPTimer::HighPerTimer & pSentT, HPTimer::HighPerTimer & pRecvT,
                       HPTimer::HighPerTimer & pDelay, const uint32_t pSize)
{
  // corrupted parameters given - sending time never should be zero
    if (pSentT.IsNil())
  {
    return;
  }

  mSentCount++;

  // sent time part

  // if previous sent time not set, we have the first valid sent packet.
  // Interpacket times can't be calculated yet.
  if (mPrevSentT.IsNil())
  {
    // first packet sent time
    mFirstSentT = pSentT;
  }
  // interpacket times can be calculated
  else
  {
    mTempT = pSentT;
    mTempT -= mPrevSentT;
    double interTime(HPTimer::HighPerTimer::HPTimertoD(mTempT));
    mSentTSum += interTime;
    mSentTSumqv += interTime * interTime;

    // min/max calculations
    if(mTempT < mSentTMin)
    {
      mSentTMin = mTempT;
      mSentMinPos = mSentCount - 1;
    }

    if(mTempT > mSentTMax)
    {
      mSentTMax = mTempT;
      mSentMaxPos = mSentCount - 1;
    }

  }
  // common part of sending times
  mPrevSentT = pSentT;
  //last sending time;
  mLastSentT = pSentT;


  // packet size related calculations

  if(pSize < mPktSizeMin)
  {
    mSizeMinPos = mSentCount - 1;
    mPktSizeMin  =  pSize;
  }
  if(pSize > mPktSizeMax)
  {
    mSizeMaxPos = mSentCount - 1;
    mPktSizeMax  =  pSize;
  }
  double size(pSize);
  
  mPktBytesSum  += size; 
  mPktBytesSumqv += size * size;
  // --- end of sending time processing part

  // valid receive time available - receviting as well as delay statistics can be
  // calculated
  if (!pRecvT.IsNil())
  {
    // only if previuos receiving time is set, we can calculate
    // calculate interpacket times as well as packet loss.
    if (!mPrevRecvT.IsNil())
    {
      // interpacket times can be calculated
      mTempT = pRecvT;
      mTempT -= mPrevRecvT;
      double interTime(HPTimer::HighPerTimer::HPTimertoD(mTempT));
      mRecvTSum += interTime;
      mRecvTSumqv += interTime * interTime;

      // min/max calculations
      if(mTempT < mRecvTMin && !mTempT.Negative())
      {
        mRecvTMin = mTempT;
        mRecvMinPos = mSentCount - 1;
      }

      if(mTempT > mRecvTMax)
      {
        mRecvTMax = mTempT;
        mRecvMaxPos = mSentCount - 1;
      }

    }
    // common part of sending times
    mPrevRecvT = pRecvT;
    mRecvCount++;
    // delay calculations
    double delay(HPTimer::HighPerTimer::HPTimertoD(pDelay));
    mDelaySum  += delay;
    mDelaySumqv += delay * delay;
    // min/max processing
    if (pDelay < mDelayMin)
    {
      mDelayMin = pDelay;
      mDelayMinPos = mSentCount - 1;
    }

    if (pDelay > mDelayMax)
    {
      mDelayMax = pDelay;
      mDelayMaxPos = mSentCount -1 ;
    }

  }
  // packet loss case (receive time is zero)
  else
  {
    mPktBytesSumLoss += static_cast<double>(pSize);
  }

  mLineCount++;
}

/// calculations, performed after all samples are read
void
CBasicStats::postProc()
{
  //Sent time processing
  mSentAvg = mSentTSum/(mLineCount-1);
  mSentStdv = std::sqrt(static_cast <long double> (abs((long double)(mSentTSumqv/(mLineCount-1) - mSentAvg*mSentAvg))));


  //Recv time processing
  mRecvAvg = static_cast<long double> (mRecvTSum)/static_cast<long double> (mRecvCount-1); //(mLineCount-1);
  mRecvStdv = std::sqrt(static_cast <long double> (abs((long double)(mRecvTSumqv/(mRecvCount -1) - mRecvAvg*mRecvAvg))));

  //Delay processing
  mDelayAvg = mDelaySum/static_cast<long double>(mRecvCount);
  mDelayStdv = std::sqrt(static_cast <long double> (abs((long double)(mDelaySumqv/mRecvCount-mDelayAvg*mDelayAvg))));


  //Packet size processing
  mPackAvg = double(mPktBytesSum)/static_cast<long double>(mLineCount); 
  mSizeStdv = std::sqrt(static_cast <long double> (abs((long double)(mPktBytesSumqv/(mLineCount) - mPackAvg * mPackAvg))));

}


// delay correction method
void
CBasicStats::CorrectDelays(HPTimer::HighPerTimer & pDelta)
{  
  mDeltaTime = pDelta;
  // pDelta.InvertSign();
  mDelayMin -= pDelta;
  mDelayMax -= pDelta; 
  mDelayAvg -= HPTimer::HighPerTimer::HPTimertoD(pDelta);
}

/// output of measurement results
void
CBasicStats::showResults(std::ostream & pStr)
{  
   
  // mRecvCount--;
    pStr << "==========" << std::endl;
    pStr << "Statistics:" << std::endl;
    pStr << "============================================================" << std::endl;
    pStr << "   Parameter   |       Packets       |        Bytes         " << std::endl;
    pStr << "============================================================" << std::endl;
  pStr << " Sent          |";
  pStr << std::setfill(' ') << std::setw(21)
  << CStringUtils::decades_formatted(mSentCount) << "|";
  pStr << std::setfill(' ') << std::setw(21)  << mSentCount*mPackAvg << std::endl;


  pStr << " Received      |";
  pStr << std::setfill(' ') << std::setw(21)  << CStringUtils::decades_formatted(mRecvCount) << "|";
  pStr << std::setfill(' ') << std::setw(21) << mRecvCount*mPackAvg << std::endl;

  pStr << " Dropped       |";
  pStr << std::setfill(' ') << std::setw(21) << CStringUtils::decades_formatted(mSentCount-mRecvCount)
  << "|";
  pStr << std::setfill(' ') << std::setw(21) <<  mPktBytesSumLoss <<  std::endl;


  pStr << " Loss (%)      |";
  pStr << std::setfill(' ') << std::setw(21) << std::setprecision(3)
  << 100.00*((double)mSentCount-(double)mRecvCount)/(double)mSentCount << "|";

  pStr << std::setfill(' ') << std::setw(21) << std::setprecision(3)
          << (double)mPktBytesSumLoss/((double)mSentCount*(double)mPackAvg)*100 <<  "\n" << std::endl;


  pStr << "=============================================================================================="
          << std::endl;
  pStr << "               |        Sent         |        Received     |        Delay        | Length  "
          << std::endl;
  pStr << "=============================================================================================="
          << std::endl;
  pStr << "   Parameter   | InterPacketTime [s] | InterPacketTime [s] |       [s]           | [bytes] "
          << std::endl;
  pStr <<"=============================================================================================="
          << std::endl;

  pStr << "     Min       |";
  pStr << std::setfill(' ') << std::setw(21) << mSentTMin << "|" << std::setfill(' ');
  pStr << std::setfill(' ') << std::setw(21) << mRecvTMin <<"|" << std::setfill(' ');
  pStr << std::setfill(' ') << std::setw(21) << mDelayMin  << "|" << std::setfill(' ');
  pStr << std::setfill(' ') << std::setw(12) << CStringUtils::decades_formatted(mPktSizeMin)
          << std::setfill(' ') << std::endl;

  pStr << "     SeqNo     |";
  pStr << std::setfill(' ') << std::setw(21)
  << CStringUtils::decades_formatted(mSentMinPos)
          << "|" << std::setfill(' ');
  pStr << std::setfill(' ') << std::setw(21)
  << CStringUtils::decades_formatted(mRecvMinPos)
          << "|" << std::setfill(' ');
  pStr << std::setfill(' ') << std::setw(21)
  << CStringUtils::decades_formatted(mDelayMinPos)
          << "|"  << std::setfill(' ');
  pStr << std::setfill(' ') << std::setw(12)
          << CStringUtils::decades_formatted(mSizeMinPos)     << std::setfill(' ') << std::endl;

  pStr << "----------------------------------------------------------------------------------------------"
          << std::endl;
  pStr << "     Max       |";
  pStr << std::setfill(' ') << std::setw(21) << mSentTMax << "|";
  pStr << std::setfill(' ') << std::setw(21) << mRecvTMax << "|";
  pStr << std::setfill(' ') << std::setw(21) << mDelayMax << "|";
  pStr << std::setfill(' ') << std::setw(12) << CStringUtils::decades_formatted(mPktSizeMax) <<  std::endl;

  pStr << "     SeqNo     |";
  pStr << std::setfill(' ') << std::setw(21) << CStringUtils::decades_formatted(mSentMaxPos)
          << "|" << std::setfill(' ');
  pStr << std::setfill(' ') << std::setw(21) << CStringUtils::decades_formatted(mRecvMaxPos)
          << "|" << std::setfill(' ');
  pStr << std::setfill(' ') << std::setw(21) << CStringUtils::decades_formatted(mDelayMaxPos)
          << "|" << std::setfill(' ');
  pStr << std::setfill(' ') << std::setw(12) << CStringUtils::decades_formatted(mSizeMaxPos)
          << std::setfill(' ') <<  std::endl;
  pStr << "----------------------------------------------------------------------------------------------"
          << std::endl;

  pStr << "     Mean      |";
  pStr << std::setfill(' ') << std::setw(21) << mSentAvg << "|" << std::setfill(' ');
  pStr << std::setfill(' ') << std::setw(21) << mRecvAvg << "|" << std::setfill(' ');
  pStr << std::setfill(' ') << std::setw(21) << HPTimer::HighPerTimer::DtoHPTimer(mDelayAvg) << "|" << std::setfill(' ');
  pStr << std::setfill(' ') << std::setw(12) << CStringUtils::decades_formatted((long)mPackAvg)
          << std::setfill(' ') << std::endl;
  pStr << "----------------------------------------------------------------------------------------------"
          << std::endl;

  pStr << " St. deviation |";
  pStr << std::setfill(' ') << std::setw(21) << mSentStdv << "|" << std::setfill(' ');
  pStr << std::setfill(' ') << std::setw(21) << mRecvStdv << "|" << std::setfill(' ');
  pStr << std::setfill(' ') << std::setw(21) << mDelayStdv << "|" << std::setfill(' ');
  pStr << std::setfill(' ') << std::setw(12) << mSizeStdv << std::setfill(' ') <<  std::endl;
  pStr << "----------------------------------------------------------------------------------------------\n";
  pStr << "A time correction by " << mDeltaTime << " accroding to the sender session initialization\n";
  pStr << "is applied to the printed results" << std::endl;
  
}

// __END__
