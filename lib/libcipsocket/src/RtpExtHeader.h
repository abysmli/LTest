/**
* \file stream4kRtpExtHeader.h
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
*   $Author:   siemense  $
*     $Date:   02 Apr 2007 12:37:20  $
*  $Modtime:   02 Apr 2007 09:57:06  $
* $Revision:   1.11  $
*****************************************************************************************/

#ifndef stream4kRtpExtHeader_H_
#define stream4kRtpExtHeader_H_

// #include "ace/Log_Msg.h"

#include <sys/types.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <iostream>




class stream4kRtpExtHeader
{
	public:
		///constructor
		stream4kRtpExtHeader();
		///destructor
		virtual ~stream4kRtpExtHeader();
		/// init: set initial sequence number, payload type and RTP protocol (temporarily use 0x60 for DPX)
		void Initialize ( unsigned char pPType = 0x60, unsigned char pVersion = 2 );

		/// assemble an RTP header and let iovec structure point to this header.
		/// Especially, set begin and end bits, as well as the SMPTE time stamp
		/// and increment the sequence number
		/// assemble an RTP header and let iovec structure point to this header.
		/// @param pIOV is the iovec struct, the RTP header shold be stored
		/// in.
		/// @param pPayloadLen is the lengh of the payload to be sent.
		/// This is necessary for the extended RTP header for the offset
		/// calculation

		void AssembleRtpExtHeader ( struct iovec & pIOV, uint32_t pPayloadLen, uint32_t pSMPTETime,
		                            bool pBFrame, bool pEFrame );

		/** assemble an RTP header and let iovec structure point to this header.
		* Especially increment the sequence number
		* \b bedin and \b end flags will be explicitly set to false
		* \b SMPTETime will be unchanged.
		 * So only first and last packets of DPX frame must be assembled, 
		 * using the full parameter list. All other can use the short one
		*/
		void AssembleRtpExtHeader ( struct iovec & pIOV, uint32_t pPayloadLen );

		/** calculate the distance between two sequence numbers
		 * we pay attention for possible wrap around
		 */

		inline uint32_t SeqNoDistance ( const uint32_t pSeq1, const uint32_t pSeq2 )
		{
			// to
			int64_t llSeqDiff ( pSeq2 - pSeq1 );
			uint64_t SeqNoRange ( 1UL<<32 );
			llSeqDiff += SeqNoRange;
			return static_cast<uint32_t> ( llSeqDiff % SeqNoRange );
		};

		/** calculate the distance between two sequence numbers
		* we pay attention for possible wrap around
		*/

		inline uint32_t SMPTEDistance ( const uint32_t pT1, const uint32_t pT2 )
		{
			// to
			int32_t TDiff ( pT2 - pT1 );
			uint32_t SMPTERange ( 1<<24 );
			TDiff += SMPTERange;
			return static_cast<uint32_t> ( TDiff % SMPTERange );
		};


	protected:
		// unfortunately, this packing is very plattform specifig. So here, fields within a byte
		// have to be positioned wit LSB first!!!
		// The rtp header
		struct RtpExtHeader
		{
		uint32_t cc:4;         /* CSRC count */
		uint32_t x:1;          /* header extension flag */
		uint32_t p:1;          /* padding flag */
		uint32_t version:2;    /* protocol version */
		uint32_t pt:7;         /* payload type */
		uint32_t m:1;          /* marker bit */
		uint32_t seq:16;       /* sequence number */
		uint32_t ts:32;                /* timestamp */
		uint32_t ssrc:32;              /* synchronization source */
		uint32_t xseq:16;      /* extended sequence number */
		uint32_t xts:16;       /* extended time stamp */
		uint32_t rsv:4;        /* reserved bits */
		uint32_t tct:2;        /* time code type */
		uint32_t e:1;          /* DPX frame end */
		uint32_t b:1;          /* DPX frame begin */
		uint32_t smpte_tc:24;  /* SMPTE time code */
		uint32_t off:32;                /* offset to current DPX frame */
		}
		mRtpExtHeader;

	public:
		/// Set iovec pointer to the RTP header. @returns true if successful
		inline void SetIOV ( struct iovec & pIOV )
		{
			pIOV.iov_base = &mRtpExtHeader;
			pIOV.iov_len = stream4kRtpExtHeader::mRtpExtHeaderLength;
		};

