#include <unistd.h>
#include <sched.h>
#include <stdexcept>
#include <sys/mman.h>
#include <string.h>
#include "strtok.h"
#include "HighPerTimer.h"
#include "config.h"
#include "SessionIf_impl.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FlowIF_impl class implementation
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///ctor
FlowIF_impl::FlowIF_impl() :
		mBlocked(false), mSessionCounter(0), mSSession(NULL), mRSession(NULL)
{
	mBlockedM = ( pthread_mutex_t
			) PTHREAD_MUTEX_INITIALIZER;
	std::cerr << "FlowIF created" << std::endl;

}

///dctor
FlowIF_impl::~FlowIF_impl()
{
	std::cerr << "FlowIF destroyed" << std::endl;
}

std::string FlowIF_impl::print_sDesc(struct sourceDesc_t &sDesc)
{
	std::stringstream Str;
	Str << "Start time: " << sDesc.StartTime.tv_sec << "."
			<< sDesc.StartTime.tv_usec << "\n" << "Stop time: "
			<< sDesc.StopTime.tv_sec << "." << sDesc.StopTime.tv_usec << "\n"
			<< "# of Samples: " << sDesc.sample_n << "\n"
			<< "listening Address: " << CIPAddress::atodd(sDesc.ListenAddr)
			<< "\nListeninig Port: " << sDesc.ListenPort << "\nProto: ";

	if (sDesc.Proto == P_TCP)
		Str << "TCP";
	else if (sDesc.Proto == P_UDP)
	{
		Str << "UDP";
	}
	else if (sDesc.Proto == P_UDT)
	{
		Str << "UDT";
	}
	else if (sDesc.Proto == P_FastTCP)
	{
		Str << "Fast TCP";
	}
	else if (sDesc.Proto == P_HSTCP)
	{
		Str << "High Speed TCP";
	}
	else
	{
		Str << "UNKNOWN Protocol !";
	}
	Str << "\n_____________________\n" << std::endl;
	return Str.str();

}

/// the start sender session routine
void FlowIF_impl::start_ssession(struct sourceDesc_t &sDec,
		struct senderDesc_t &senderDesc)
{

	if (this->Blocked() || (sDec.Block && mSessionCounter > 0))
	{
		throw CNetwork_exception("blocked", mSessionCounter);
	}
	mSessionCounter++;
	std::cout << "start new sender session\n" << print_sDesc(sDec) << std::endl;
	this->Blocked(true);
	mBlockTill = senderDesc.FinishTime;
	mSSession = new SSessionIf_impl(senderDesc, this);
	mSSession->run_sender(senderDesc, sDec);
	//std::string intermediateURL = senderDesc.RemoteURL;
	//ltestProxy proxy(intermediateURL.c_str(),SOAP_IO_KEEPALIVE);

}

/// the start session routine, in that the data receiver runs
void FlowIF_impl::start_rsession(struct sourceDesc_t & sDesc)
{
	if (this->Blocked() || (sDesc.Block && mSessionCounter > 0))
	{
		throw CNetwork_exception("blocked", mSessionCounter);
	}
	mSessionCounter++;
	std::cout << "start new session\n" << print_sDesc(sDesc) << std::endl;
	if (sDesc.Block)
	{
		this->Blocked(true);
		mBlockTill = sDesc.StopTime;
	}
	mRSession = new RSessionIf_impl(sDesc, this);
	mRSession->createThread(sDesc.Prio);

}

RSessionIf_impl *
FlowIF_impl::GetRSessionPtr()
{

	return mRSession;
}

SSessionIf_impl *
FlowIF_impl::GetSSessionPtr()
{
	return mSSession;
}

/// decrement the session counter
int32_t FlowIF_impl::decrement_session_counter()
{
	if ((--mSessionCounter) == 0)
	{
		this->Blocked(false);
	}
	return mSessionCounter;

}

/// accessor to Blocked() routine
bool FlowIF_impl::Blocked()
{
	pthread_mutex_lock(&mBlockedM);
	bool Tmp = mBlocked;
	pthread_mutex_unlock(&mBlockedM);
	return Tmp;

}

void FlowIF_impl::Exit()
{
	exit(0);
}

/// accessor to Blocked() routine
void FlowIF_impl::Blocked(bool pBlock)
{
	pthread_mutex_lock(&mBlockedM);
	mBlocked = pBlock;
	pthread_mutex_unlock(&mBlockedM);
}

