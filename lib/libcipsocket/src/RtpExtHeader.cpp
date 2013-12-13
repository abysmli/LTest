/**
* \file stream4kRtpExt.cpp
*
* \brief Handle Thomson's extended RTP header
*
* <b>THOMSON 4k Streaming Demonstrator</b>
*
* long description
*
* \par COPYRIGHT NOTICE
*
* Copyright 2007 (c) DTB
*
* DTB owns the sole copyright to this software. Under international copyright laws you
* (1) may not make a copy of this software except for the purposes of maintaining a single
* archive copy, (2) may not derive works here from, (3) may not distribute this work to
* others. These rights are provided for information clarification, other restrictions of
* rights may apply as well.
*
* This is an unpublished work.
*
* \par Warranty
*
* DTB MAKES NO WARRANTY OF ANY KIND WITH REGARD TO THE USE OF THIS SOFTWARE, EITHER
* EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
*
* \author  Eduard Siemens
*
* \version Draft
*/

/*****************************************************************************************
* Source control information (updated by Merant Version Manager on check-in)
* ----------------------------------------------------------------------------------------
*  $Archive::
* ----------------------------------------------------------------------------------------
*   $Author:   koehlerr  $
*     $Date:   07 Apr 2007 14:24:18  $
*  $Modtime:   07 Apr 2007 14:23:48  $
* $Revision:   1.10  $
*****************************************************************************************/
#include "RtpExtHeader.h"


/// constructor
stream4kRtpExtHeader::stream4kRtpExtHeader() :
		mLastPayloadLen ( 0 )
{}

/// destructor
stream4kRtpExtHeader::~stream4kRtpExtHeader()
{}

/// init, set initial sequence number as well as the payload type
void
stream4kRtpExtHeader::Initialize ( unsigned char pPType,
                                   unsigned char pVersion )
{
	mRtpExtHeader.version = pVersion;
	mRtpExtHeader.p = 0;	  // no padding
	mRtpExtHeader.x = 0;	  // no extension
	mRtpExtHeader.cc = 0;   // no contributing sources
	mRtpExtHeader.m = 0;    // marker bit, should be set by the application
	mRtpExtHeader.pt = pPType;  // no payload nbr defined for dpx, we may choose our own
	// mRtpExtHeader.seq = 0xAAFF;	// sequence number; should be init with a random value (more secure)
	mRtpExtHeader.ts = 0; 	// timestamp
	mRtpExtHeader.ssrc = 0xDEAD; // synchronization source arbitrary value
	// mRtpExtHeader.xseq = 0xFFFF; // devil hack! Random number should be
	mRtpExtHeader.xts = 0; // no timestamps used
	mRtpExtHeader.b = 0; // set by user
	mRtpExtHeader.e = 0; // set by user
	mRtpExtHeader.tct = 0; // currently not used
	mRtpExtHeader.rsv = 0;
	mRtpExtHeader.smpte_tc = 0; // set by user
	mRtpExtHeader.off = 0; // set by user
	
	SetSeqNo(0xFFFFFFAA);
}

/// assemble the rtp header and set the iovec to point to it
void
stream4kRtpExtHeader::AssembleRtpExtHeader ( iovec & pIOV,
        uint32_t pPayloadLen,
        uint32_t pSMPTETime,
        bool pBFrame,
        bool pEFrame )
{

	mRtpExtHeader.smpte_tc = ( htonl ( pSMPTETime ) ) >> 8;
	mRtpExtHeader.b = pBFrame;
	mRtpExtHeader.e = pEFrame;
	uint32_t seqno ( SeqNo() );

	if ( pBFrame )
	{
		mRtpExtHeader.off = 0;
	}
	else
	{
		uint32_t offset ( ntohl ( mRtpExtHeader.off ) );
		mRtpExtHeader.off = htonl ( offset + mLastPayloadLen );
	}

	SetSeqNo ( ++seqno );
	pIOV.iov_base = &mRtpExtHeader;
	pIOV.iov_len = stream4kRtpExtHeader::mRtpExtHeaderLength;
	// save the last payload len, to calculate the correct DPX
	// frame offset
	mLastPayloadLen = pPayloadLen;
}

/// assemble the rtp header and set the iovec to point to it
/// begin and end marks will be explicitly set to zero,
/// SMPTE time will be unchanged
void
stream4kRtpExtHeader::AssembleRtpExtHeader ( iovec & pIOV, uint32_t pPayloadLen )
{
	mRtpExtHeader.b = 0;
	mRtpExtHeader.e = 0;
	uint32_t seqno ( SeqNo() );
	SetSeqNo ( ++seqno );
	pIOV.iov_base = &mRtpExtHeader;
	pIOV.iov_len = stream4kRtpExtHeader::mRtpExtHeaderLength;
	uint32_t offset ( ntohl ( mRtpExtHeader.off ) );
	mRtpExtHeader.off = htonl ( offset + pPayloadLen );
}

// get timestamp
uint64_t
stream4kRtpExtHeader::GetTimestamp()
{
	uint64_t timestamp ( mRtpExtHeader.ts << 16 );
	timestamp |= ( mRtpExtHeader.xts & 0xFFFF );
	return timestamp;
}

// set timestamp
void
stream4kRtpExtHeader::SetTimestamp ( uint64_t pTime )
{
	pTime &= 0x00FFFFFFFFFFFFLL; // timestamp is only 48 bits wide
	mRtpExtHeader.ts = pTime >> 16; // high order
	mRtpExtHeader.xts = pTime & 0xFFFF;
	return;
}


