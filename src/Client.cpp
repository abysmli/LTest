#include "Client.h"

ltestProxy * GsoapClient::mProxy = NULL;
GsoapClient::GsoapClient(ltestProxy &proxy)
{
	mProxy = &proxy;
}

GsoapClient::~GsoapClient()
{
	//if(mDataSource!=NULL)
	//  delete mDataSource;
}

void GsoapClient::Run()
{

	ConfLtestStruct * Conf = CCommandLine::conf();

	// initialize the sender description structure, used by the receiver
	sourceDesc_t sDesc;
	sDesc.StartTime.tv_sec = Conf->StartTime.Seconds();
	sDesc.StartTime.tv_usec = Conf->StartTime.USeconds();
	sDesc.StopTime.tv_sec = Conf->FinishTime.Seconds();
	sDesc.StopTime.tv_usec = Conf->FinishTime.USeconds();
	sDesc.ListenAddr = Conf->ReceiverDataAddress.IPAddress();
	sDesc.ListenPort = Conf->ReceiverDataAddress.Port();
	sDesc.Proto = Conf->Proto;
	sDesc.Prio = Conf->Priority;                // priority of the thread
	sDesc.TOS = Conf->TOS;                      // TOS field value
	// socket options, to be passed to be passed to the sender and receiver sockets
	SockOptsMap::iterator it(Conf->SocketOptions.begin());
	for (; it != Conf->SocketOptions.end(); it++)
	{
		sock_opt_t mysock =
		{ it->first, it->second };
		sDesc.SockOpts.push_back(mysock);
	}
	// extimate the number of samples to be transmitted
	// initialize the vector with sent pdu headers
	if (Conf->Packets != 0)
	{
		sDesc.sample_n = Conf->Packets;
	}
	else if (Conf->Bytes != 0)
	{
		sDesc.sample_n = Conf->Bytes;
	}
	else
	{
		HPTimer::HighPerTimer duration;
		if (Conf->Duration != HPTimer::HighPerTimer::Nil())
		{
			duration = Conf->Duration;
		}
		else
		{
			duration = Conf->FinishTime - Conf->StartTime;
		}
		if (Conf->BPS != 0)
		{
			long long InterPacketTics = static_cast<long long>(Conf->Size) * 8
					* 1000000 * HPTimer::HighPerTimer::GetHPFrequency()
					/ Conf->BPS;
			unsigned long long packets = duration.HPTics() / InterPacketTics;
			// limit by 5 million samples
			packets = std::min(packets, (unsigned long long) 5000000);
			sDesc.sample_n = packets;
		}
	}
	/// if Block is set, receiver tries to block,
	/// if other sessions allready runs, start session throws an SessionBlock
	/// exception with the Finish Time of the session that will finish last
	sDesc.Block = Conf->Blocking;
	/// signal the big PDU sizes to the receiver side
	if (Conf->SType == ST_FS)
	{
		sDesc.maxPDUSize = TCP_PERFORMANCE_SCALE * MAX_IP_SIZE;
	}
	else
	{
		sDesc.maxPDUSize = MAX_IP_SIZE;
	}

	if (Conf->RemoteSender)
	{
		senderDesc_t senderDesc;
		senderDesc.LocalAddr = Conf->LocalDataAddress.IPAddress();
		senderDesc.LocalPort = Conf->LocalDataAddress.Port();
		senderDesc.RemoteAddr = Conf->ReceiverDataAddress.IPAddress();
		senderDesc.RemotePort = Conf->ReceiverDataAddress.Port();
		senderDesc.RemoteURL = Conf->ReceiverGsoapAddress.GsoapURL();
		senderDesc.Bytes = Conf->Bytes;
		senderDesc.Packets = Conf->Packets;
		senderDesc.Proto = Conf->Proto;
		senderDesc.StartTime.tv_sec = Conf->StartTime.Seconds();
		senderDesc.StartTime.tv_usec = Conf->StartTime.USeconds();
		senderDesc.Duration.tv_sec = Conf->Duration.Seconds();
		senderDesc.Duration.tv_usec = Conf->Duration.USeconds();
		senderDesc.FinishTime.tv_sec = Conf->FinishTime.Seconds();
		senderDesc.FinishTime.tv_usec = Conf->FinishTime.USeconds();
		senderDesc.Priority = Conf->Priority;
		senderDesc.SType = Conf->SType;
		senderDesc.BPS = Conf->BPS;
		senderDesc.Size = Conf->Size;
		senderDesc.FifoName = Conf->FifoFile;
		senderDesc.TimeCorrection = Conf->TimeCorrection;
		senderDesc.MaxTimeDev = Conf->MaxTimeDev;
		senderDesc.TOS = Conf->TOS;
		/// this routine blocks until the remote sender is finished, so after return from
		/// call we can immediately fetch the results
		mDataSource = new CDummySource();
		std::cout << "New source started. Source description:\n"
				<< mDataSource->PrintSDesc() << "..." << std::endl;
		StartSSessionResponse StartSSessRes;
		ReleaseSSessionResponse ReleaseSSessRes;
		try
		{
			if (SOAP_OK
					!= mProxy->start_ssession(sDesc, senderDesc, StartSSessRes))
				mProxy->soap_stream_fault(std::cerr);
		} catch (const std::bad_alloc & ex)
		{
			std::cerr
					<< "Out of memory error! The sender session will be released."
					<< std::endl;
			mProxy->release_ssession(ReleaseSSessRes);
			throw(ex);
		}

		catch (const std::exception & ex)
		{
			std::cerr << "System exception caught! Reason is " << ex.what()
					<< "  The sender session will be released." << std::endl;
			throw;
		}

		catch (...)
		{
			std::cerr
					<< "Unknown exception caught! The sender session will be released."
					<< std::endl;
			throw;
		}
	}
	else
	{
		// the difference of the client and server times. Used for time correction on sender results
		bool TimeCorrectionPerformed(false);
		uint16_t remoteDataPort = 0;
		StartRSessionResponse StartRSessRes;
		if (SOAP_OK != mProxy->start_rsession(sDesc, StartRSessRes))
			mProxy->soap_stream_fault(std::cerr);
		if (SOAP_OK != mProxy->Port(remoteDataPort))
			mProxy->soap_stream_fault(std::cerr);
		std::cerr << "remoteDataPort: " << remoteDataPort << std::endl;
		/// The calculated time difference between sender and receiver and the position of delta time | changed by li, yuan
		HPTimer::HighPerTimer mDeltaTime, mDeltaTime_pos;
		/// The calculated time difference between sender and receiver at the end of measurement and the position of delta time | added by li, yuan
		HPTimer::HighPerTimer mDeltaTime_end, mDeltaTime_end_pos;
		/// To calculate time difference between each part of measurement
		HPTimer::HighPerTimer time1, time2, time3, time4;
		// if time correction configured, perform time correction after
		// session initialization
		// time stamp 1 | added by Li, Yuan
		time1.SetNow();
		if (Conf->TimeCorrection)
		{
			std::cout
					<< "\n\nLaunch the Time Correction at the begin of Measurement\n";
			std::cout
					<< "------------------------------------------------------------------------\n\n";
			CTimeCorrection * timeCorr = NULL;

			try
			{
				CIPAddress rUDPTimeAddr(Conf->ReceiverDataAddress.IPAddress(),
						0);
				std::cerr << "Start UDP time correction: " << std::endl;
				timeCorr = new CUDPTimeCorrection(mProxy,
						Conf->ReceiverDataAddress, Conf->MaxTimeDev);
				if ((TimeCorrectionPerformed = timeCorr->PerformCorrection())
						== true)
				{
					mDeltaTime = timeCorr->CalcDelta();
					std::cout
							<< "Time correction via UDP performed. Correction result is:"
							<< mDeltaTime << " s\n";
					timeCorr->mCorrStats.print_out(std::cout);
					mDeltaTime_pos = timeCorr->mCorrStats.ICADeltaSendTime;
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
						<< __PRETTY_FUNCTION__ << ": " << ex.what() << " Code: "
						<< ex.code() << std::endl;
				if (timeCorr != NULL)
				{
					delete timeCorr;
					timeCorr = NULL;
				}
			} catch (const CNetwork_exception & ex)
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
			// if correction by UDP failed, correction via the CORBA channel
			if (!TimeCorrectionPerformed)
			{
				try
				{
					timeCorr = new GSOAPTimeCorrection(mProxy,
							Conf->MaxTimeDev);
					if ((TimeCorrectionPerformed = timeCorr->PerformCorrection())
							== true)
					{
						mDeltaTime = timeCorr->CalcDelta();
						std::cout
								<< "Time correction via GSOAP performed. Correction result is:"
								<< mDeltaTime << " s \n";
						timeCorr->mCorrStats.print_out(std::cout);
						mDeltaTime_pos = timeCorr->mCorrStats.ICADeltaSendTime;
						std::cout << std::endl;
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
			std::cout
					<< "------------------------------------------------------------------------\n\n";
		} // --- end if Conf->TimeCorrection
		  // time stamp 2 | added by Li, Yuan
		time2.SetNow();
		// create new data source
		if (Conf->SType == ST_FIFO)
		{

			std::cerr << "Create FIFO source " << "\n" << std::endl;
			mDataSource = new CFIFOSource(Conf->LocalDataAddress.IPAddress(),
					Conf->LocalDataAddress.Port(),
					Conf->ReceiverDataAddress.IPAddress(), remoteDataPort,
					Conf->Bytes, Conf->Packets, Conf->Proto, Conf->StartTime,
					Conf->Duration, Conf->FinishTime, Conf->Priority,
					Conf->FifoFile, Conf->TOS, Conf->SocketOptions);

		}
		else if (Conf->SType == ST_CBR)
		{
			std::cerr << "Create CBR source " << "\n" << std::endl;

			mDataSource = new CCBRSource(Conf->LocalDataAddress.IPAddress(),
					Conf->LocalDataAddress.Port(),
					Conf->ReceiverDataAddress.IPAddress(), remoteDataPort,
					Conf->Bytes, Conf->Packets, Conf->Proto, Conf->BPS,
					Conf->Size, Conf->StartTime, Conf->Duration,
					Conf->FinishTime, Conf->Priority, Conf->TOS,
					Conf->SocketOptions);

		}
		// full speed source
		else if (Conf->SType == ST_FS)
		{
			std::cerr << "Create Full Speed source " << "\n" << std::endl;

			mDataSource = new CFullSpeedSource(
					Conf->LocalDataAddress.IPAddress(),
					Conf->LocalDataAddress.Port(),
					Conf->ReceiverDataAddress.IPAddress(), remoteDataPort,
					Conf->Bytes, Conf->Packets, Conf->Proto, Conf->Size,
					Conf->StartTime, Conf->Duration, Conf->FinishTime,
					Conf->Priority, Conf->TOS, Conf->SocketOptions);

		}

		else if (Conf->SType == ST_POISSON)
		{
			std::cerr << "Create Poisson source " << "\n" << std::endl;

			mDataSource = new CPoissonSource(Conf->LocalDataAddress.IPAddress(),
					Conf->LocalDataAddress.Port(),
					Conf->ReceiverDataAddress.IPAddress(), remoteDataPort,
					Conf->Bytes, Conf->Packets, Conf->Proto, Conf->BPS,
					Conf->Size, Conf->StartTime, Conf->Duration,
					Conf->FinishTime, Conf->Priority, Conf->TOS,
					Conf->SocketOptions);
		}
		else
		{
			std::cerr << "error no vaild source selceted, exit now"
					<< std::endl;
			exit(3);
		}

		// set the time correction of the DataSource
		if (TimeCorrectionPerformed)
		{
			mDataSource->SetDeltaTime(mDeltaTime, mDeltaTime_pos);
		}

		std::cout << "New source started. Source description:\n"
				<< mDataSource->PrintSDesc() << "..." << std::endl;
		ReleaseRSessionResponse ReleaRSessRes;
		try
		{
			// dispatch the source until finished
			while (!mDataSource->Finished())
			{
				mDataSource->Dispatch();
			}
#ifdef DEBUG_OUTPUT
			std::cerr << "Data Source finished ..." << std::endl;
#endif
		} catch (const std::bad_alloc & ex)
		{
			std::cerr
					<< "Out of memory error! The sender session will be released."
					<< std::endl;
			if (SOAP_OK != mProxy->release_rsession(ReleaRSessRes))
				mProxy->soap_stream_fault(std::cerr);
			throw(ex);

		} catch (...)
		{
			std::cerr
					<< "Unknown exception caught! The sender session will be released."
					<< std::endl;
			if (SOAP_OK != mProxy->release_rsession(ReleaRSessRes))
				mProxy->soap_stream_fault(std::cerr);
			throw;
		}

		// time stamp 3 | added by Li, Yuan
		time3.SetNow();
		//------------------------------------added by Li, Yuan----------------------------------
		if (Conf->ICA && Conf->TimeCorrection)
		{
			std::cout
					<< "\n\nLaunch the Time Correction at the end of Measurement\n";
			std::cout
					<< "------------------------------------------------------------------------\n\n";
			CTimeCorrection * timeCorr = NULL;

			try
			{
				CIPAddress rUDPTimeAddr(Conf->ReceiverDataAddress.IPAddress(),
						0);
				std::cerr << "Start UDP time correction: " << std::endl;
				timeCorr = new CUDPTimeCorrection(mProxy,
						Conf->ReceiverDataAddress, Conf->MaxTimeDev);
				if ((TimeCorrectionPerformed = timeCorr->PerformCorrection())
						== true)
				{
					mDeltaTime_end = timeCorr->CalcDelta();
					std::cout
							<< "Time correction via UDP performed. Correction result is:"
							<< mDeltaTime_end << " s\n";
					timeCorr->mCorrStats.print_out(std::cout);
					mDeltaTime_end_pos = timeCorr->mCorrStats.ICADeltaSendTime;
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
						<< __PRETTY_FUNCTION__ << ": " << ex.what() << " Code: "
						<< ex.code() << std::endl;
				if (timeCorr != NULL)
				{
					delete timeCorr;
					timeCorr = NULL;
				}
			} catch (const CNetwork_exception & ex)
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
			// if correction by UDP failed, correction via the CORBA channel
			if (!TimeCorrectionPerformed)
			{
				try
				{
					timeCorr = new GSOAPTimeCorrection(mProxy,
							Conf->MaxTimeDev);
					if ((TimeCorrectionPerformed = timeCorr->PerformCorrection())
							== true)
					{
						mDeltaTime_end = timeCorr->CalcDelta();
						std::cout
								<< "Time correction via GSOAP performed. Correction result is:"
								<< mDeltaTime_end << " s \n";
						timeCorr->mCorrStats.print_out(std::cout);
						mDeltaTime_end_pos =
								timeCorr->mCorrStats.ICADeltaSendTime;
						std::cout << std::endl;
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
			std::cout
					<< "------------------------------------------------------------------------\n\n";

			// set the time correction, which at the end of measurement of the DataSource
			if (TimeCorrectionPerformed)
			{
				mDataSource->SetDeltaTime_end(mDeltaTime_end,
						mDeltaTime_end_pos);
			}
		} // --- end if Conf->ICA
		time4.SetNow();
		std::cout << "\n\nTime difference between each part of measurement"
				<< "\n1. ICA      : " << time2 - time1 << "\nMeasurement : "
				<< time3 - time2 << "\n2. ICA      : " << time4 - time3 << "\n\n\n";

	}
}

void GsoapClient::FetchResults()
{
	ConfLtestStruct * Conf = CCommandLine::conf();

// remote sender: fetch samples from the remote side in pieces of SAMPLE_LIST_MAX
	if (Conf->RemoteSender)
	{
		std::vector<whole_sample_t> SampleList;
		std::cerr
				<< "Session finished. Start receiving sample headers from the remote sender"
				<< std::endl;
		CDummySource *Source = dynamic_cast<CDummySource *>(mDataSource);
		for (int k(0); true; k++)
		{
			if ((mProxy->get_samples_whole(SampleList)) != SOAP_OK)
				mProxy->soap_stream_fault(std::cerr);
			for (uint32_t i(0); i < SampleList.size(); i++)
			{
				Source->StoreSample(SampleList.at(i).Header);
			}
			// if list don't have full length, then no more samples available in the receiver
			// just finish the receiving loop
			if (SampleList.size() < 1000)
				break;
			std::cerr << "." << std::flush;
			if ((k % 50) == 0)
				std::cerr << std::endl;

		}
		BasicSourceStats_t BasicStats;
		if (SOAP_OK != mProxy->getBasicSourceStats(BasicStats))
			mProxy->soap_stream_fault(std::cerr);
		// store basic source statistics in the dummy source variables
		Source->ImportBasicStats(BasicStats);

		ReleaseSSessionResponse ReleaSSessRes;
		if (SOAP_OK != mProxy->release_ssession(ReleaSSessRes))
			mProxy->soap_stream_fault(std::cerr);
	}
	else  // local sender
	{
		mDataSource->CloseSocket();
		FinishSessionResponse FinishSessRes;
		if (SOAP_OK
				!= mProxy->finish_session(mDataSource->LastSeqNo(),
						FinishSessRes))
			mProxy->soap_stream_fault(std::cerr);
		std::cerr << "Session finished. Start receivinig sample headers from"
				<< " the data receiver..." << std::endl;
		// fetch samples from the receiver in peaces of SAMPLE_LIST_MAX
		std::vector<sample_t> SampleList;
		for (int k(0); true; k++)
		{

			if ((mProxy->get_samples(SampleList)) != SOAP_OK)
				mProxy->soap_stream_fault(std::cerr);

			for (uint32_t i(0); i < SampleList.size(); i++)
			{
				mDataSource->StoreRSample(SampleList.at(i).Header);
			}
			// if list don't have full length, then no more samples available in the receiver
			// just finish the receiving loop
			if (SampleList.size() < 1000)
				break;
			std::cerr << "." << std::flush;
			if ((k % 50) == 0)
				std::cerr << std::endl;
		}

		ReleaseRSessionResponse ReleaRSessRes;
		if (SOAP_OK != mProxy->release_rsession(ReleaRSessRes))
			mProxy->soap_stream_fault(std::cerr);
	}

// save and print results
	std::cout << "\n" << mDataSource->PrintStats() << std::endl;
	mDataSource->PrintBasicStats();

// dumpt results
	if (Conf->DumpSamples)
	{
		std::ostream * DStream(0);
		if (Conf->DumpFile.length() != 0)
		{
			DStream = new std::ofstream(Conf->DumpFile.c_str());
		}
		else
		{
			DStream = &std::cout;
		}
		mDataSource->DumpSamples(DStream);
		*DStream << std::flush;
	}

}

void GsoapClient::deal_signal(int dummy)
{
	ConfLtestStruct * Conf = CCommandLine::conf();
	std::cout << "caught signal!" << std::endl;
	if (Conf->RemoteSender)
	{
		ReleaseSSessionResponse ReleaSSessRes;
		if (SOAP_OK != mProxy->release_ssession(ReleaSSessRes))
			mProxy->soap_stream_fault(std::cerr);
	}
	else
	{
		ReleaseRSessionResponse ReleaRSessRes;
		if (SOAP_OK != mProxy->release_rsession(ReleaRSessRes))
			mProxy->soap_stream_fault(std::cerr);

	}
	exit(0);
}

void GsoapClient::LtestHandler()
{
	signal(SIGINT, GsoapClient::deal_signal);
	signal(SIGTERM, GsoapClient::deal_signal);
	signal(SIGHUP, GsoapClient::deal_signal);

}

/// for web mode: get data source ptr
CDataSource* GsoapClient::GetDataSource()
{
	return mDataSource;
}