		/// get the sequence number of the RTP header in host byte order
		inline uint32_t SeqNo()
		{
			uint32_t seqno ( ntohs ( mRtpExtHeader.xseq ) << 16 );
			seqno |= ( ntohs ( mRtpExtHeader.seq & 0xFFFF ) );
			return seqno;
		};

		/// get and set the version number
		inline unsigned int Version()
		{
			return mRtpExtHeader.version;
		};

		/// length of the extended RTP header
		inline long RtpExtHeaderLength()
		{
			return mRtpExtHeaderLength;
		};


		/// the byte offest within the DPX file
		inline uint32_t Offset()
		{
			return ntohl ( mRtpExtHeader.off );
		}

		/// get the SMPTE time code - updates onse per DPX frame
		/// in our case, we will increment by one per DPX frame
		inline long SMPTETime()
		{
			return ( ntohl ( mRtpExtHeader.smpte_tc ) >> 8 );
		};

		/// get the SMPTE time code - updates onse per DPX frame
		/// in our case, we will increment by one per DPX frame
		inline void SMPTETime ( long pSMPTECount )
		{
			mRtpExtHeader.smpte_tc = htonl ( pSMPTECount );
		};

		/// DPX frame begin getter
		inline bool DPXFrameBegin()
		{
			return mRtpExtHeader.b;
		}

		/// DPX frame begin setter
		inline void DPXFrameBegin ( bool pBegin )
		{
			mRtpExtHeader.b = pBegin;
		}

		/// DPX frame end getter
		inline bool DPXFrameEnd()
		{
			return mRtpExtHeader.e;
		}

		/// DPX frame end setter
		inline void DPXFrameEnd ( bool pEnd )
		{
			mRtpExtHeader.e = pEnd;
		}

		/// Get the timestamp as uint64_t plain - no byte reordering performed
		uint64_t GetTimestamp();

		/// Set the timestamp as uint64_t (as defined by RTP and RTP extension
		/// no byte reordering will be performed
		void SetTimestamp ( uint64_t pTime );

		/// print out header fields
		void Dump ( void )
		{
		  std::cout << std::hex << "stream4kRtpExtHeader: (in hex) ** "
			<< " cc:4 " <<  mRtpExtHeader.cc       /* CSRC count */
			<< " x:1 "  << mRtpExtHeader.x         /* header extension flag */
			<< " p:1 "  << mRtpExtHeader.p         /* padding flag */
			<< " version:2 " << mRtpExtHeader.version   /* protocol version */
			<< " pt:7 " << mRtpExtHeader.pt        /* payload type */
			<< " m:1 " << mRtpExtHeader.m          /* marker bit */
			<< " seq:16 " << mRtpExtHeader.seq     /* sequence number */
			<< " ts:32 " << mRtpExtHeader.ts      /* timestamp */
			<< " ssrc:32 " << mRtpExtHeader.ssrc   /* synchronization source */
			<< " xseq:16 " << mRtpExtHeader.xseq  /* extended sequence number */
			<< " xts:16 " << mRtpExtHeader.xts      /* extended time stamp */
			<< " rsv:4 " << mRtpExtHeader.rsv     /* reserved bits */
			<< " tct:2 " << mRtpExtHeader.tct       /* time code type */
			<< " e:1 " << mRtpExtHeader.e         /* DPX frame end */
			<< " b:1 " << mRtpExtHeader.b         /* DPX frame begin */
			<< " smpte_tc:24 " << mRtpExtHeader.smpte_tc /* SMPTE time code */
		    << " off:32 " << mRtpExtHeader.off << std::dec <<  std::endl;                /* offset to current DPX frame */
		};


	private:
		/// set the sequence number, given in host byte order
		inline void SetSeqNo ( uint32_t pSeqNo )
		{
			mRtpExtHeader.xseq = pSeqNo >> 16; // high bytes
			mRtpExtHeader.xseq = htons ( mRtpExtHeader.xseq );
			mRtpExtHeader.seq =  pSeqNo & 0x0000FFFF ; //low bytes
			mRtpExtHeader.seq = htons ( mRtpExtHeader.seq );
		}

		size_t mLastPayloadLen;

	public:
		static const long mRtpExtHeaderLength = 24;

                                        };
#endif // stream4kRtpExtHeader_H_