/// ping method for testing availability of the server
void FlowIF_impl::ping()
{
	std::cerr << "ping call issued" << std::endl;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SessionIf_impl class implementation
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 wrappers for starting new threads
 */
extern "C"
{

void *new_thread(void *pClass)
{
	try
	{
		((RSessionIf_impl *) pClass)->Run();
		std::cerr << "finish RSessionIf_impl->Run thread..." << std::endl;
	} catch (CNetwork_exception & ex)
	{
		std::cout
				<< "Network exception caught in the RSession->Run() routine! Reason: "
				<< ex.what() << " Error code: " << ex.code() << " Glibc errno: "
				<< ex.sys_errno() << "\nThe Run thread will be finished now. "
				<< std::endl;

	} catch (...)
	{
		std::cout << "unknown exception caught within the "
				<< "RSessionIf_impl->Run thread..." << std::endl;

	}
	((RSessionIf_impl *) pClass)->close_socket();
	return 0;
}
}

extern "C"
{
void *UDP_time_thread(void *pClass)
{
	try
	{
		std::cerr << "New UDP_time_port thread created: " << std::endl;
		((RSessionIf_impl *) pClass)->run_UDP_time();
		std::cerr << "finish RSessionIf_impl->run_UDP_time thread..."
				<< std::endl;
	} catch (CNetwork_exception & ex)
	{
		std::cout << "Network exception caught! " << ex.what() << " Error: "
				<< ex.code() << std::endl;

	} catch (...)
	{
		std::cout << "unknown exception caught within the "
				<< "RSessionIf_impl->run_UDP_time thread..." << std::endl;
		;
	}
	return 0;
}
}

///SessionIf_impl ctor: set the port number
SessionIf_impl::SessionIf_impl(FlowIF_impl * pFlowIF_ptr) :
		mSocket(NULL), mDataSource(NULL), mFinished(false), mFreezed(false), mFlowIF_ptr(
				pFlowIF_ptr)
{
	mFreezedM = ( pthread_mutex_t
			) PTHREAD_MUTEX_INITIALIZER;
	mFreezedC = ( pthread_cond_t
			) PTHREAD_COND_INITIALIZER;
}

/// dtor
SessionIf_impl::~SessionIf_impl()
{
}

/// get allocated Port number
uint16_t SessionIf_impl::Port()
{
	return mServerPort;
}

/// close the pertinent socket of the session
void SessionIf_impl::close_socket()
{
	if (NULL != mSocket)
	{
		mSocket->Close();
	}
}

/**
 create a new thread and start a session routune in this thread (server-side)
 */
pthread_t RSessionIf_impl::createThread(int pPrio)
{
	pthread_t tID;
	int rValue(0);
	pthread_attr_t tAttr;
	pthread_attr_init(&tAttr);
	pthread_attr_t * tAttr_ptr = &tAttr;
	if (getuid() == 0)
	{
		pthread_attr_setschedpolicy(&tAttr, SCHED_RR);
		struct sched_param tParam;
		memset(&tParam, 0, sizeof(tParam));
		pthread_attr_getschedparam(&tAttr, &tParam);
		tParam.sched_priority =
				pPrio == -1 ? sched_get_priority_min(SCHED_RR) : pPrio;
		pthread_attr_setschedparam(&tAttr, &tParam);
	}
	else
	{
		tAttr_ptr = NULL;
	}
	// thread should not be joint
	pthread_attr_setdetachstate(&tAttr, PTHREAD_CREATE_DETACHED);

	if ((rValue = pthread_create(&tID, tAttr_ptr, &new_thread, (void*) this))
			!= 0)
		throw CNetwork_exception(
				"can't open new thread. Return value: "
						+ CStringUtils::itos(rValue));
	std::cout << "New data receiver thread created. Thread ID is:\t" << tID
			<< std::endl;
	return tID;
}

/**
 create a new thread and start a session routune in this thread (server-side)
 */
pthread_t SSessionIf_impl::createThread(int pPrio)
{
	pthread_t tID;
	int rValue(0);
	pthread_attr_t tAttr;
	pthread_attr_init(&tAttr);
	pthread_attr_t * tAttr_ptr = &tAttr;
	if (getuid() == 0)
	{
		pthread_attr_setschedpolicy(&tAttr, SCHED_RR);
		struct sched_param tParam;
		memset(&tParam, 0, sizeof(tParam));
		pthread_attr_getschedparam(&tAttr, &tParam);
		tParam.sched_priority =
				pPrio == -1 ? sched_get_priority_min(SCHED_RR) : pPrio;
		// tParam.sched_priority = sched_get_priority_max(SCHED_RR);
		pthread_attr_setschedparam(&tAttr, &tParam);
	}
	else
	{
		tAttr_ptr = NULL;
	}

	// thread should not be joint
	pthread_attr_setdetachstate(&tAttr, PTHREAD_CREATE_DETACHED);

	if ((rValue = pthread_create(&tID, tAttr_ptr, &new_thread, (void*) this))
			!= 0)
		throw CNetwork_exception(
				"can't open new thread. Return value: "
						+ CStringUtils::itos(rValue));
	std::cout << "new thread created for data receiver. PID:\t" << getpid()
			<< std::endl;
	return tID;
}

/// ctor for creating a remote sender session
SSessionIf_impl::SSessionIf_impl(struct senderDesc_t & senderDesc,
		FlowIF_impl *pFlowIF) :
		SessionIf_impl(pFlowIF)
{
	/// initialize intermediate orb for the communication channel between sender server
	/// and receiver server
	std::string intermediateURL = senderDesc.RemoteURL;
	rproxy = new ltestProxy(intermediateURL.c_str(), SOAP_IO_KEEPALIVE);
}

///dtor for sender session, delete the data sender
SSessionIf_impl::~SSessionIf_impl()
{
	if (mDataSource)
		delete mDataSource;
}

/// run the sender routine till gets results from the receiver
bool SSessionIf_impl::run_sender(struct senderDesc_t & senderDesc,
		struct sourceDesc_t & sDesc)
{
	if (rproxy != NULL)
	{
		/// connect to the receiver object
		struct StartRSessionResponse RStartRSessRes;
		if (SOAP_OK != rproxy->start_rsession(sDesc, RStartRSessRes))
			rproxy->soap_stream_fault(std::cerr);

		/// time correction related stuff
		bool TimeCorrectionPerformed(false);
		HPTimer::HighPerTimer DeltaTime, DeltaTime_pos;
		uint16_t rPort;
		if (SOAP_OK != rproxy->Port(rPort))
			rproxy->soap_stream_fault(std::cerr);
		uint32_t lHost =
				senderDesc.LocalAddr == 0 ? INADDR_ANY : senderDesc.LocalAddr;

		/// convert tv_type timers in CTSCTimer format
		HPTimer::HighPerTimer StartTime(
				static_cast<int64_t>(senderDesc.StartTime.tv_sec),
				static_cast<int64_t>(senderDesc.StartTime.tv_usec) * 1000);
		HPTimer::HighPerTimer Duration(
				static_cast<int64_t>(senderDesc.Duration.tv_sec),
				static_cast<int64_t>(senderDesc.Duration.tv_usec) * 1000);
		HPTimer::HighPerTimer FinishTime(
				static_cast<int64_t>(senderDesc.FinishTime.tv_sec),
				static_cast<int64_t>(senderDesc.FinishTime.tv_usec) * 1000);

		uint16_t lPort(senderDesc.LocalPort);
		uint32_t rAddr(senderDesc.RemoteAddr);
		uint32_t Bytes(senderDesc.Bytes);
		uint32_t Packets(senderDesc.Packets);
		int Prio(senderDesc.Priority);
		std::string FifoName(senderDesc.FifoName);

		/// initialize the socket option map extract socket options from
		/// the sDest to the sockOptsMap

		SockOptsMap SockOpts;
		for (uint32_t i = 0; i < senderDesc.SockOpts.size(); i++)
		{
			SockOpts[sDesc.SockOpts.at(i).key] = sDesc.SockOpts.at(i).value;
		}

		if (senderDesc.TimeCorrection)
		{
			CTimeCorrection * timeCorr = NULL;
			try
			{
				std::cerr << "Start UDP time correction: " << std::endl;
				CIPAddress rUDPTimeAddr(rAddr, 0);
				timeCorr = new CUDPTimeCorrection(rproxy, rUDPTimeAddr,
						senderDesc.MaxTimeDev);
				if ((TimeCorrectionPerformed = timeCorr->PerformCorrection())
						== true)
				{
					DeltaTime = timeCorr->CalcDelta();
					std::cout
							<< "Time correction via UDP performed. Correction result is: "
							<< DeltaTime << " s\n";
					timeCorr->mCorrStats.print_out(std::cout);
					std::cout << std::endl;
				}
				if (timeCorr != NULL)
				{
					delete timeCorr;
					timeCorr = NULL;
				}

			} catch (const CTimeCorrect_exception & ex)
			{
				std::cerr << "WARNING! CTimeCorrect_exception caught within "
						<< __PRETTY_FUNCTION__ << ":" << ex.what() << "Code: "
						<< ex.code() << std::endl;
				if (timeCorr != NULL)
				{
					delete timeCorr;
					timeCorr = NULL;
				}

			}

			catch (const CNetwork_exception & ex)
			{
				std::cerr << "WARNING! CNetwork_exception caught within "
						<< __PRETTY_FUNCTION__ << ": " << ex.what() << " Code: "
						<< ex.code() << std::endl;
				if (timeCorr != NULL)
				{
					delete timeCorr;
					timeCorr = NULL;
				}
			}

			if (!TimeCorrectionPerformed)
			{
				try
				{
					timeCorr = new GSOAPTimeCorrection(rproxy,
							senderDesc.MaxTimeDev);
					if ((TimeCorrectionPerformed = timeCorr->PerformCorrection())
							== true)
					{
						DeltaTime = timeCorr->CalcDelta();
						std::cout
								<< "Time correction via GSOAP performed. Correction result is:"
								<< DeltaTime << " s \n";
						timeCorr->mCorrStats.print_out(std::cout);
						std::cout << std::endl;
						DeltaTime_pos=timeCorr->mCorrStats.ICADeltaSendTime;
					}
					if (timeCorr != NULL)
					{
						delete timeCorr;
						timeCorr = NULL;
					}
				} catch (const CNetwork_exception & ex)
				{
					std::cerr << "WARNING! CNetwork_exception caught within "
							<< __PRETTY_FUNCTION__ << ": " << ex.what()
							<< " Code: " << ex.code() << std::endl;
					if (timeCorr != NULL)
					{
						delete timeCorr;
						timeCorr = NULL;
					}
				}
			}

		}

		if (senderDesc.SType == ST_FIFO)
		{
			mDataSource = new CFIFOSource(lHost, lPort, rAddr, rPort, Bytes,
					Packets, senderDesc.Proto, StartTime, Duration, FinishTime,
					Prio, FifoName, senderDesc.TOS, SockOpts);

		}
		else if (senderDesc.SType == ST_CBR)
		{
			mDataSource = new CCBRSource(lHost, lPort, rAddr, rPort, Bytes,
					Packets, senderDesc.Proto, senderDesc.BPS, senderDesc.Size,
					StartTime, Duration, FinishTime, Prio, senderDesc.TOS,
					SockOpts);
		}

		// full speed source
		else if (senderDesc.SType == ST_FS)
		{
			std::cerr << "Create Full Speed source " << "\n" << std::endl;
			mDataSource = new CFullSpeedSource(lHost, lPort, rAddr, rPort,
					Bytes, Packets, senderDesc.Proto, senderDesc.Size,
					StartTime, Duration, FinishTime, Prio, senderDesc.TOS,
					SockOpts);

		}
		// poisson data source
		else if (senderDesc.SType == ST_POISSON)
		{
			mDataSource = new CPoissonSource(lHost, lPort, rAddr, rPort, Bytes,
					Packets, senderDesc.Proto, senderDesc.BPS, senderDesc.Size,
					StartTime, Duration, FinishTime, Prio, senderDesc.TOS,
					SockOpts);
			std::cout << "poisson source discipline will be applied !"
					<< std::endl;
		}

		if (TimeCorrectionPerformed)
		{
			mDataSource->SetDeltaTime(DeltaTime,DeltaTime_pos);
		}

		while (!mDataSource->Finished())
		{
			mDataSource->Dispatch();
		}

		if (mDataSource == NULL)
		{
			return false;   // data source is died
		}

		else
		{
			mDataSource->CloseSocket();
			FinishSessionResponse RFinishSessRes;
			if (SOAP_OK
					!= rproxy->finish_session(mDataSource->LastSeqNo(),
							RFinishSessRes))
				rproxy->soap_stream_fault(std::cerr);
			std::cout << "sender session finished, will try to fetch results"
					<< std::endl;
			std::vector<sample_t> RSampleList;
			/// get all samples from the receiver and store them in the data sender structure
			for (int k(0); true; k++)
			{

				if (SOAP_OK != rproxy->get_samples(RSampleList))
					rproxy->soap_stream_fault(std::cerr);
				for (unsigned long i(0); i < RSampleList.size(); i++)
				{
					mDataSource->StoreRSample(RSampleList.at(i).Header);
				}
				// if list don't have full length, then no more samples available in the receiver
				// just finish the receiving loop
				if (RSampleList.size() < 1000)
					break;
				std::cout << "." << std::flush;
				if ((k % 50) == 0)
					std::cout << std::endl;
			}
			mDataSource->Freeze();
			pthread_mutex_lock(&mFreezedM);
			mFreezed = true;
			pthread_cond_broadcast(&mFreezedC);
			pthread_mutex_unlock(&mFreezedM);
			ReleaseRSessionResponse RReleaRSessRes;
			rproxy->release_rsession(RReleaRSessRes);

		}

		delete rproxy;
		return true;

	}

	return false;

}

/// delete the sender session structure
void SSessionIf_impl::release_session()
{
	if (mDataSource != NULL)
		delete mDataSource;
	mFlowIF_ptr->decrement_session_counter();
	if (mSocket != NULL)
	{
		delete mSocket;
		mSocket = NULL;
	}

}

/**
 * 
 */
void SSessionIf_impl::get_samples(
		std::vector<struct whole_sample_t> & whole_sample_list_t)
{

	CPDUHeader * Header_ptr;
	/// handle freeze and finish mutexes and conditions
	pthread_mutex_lock(&mFreezedM);
	if (mFreezed == false)
	{
		pthread_cond_wait(&mFreezedC, &mFreezedM);
	}
	else
	{
		pthread_mutex_unlock(&mFreezedM);
	}

	/// fill the sample list
	unsigned long len(0);
	for (unsigned long i(0); i < SAMPLE_LIST_MAX; i++)
	{
		if ((Header_ptr = mDataSource->getNextHeader()) != 0)
		{
			// FIXME: we have to initialize this sample at once. And copy of temporary. Move semantics
			len++;
			whole_sample_t mysample =
			{ Header_ptr->Header[0], Header_ptr->Header[1],
					Header_ptr->Header[2], Header_ptr->Header[3],
					Header_ptr->Header[4], Header_ptr->Header[5],
					Header_ptr->Header[6], Header_ptr->Header[7],
					Header_ptr->Header[8], Header_ptr->Header[9],
					Header_ptr->Header[10], Header_ptr->Header[11],
					Header_ptr->Header[12], Header_ptr->Header[13],
					Header_ptr->Header[14], Header_ptr->Header[15],
					Header_ptr->Header[16], Header_ptr->Header[17],
					Header_ptr->Header[18], Header_ptr->Header[19],
					Header_ptr->Header[20], Header_ptr->Header[21],
					Header_ptr->Header[22], Header_ptr->Header[23]

			};
			whole_sample_list_t.push_back(mysample);
		}
		else
		{
			break;
		}

	}

}

/// get basic statistics of the source
BasicSourceStats_t SSessionIf_impl::getBasicSourceStats()
{
	BasicSourceStats_t Stats;
	Stats = mDataSource->getBasicStats();
	return Stats;
}

/// ctor for the receiver session
RSessionIf_impl::RSessionIf_impl(struct sourceDesc_t & sDesc,
		FlowIF_impl * pFlowIF) :
		SessionIf_impl(pFlowIF), mUDPTimeSocket_ptr(NULL), mDesc(sDesc), mReceiver(
				NULL), mLastSeqNo(0), mUDPTimeFinished(false), mRecvFinished(
				false)
{
	try
	{
		if (sDesc.Proto == P_TCP)
		{
			mSocket = new CTCPSocket(true, sDesc.ListenPort, sDesc.ListenAddr);
			if (sDesc.maxPDUSize != 0)
			{
				// be carefull because of may be exhausted memory
				try
				{
					mReceiver = new CTCPReceiver(mSocket, sDesc.sample_n,
							sDesc.maxPDUSize);
				} catch (const std::bad_alloc )
				{
					throw CNetwork_exception(
							"Memory exhausted! Could not instantiate the TCP receiver",
							11);
				}
			}
			else
			{
				mReceiver = new CTCPReceiver(mSocket, sDesc.sample_n);
			}
		}
		else if (sDesc.Proto == P_UDP)
		{
			mSocket = new CUDPSocket(true, sDesc.ListenPort, sDesc.ListenAddr);
			mReceiver = new CUDPReceiver(mSocket, sDesc.sample_n);
		}
		else if (sDesc.Proto == P_UDT)
		{

			// extract socket options from the sDest to the SockOptsMap
			SockOptsMap SockOpts;
			for (uint32_t i(0); i < sDesc.SockOpts.size(); i++)
			{
				SockOpts[std::string(sDesc.SockOpts.at(0).key)] =
						sDesc.SockOpts.at(0).value;
			}

		}
		else if (sDesc.Proto == P_FastTCP)
		{
			mSocket = new CFastTCPSocket(true, sDesc.ListenPort,
					sDesc.ListenAddr);
			mReceiver = new CFastTCPReceiver(mSocket, sDesc.sample_n);
		}
		else if (sDesc.Proto == P_HSTCP)
		{
			mSocket = new CHSTCPSocket(true, sDesc.ListenPort,
					sDesc.ListenAddr);
			mReceiver = new CHSTCPReceiver(mSocket, sDesc.sample_n);
		}

#ifdef SDP_ENABLED
		else if ( sDesc.Proto == P_SDP )
		{
			mSocket = new CSDPSocket ( true, sDesc.ListenPort, sDesc.ListenAddr,
					sDesc.TOS );

			if ( sDesc.maxPDUSize != 0 )
			{
				// be carefull because of may be exhausted memory
				try
				{
					mReceiver = new CSDPReceiver ( mSocket, sDesc.maxPDUSize, sDesc.sample_n );
				}
				catch ( const std::bad_alloc )
				{
					throw CNetwork_exception ( ( string ) "Memory exhausted" +
							"at the server side! Could" +
							"not instantiate" +
							"the TCP receiver", 11 );
				}
			}
			else
			{
				mReceiver = new CSDPReceiver ( mSocket );
			}
		}
#endif // SDP_ENABLED
		else
		{
			std::string eMsg("Wrong transport protocol used. Only TCP and UDP");

#ifdef	SDP_ENABLED

			eMsg += " and SDP";
#endif // SDP_ENABLED
			eMsg += " currently supported!";
			throw CNetwork_exception(eMsg);
		}

		// listen on the random port

		if (!mSocket->Listen() || mSocket->BindPort() == 0)
		{
			throw CNetwork_exception(
					"Starting listener on the Port "
							+ CStringUtils::itos(mSocket->BindPort())
							+ " failed!");
		}
		mServerPort = mSocket->BindPort();
		std::cout << "Server Listen Port is: " << mServerPort << std::endl;
		std::cerr << "\nSession " << this << " successfully started"
				<< std::endl;
	} catch (const CNetwork_exception & ex)
	{
		mFlowIF_ptr->decrement_session_counter();
		throw; ///TODO:

	}
}

/// dtor: delete socket, associated with that session and deactivate poa object
RSessionIf_impl::~RSessionIf_impl()
{
	std::cerr << "Session " << this << " destroyed at "
			<< HPTimer::HighPerTimer::Now() << " seconds\n" << " ( time: "
			<< HPTimer::HighPerTimer::Now().PrintTime(false, false) << " ) "
			<< std::endl;
#ifdef EXIT_ON_SESSION_FINISH
	mFlowIf_ptr->Exit();
#endif
}

/**
 return  received samples one by one, if no more samples avaiable,
 Sample.HeaderSet will be set to false. The content of Header is then
 undefined
 */
void RSessionIf_impl::get_samples(std::vector<struct sample_t> & sample_list_t)
{

	CRPDUHeader * Header_ptr;
	// handle freeze and finish mutexes and conditions
	pthread_mutex_lock(&mFreezedM);
	if (mFreezed == false)
	{
		pthread_cond_wait(&mFreezedC, &mFreezedM);

	}
	pthread_mutex_unlock(&mFreezedM);

	// fill the sample list
	uint32_t len(0);
	for (uint32_t i(0); i < SAMPLE_LIST_MAX; i++)
	{
		if ((Header_ptr = mReceiver->getNextHeader()) != 0)
		{
			len++;
			sample_t mysample =
			{ Header_ptr->Header[0], Header_ptr->Header[1],
					Header_ptr->Header[2], Header_ptr->Header[3],
					Header_ptr->Header[4], Header_ptr->Header[5],
					Header_ptr->Header[6], Header_ptr->Header[7],
					Header_ptr->Header[8], Header_ptr->Header[9],
					Header_ptr->Header[10], Header_ptr->Header[11] };
			sample_list_t.push_back(mysample);
		}
		else
		{
			break;
		}
	}

}

/**
 finish the session, socket will be closed, but session won't be destroyed
 now received data are available via call get_samples()
 */
void RSessionIf_impl::finish_session(uint32_t pLastSeqNo)
{
	mFinished = true;
	mLastSeqNo = pLastSeqNo;

}

int ltestService::finish_session(uint32_t pLastSeqNo,
		struct FinishSessionResponse &response)
{
	FlowIF_impl *flowif = (FlowIF_impl *) soap->user;
	flowif->GetRSessionPtr()->finish_session(pLastSeqNo);
	return SOAP_OK;
}

/// delete the session structure
void RSessionIf_impl::release_session()
{

	mFinished = true;
	// wait till the data receiver can be deleted, but maximum 0.5 seconds
	for (uint i(0); i < 50; i++)
	{
		if (mRecvFinished)
		{
			break;
		}
		else
		{
			// FIXME: why this 500 ms? ES
			usleep(100000);

		}
	}
	if (mReceiver != NULL)
	{
		delete mReceiver;
		std::cerr << "delete mReceiver!" << std::endl;
		mReceiver = NULL;
	}
	else
	{
	}
	//we must assure, the session is released before we decrement the counter and delete the socket!
	// this is not the case, if get_results has not been called at the client
	pthread_mutex_lock(&mFreezedM);
	if (mFreezed == false)
	{
		pthread_cond_wait(&mFreezedC, &mFreezedM);
	}
	pthread_mutex_unlock(&mFreezedM);

	int session_counter = mFlowIF_ptr->decrement_session_counter();
	std::cerr << session_counter
			<< " another sessions hosted at the time on the server"
			<< std::endl;
	if (mSocket)
	{
		std::cout << "\ndelete socket while destroying RSessionIf_impl"
				<< std::endl;
		delete (mSocket);
		mSocket = NULL;
	}

}

/// get the system time of the server via the CORBA channel
tv_type RSessionIf_impl::get_time_by_gsoap()
{
	timeval now;
	//gettimeofday(&now, NULL);
	HPTimer::HighPerTimer HPnow;
	HPnow = HPTimer::HighPerTimer::Now();
	HPnow.SetTV(now);

	tv_type tv_now =
	{ now.tv_sec, now.tv_usec };
	return tv_now;
}

/**
 start new UDP listener on a given port, or fetch a free port from the OS.
 Start listening in a new thread and answer all received data packets with
 a time stamp in the form <seconds><microseconds> in network byte order
 */
uint16_t RSessionIf_impl::start_UDP_time_port(uint16_t pPort)
{
	mUDPTimeFinished = false;
	mUDPTimeSocket_ptr = new CUDPSocket(true, pPort);

	// run the UDP timesync server on within a new thread
	pthread_create(&mUDPTimeThreadID, NULL, &UDP_time_thread, (void*) this);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

	// set read timer or the mUDPTimeSocket_ptr
	HPTimer::HighPerTimer timer(static_cast<int64_t>(0u),
			static_cast<int64_t>(500000000u));
	mUDPTimeSocket_ptr->SetRecvTimeout(timer);
	return mUDPTimeSocket_ptr->BindPort();
}

/** finish the listener - delete the corresponding listener
 */
void RSessionIf_impl::release_UDP_time_port()
{
	mUDPTimeFinished = true;
	// only when the time UDP port finishes, the socket should be deleted
	pthread_join(mUDPTimeThreadID, NULL);
	if (mUDPTimeSocket_ptr)
	{
		mUDPTimeSocket_ptr->Close();
		delete (mUDPTimeSocket_ptr);
		std::cerr << "UDP time sync socket destroyed" << std::endl;
		mUDPTimeSocket_ptr = NULL;
	}
	std::cerr << "UDP time port thread released" << std::endl;
}

/**
 the UDP time server running within the receiver session
 currently it doesn't use the received time - only test, if the
 data length is 8 bytes (for the format <seconds><microseconds>
 as uint32
 */
void RSessionIf_impl::run_UDP_time()
{
	char Buff[65528]; // maximal udp packet size
	unsigned int FrameLen = 12; // Length of the timestamp data packet
	unsigned int len;
	timeval now;
	unsigned char secStr[4];
	unsigned char usecStr[4];
	while (!mUDPTimeFinished)
	{

		// test for thread conacelactoin from the main thread
		// pthread_testcancel();
		len = mUDPTimeSocket_ptr->Recv(Buff, sizeof(Buff));

		// test, if a connection to the sender is established. If not, establish connection
		// backward to the sender
		CIPAddress remoteAddr;
		if (!mUDPTimeSocket_ptr->Connected())
		{
			remoteAddr = mUDPTimeSocket_ptr->RemoteAddr();
			mUDPTimeSocket_ptr->Connect(remoteAddr);
		}

		if (len == 12)
		{
			// prepare data packet with the content <seconds><microseconds>
			//gettimeofday(&now, NULL);
			HPTimer::HighPerTimer HPnow;
			HPnow = HPTimer::HighPerTimer::Now();
			HPnow.SetTV(now);

			*((uint32_t *) &secStr) = (uint32_t) htonl(now.tv_sec);
			*((uint32_t *) &usecStr) = (uint32_t) htonl(now.tv_usec);
			// copy the timestamp from the receiving to sending buffer
			memcpy(Buff + 4, secStr, 4);
			memcpy(Buff + 8, usecStr, 4);

#ifdef DEBUG_OUTPUT

			std::cerr << "Captured time is: " << now.tv_sec << "." << now.tv_usec << std::endl;
			std::cerr << " " << std::endl;
			std::cerr << "secStr: 0x" << std::hex;
			for ( uint i ( 0 ); i< sizeof ( secStr ); i++ )
			{
				std::cerr << ( int ) ( unsigned char ) secStr[i] << " ";
			}
			std::cerr << "\tusecStr: 0x";
			for ( uint i ( 0 ); i< sizeof ( usecStr ); i++ )
			{
				std::cerr << ( int ) ( unsigned char ) usecStr[i] << " ";
			}
			std::cerr << std::dec << std::endl;
#endif

			// send the data packet
			mUDPTimeSocket_ptr->Send(Buff, FrameLen);
		}
	}
	return;
}

/**
 the routine that accept the connection, receives the packets and stores
 the samples
 */
void RSessionIf_impl::Run()
{
	// constexpr CTSCTimer RCV_SOCK_TIMEOUT ( static_cast<int32_t>(0u), static_cast<int32_t>(500000u), false ); // 0,5 s
	// set accept timeout to 0.5 s
	HPTimer::HighPerTimer timer1(static_cast<int64_t>(0u),
			static_cast<int64_t>(100000000u));
	mSocket->SetRecvTimeout(timer1);

	if (0 != mSocket)
	{
		while (!mSocket->Accept())
		{

		}
	}

	// FIXME: Please exchange by constexpr values within the header! ES
	// set receive timeout to 0.5 s
	HPTimer::HighPerTimer timer2(static_cast<int64_t>(0u),
			static_cast<int64_t>(100000000u));
	mSocket->SetRecvTimeout(timer2);

	int bytes(0);
	// the incoming connection can only be reported after first data packet received
	// for correct fetching remote address
	int i(0);
	for (; i < 100; i++)
	{
		if ((bytes = mReceiver->Recv()) < 0)
		{
			std::cout << bytes << std::endl;
			std::cerr << "error in data reception" << std::endl;
			ballout();
			return;
		}
		if (mFinished)
		{
			ballout();
			return;
		}
		else
		{
		}
		if (bytes > 0)
		{
			break;
		}
	}
	std::cout << "Data connection from peer " << mSocket->RemoteAddr()
			<< std::endl;

	while ((bytes = mReceiver->Recv()) >= 0)
	{
		if (mFinished)
		{
			ballout();
			break;
		}
		else
		{
		}

	}

	std::cout << std::endl;
	std::cout << "end of receiving data\n" << std::endl;
}

// set finished flag and freeze the data source
void RSessionIf_impl::ballout()
{
	mFinished = true;
	// set the timeout timer to 2 secs
	HPTimer::HighPerTimer TimeOut(static_cast<int64_t>(2u),
			static_cast<int64_t>(0u));
#ifdef DEBUG_OUTPUT

	std::cerr << "Receiver will be finished after 2 seconds timeout. " << __PRETTY_FUNCTION__ << std::endl;
#endif
	// freeze the mReceiver structures, play out back log
	mReceiver->Freeze(mLastSeqNo, TimeOut);
	mRecvFinished = true;
	mSocket->Close();
	pthread_mutex_lock(&mFreezedM);
	mFreezed = true;
	pthread_cond_broadcast(&mFreezedC);
	pthread_mutex_unlock(&mFreezedM);
}

int ltestService::print_sDesc(struct sourceDesc_t sDec, std::string &str)
{
	FlowIF_impl *flowif = (FlowIF_impl *) soap->user;
	str = flowif->print_sDesc(sDec);
	return SOAP_OK;

}

int ltestService::start_ssession(struct sourceDesc_t sDec,
		struct senderDesc_t senderDesc, struct StartSSessionResponse &response)
{
	FlowIF_impl *flowif = (FlowIF_impl *) soap->user;
	try
	{
		flowif->start_ssession(sDec, senderDesc);
	} catch (CNetwork_exception & ex)
	{
		std::string strerr = "server is blocked. Block finish time is: "
				+ CStringUtils::itos(sDec.StopTime.tv_sec) + "."
				+ CStringUtils::itos(sDec.StopTime.tv_usec);
		std::string strdetail = CStringUtils::itos(ex.code())
				+ "sessions already hosted on the server host";
		soap_senderfault(strerr.c_str(), strdetail.c_str());
	}
	return SOAP_OK;

}

int ltestService::start_rsession(struct sourceDesc_t sDec,
		struct StartRSessionResponse &response)
{
	FlowIF_impl *flowif = (FlowIF_impl *) soap->user;
	try
	{
		flowif->start_rsession(sDec);
	} catch (CNetwork_exception & ex)
	{
		std::string strerr = "server is blocked. Block finish time is: "
				+ CStringUtils::itos(sDec.StopTime.tv_sec) + "."
				+ CStringUtils::itos(sDec.StopTime.tv_usec);
		std::string strdetail = CStringUtils::itos(ex.code())
				+ "sessions already hosted on the server host";
		soap_senderfault(strerr.c_str(), strdetail.c_str());
	}
	return SOAP_OK;
}

int ltestService::ping(struct PingResponse &response)
{
	FlowIF_impl *flowif = (FlowIF_impl *) soap->user;
	flowif->ping();
	return SOAP_OK;
}

int ltestService::Port(uint16_t &pPort)
{
	FlowIF_impl *flowif = (FlowIF_impl *) soap->user;
	pPort = flowif->GetRSessionPtr()->Port();
	return SOAP_OK;
}

int ltestService::release_ssession(struct ReleaseSSessionResponse &response)
{
	FlowIF_impl *flowif = (FlowIF_impl *) soap->user;
	flowif->GetSSessionPtr()->release_session();
	return SOAP_OK;

}

int ltestService::release_rsession(struct ReleaseRSessionResponse &response)
{
	FlowIF_impl *flowif = (FlowIF_impl *) soap->user;
	flowif->GetRSessionPtr()->release_session();
	return SOAP_OK;

}

int ltestService::get_samples_whole(
		std::vector<struct whole_sample_t> & GetSampleList)
{
	FlowIF_impl *flowif = (FlowIF_impl *) soap->user;
	flowif->GetSSessionPtr()->get_samples(GetSampleList);
	return SOAP_OK;
}

int ltestService::getBasicSourceStats(
		struct BasicSourceStats_t & GetBasicSourceStatics)
{
	FlowIF_impl *flowif = (FlowIF_impl *) soap->user;
	GetBasicSourceStatics = flowif->GetSSessionPtr()->getBasicSourceStats();
	return SOAP_OK;

}

int ltestService::get_samples(std::vector<struct sample_t> &sample_list_t)
{

	FlowIF_impl *flowif = (FlowIF_impl *) soap->user;
	flowif->GetRSessionPtr()->get_samples(sample_list_t);
	return SOAP_OK;

}

int ltestService::get_time_by_gsoap(struct tv_type &ptv)
{
	FlowIF_impl *flowif = (FlowIF_impl *) soap->user;
	ptv = flowif->GetRSessionPtr()->get_time_by_gsoap();
	return SOAP_OK;

}

int ltestService::start_UDP_time_port(uint16_t pPort, uint16_t &OPort)
{
	FlowIF_impl *flowif = (FlowIF_impl *) soap->user;
	OPort = flowif->GetRSessionPtr()->start_UDP_time_port(pPort);
	return SOAP_OK;
}

int ltestService::release_UDP_time_port(
		struct ReleaseUDPTimePortResponse &response)
{
	FlowIF_impl *flowif = (FlowIF_impl *) soap->user;
	flowif->GetRSessionPtr()->release_UDP_time_port();
	return SOAP_OK;
}

