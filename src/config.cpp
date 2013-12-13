#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include "strtok.h"

#ifdef linux
#include <getopt.h>
#endif

#include <unistd.h>
#include "config.h"

CCommandLine * CCommandLine::mCommandLine = 0;
ConfLtestStruct * CCommandLine::mConf = 0;

/// ctor
CCommandLine::CCommandLine() :
		mCLineParsed(false)
{
	OptionNameMap['B'] = std::string("blockingMode");
	OptionNameMap['C'] = std::string("gsoapAddress");
	OptionNameMap['D'] = std::string("dumpSamples");
	OptionNameMap['F'] = std::string("fifoFile");
	OptionNameMap['L'] = std::string("clientGsoapAddress");
	OptionNameMap['O'] = std::string("poisson");
	OptionNameMap['P'] = std::string("priority");
	OptionNameMap['R'] = std::string("remoteSender");
	OptionNameMap['S'] = std::string("server");
	OptionNameMap['T'] = std::string("TOS");
	OptionNameMap['Y'] = std::string("timeCorrection"); // activate client-server time correction
	OptionNameMap['I'] = std::string("ICA");
	OptionNameMap['Z'] = std::string("packetSize");
	OptionNameMap['b'] = std::string("bytes");
	OptionNameMap['d'] = std::string("duration");
	OptionNameMap['f'] = std::string("finishTime");
	OptionNameMap['h'] = std::string("help");
	OptionNameMap['l'] = std::string("localDataAddress");
	OptionNameMap['p'] = std::string("protocol");
	OptionNameMap['r'] = std::string("rate");
	OptionNameMap['s'] = std::string("startTime");
	OptionNameMap['t'] = std::string("target"); // only for gui or conf file based control
	OptionNameMap['z'] = std::string("packets"); // finish when number of packets sent
	OptionNameMap['W'] = std::string("webMode"); // for WEB modus
	mConf = new ConfLtestStruct;
}

/// dtor
CCommandLine::~CCommandLine()
{
	delete mConf;
	mConf = 0;
}

/// initialize the CCommandLine object
void CCommandLine::instance(int cargc, char *cargv[])
{

	assert(mCommandLine == 0);
	mCommandLine = new CCommandLine;
	assert(mCommandLine != 0);
	mCommandLine->parseCommandLine(cargc, cargv);
}

/// accessor the CCommandObject
CCommandLine *
CCommandLine::instance()
{
	assert(mCommandLine != 0);
	return mCommandLine;
}

extern "C" char *optarg;
extern "C" int optind;

/// parse the command line
void CCommandLine::parseCommandLine(int argc, char * argv[])
{

	// minimum arguments check
	if (argc < 2)
	{
		printUsage(argv[0]);
		exit(1);
	}
	int Opt;
	TableIter OptIter;

	// get all command line parameters
	while ((Opt = getopt(argc, (char **) argv,
			"BC:D:F:L:OP:R:ST:Y::I::Z:b:d:f:hl:p:r:s:z:W?")) != EOF)
	{
		if (Opt == '?' || Opt == 'h')
		{
			printUsage(argv[0]);
			exit(0);
		}
		if (optarg)
		{
			mCmdOptTable[OptionNameMap[Opt]] = (std::string) optarg;
		}
		else
		{
			mCmdOptTable[OptionNameMap[Opt]] = "";
		}
	}

	// initialize the command opt table iterator
	OptIter = mCmdOptTable.begin();

	// test web mode firstly
	if (mCmdOptTable.count("webMode"))
	{
		int ParamCounter(5);
		std::string gsoapAddress = "0.0.0.0:5555";
		if (mCmdOptTable.count("gsoapAddress"))
		{
			ParamCounter += 2;
			gsoapAddress = mCmdOptTable["gsoapAddress"];
		}

		GsoapAddress addr(gsoapAddress);
		mConf->GsoapLocalAddress = addr;

		// more arguments, then allowed in server mode ?

		if (argc > ParamCounter)
		{
			std::cerr << "found unallowed parameters in WebGUI mode"
					<< std::endl;
			printUsage(argv[0]);
			exit(1);
		}
		mConf->Server = true;
		mConf->WebMode = true;
	}

	// test server mode
	else if (mCmdOptTable.count("server"))
	{
		int ParamCounter(4);
		mConf->Server = true;
		std::string gsoapAddress = "0.0.0.0:5555";
		if (mCmdOptTable.count("gsoapAddress"))
		{
			ParamCounter += 2;
			gsoapAddress = mCmdOptTable["gsoapAddress"];
		}

		GsoapAddress addr(gsoapAddress);
		mConf->GsoapLocalAddress = addr;

		// more arguments, then allowed in server mode ?

		if (argc > ParamCounter)
		{
			std::cerr << "found unallowed parameters in server mode"
					<< std::endl;
			printUsage(argv[0]);
			exit(1);
		}
	}

	// test client mode
	else
	{
		// set blocking mode flag
		if (mCmdOptTable.count("blockingMode"))
		{
			mConf->Blocking = true;
		}

		// set ICA
		if (mCmdOptTable.count("ICA"))
		{
			mConf->ICA = true;
			if (mCmdOptTable["ICA"].length() != 0)
			{

				mConf->MaxTimeDev = 1000*CStringUtils::stoi(
						mCmdOptTable["ICA"]);
			}
		}

		// set time correction
		if (mCmdOptTable.count("timeCorrection"))
		{
			mConf->TimeCorrection = true;
			if (mCmdOptTable["timeCorrection"].length() != 0)
			{

				mConf->MaxTimeDev = 1000*CStringUtils::stoi(
						mCmdOptTable["timeCorrection"]);
			}
		}

		// set source type and if applicable fifo file name
		if (mCmdOptTable.count("fifoFile"))
		{
			mConf->SType = ST_FIFO;
			if (mCmdOptTable["fifoFile"].length() != 0)
			{
				mConf->FifoFile = mCmdOptTable["fifoFile"];
				// reset Bytes, if not set explicitly
				if (mCmdOptTable["bytes"].length() == 0)
				{
					mConf->Bytes = 0;
				}
			}

			// reset Packets, if not set explicitly
			if (mCmdOptTable["packets"].length() == 0)
			{
				mConf->Packets = 0;
			}
		}

		// test if poisson source is applicable
		else if (mCmdOptTable.count("poisson"))
		{
			mConf->SType = ST_POISSON;
		}

		// set local data address
		if (mCmdOptTable.count("localDataAddress") != 0)
		{

			CIPAddress LocalAddr(mCmdOptTable["localDataAddress"]);
			mConf->LocalDataAddress = LocalAddr;
		}
		std::string destination;
		if ((argc - optind) != 1)
		{
			// destination data host missed
			if (mCmdOptTable.count("target") == 0)
			{
				std::cerr << "destination host missed" << std::endl;
				printUsage(argv[0]);
				exit(1);
			}
			else
			{
				destination = mCmdOptTable["target"];
			}

		}
		else
		{
			destination = argv[optind];
		}

		CStringTokenizer tok(destination, ":", false, false);
		if (!tok.hasMoreTokens())
		{
			std::cerr << "destination host format error" << std::endl;
			printUsage(argv[0]);
			exit(1);
		}

		// set destination data address
		std::string ReceiverDataHost = tok.nextToken();
		mConf->ReceiverDataAddress.Hostname(ReceiverDataHost);
		unsigned short Port(0);
		if (tok.hasMoreTokens())
		{
			Port = CStringUtils::stoi(tok.nextToken());
		}
		mConf->ReceiverDataAddress.Port(Port);
		// set receiver GSOAP address

		mConf->ReceiverGsoapAddress.Hostname(ReceiverDataHost);
		mConf->ReceiverGsoapAddress.Port(5555);
		mConf->GsoapRemoteAddress = mConf->ReceiverGsoapAddress;
		if (mCmdOptTable.count("remoteSender")
				&& (mCmdOptTable["remoteSender"]).length())
		{
			GsoapAddress RecvGsoapAddress(mCmdOptTable["remoteSender"]);
			mConf->ReceiverGsoapAddress = RecvGsoapAddress;
		}

		// set remote Sender flag
		if (mCmdOptTable.count("remoteSender"))
		{
			mConf->RemoteSender = true;
		}

		// set the Client Gsoap Address
		std::string clientGsoapAddress("0.0.0.0:5554");
		if (mCmdOptTable.count("clientGsoapAddress"))
		{
			clientGsoapAddress = mCmdOptTable["clientGsoapAddress"];
		}
		GsoapAddress Address(clientGsoapAddress);
		mConf->GsoapLocalAddress = Address;

		if (mCmdOptTable.count("gsoapAddress") && !(mConf->RemoteSender))
		{
			std::string gsoapAddressString(mCmdOptTable["gsoapAddress"]);
			GsoapAddress Address(gsoapAddressString);
			mConf->GsoapRemoteAddress = Address;
		}

		// set CorbaRemobteAddress
		if (mConf->RemoteSender)
		{
			if (!(mCmdOptTable.count("gsoapAddress")))
			{
				std::cerr << "-C parameter is mandatory in remote sender mode"
						<< std::endl;
				printUsage(argv[0]);
				exit(1);
			}
			GsoapAddress Address(mCmdOptTable["gsoapAddress"]);
			mConf->GsoapRemoteAddress = Address;
		}

		// set protocol
		if (mCmdOptTable.count("protocol"))
		{
			// now, <PROTO>,<option>[=<value>]... syntax is possible
			CStringTokenizer protoString(mCmdOptTable["protocol"], ",");
			std::string proto;

			if (protoString.countTokens() >= 1)
			{
				proto = protoString.nextToken();
			}
			else
			{
				throw CConfigException("protocol string not found");
			}
			while (protoString.hasMoreTokens())
			{
				std::string key;
				unsigned int value(0);
				CStringTokenizer tok(protoString.nextToken(), "=");

				if (tok.countTokens() < 1 || tok.countTokens() > 2)
				{
					throw CConfigException(
							"can't parse socket option " + tok.nextToken());
				}
				key = tok.nextToken();
				if (tok.hasMoreTokens())
				{
					value = CStringUtils::stoi(tok.nextToken());
				}
				mConf->SocketOptions[key] = value;
			}

			// compare case insensitiv
			proto = CStringUtils::toupper(proto);

			if (proto == "TCP")
			{
				mConf->Proto = P_TCP;
			}
			else if (proto == "UDP")
			{
				mConf->Proto = P_UDP;
			}
			else if (proto == "UDT")
			{
				mConf->Proto = P_UDT;
			}
			else if (proto == "FASTTCP")
			{
				mConf->Proto = P_FastTCP;
			}
			else if (proto == "HSTCP")
			{
				mConf->Proto = P_HSTCP;
			}
			// SDP support added
#ifdef SDP_ENABLED
			else if (proto == "SDP")
			{
				mConf->Proto = P_SDP;
			}
#endif //SDP_ENABLED
			else
			{
				std::cerr
						<< "unknown or unsupported transport protocol choosed: "
						<< mCmdOptTable["protocol"] << " only TCP and UDP";

#ifdef SDP_ENABLED
				std::cerr << " and SDP";
#endif // SDP_ENABLED
				std::cerr << "protocols supported" << std::endl;
				printUsage(argv[0]);
				exit(1);
			}
		}

		// set start time
		if (mCmdOptTable.count("startTime"))
		{
			unsigned int sec;
			unsigned int usec;
			parseTimeString(mCmdOptTable["startTime"], sec, usec);
			mConf->StartTime.SetTimer(sec, usec * 1000);
		}

		// set finish time
		if (mCmdOptTable.count("finishTime"))
		{
			unsigned int sec;
			unsigned int usec;
			parseTimeString(mCmdOptTable["finishTime"], sec, usec);
			mConf->FinishTime.SetTimer(sec, usec * 1000);
		}
		// set duration
		if (mCmdOptTable.count("duration"))
		{
			unsigned int sec;
			unsigned int usec;
			parseTimeString(mCmdOptTable["duration"], sec, usec);
			mConf->Duration.SetTimer(sec, usec * 1000);
		}

		// set bytes, to be transmitted
		if (mCmdOptTable.count("bytes"))
		{
			std::string sBytes=mCmdOptTable["bytes"];
			mConf->Bytes = static_cast<uint64_t>(UnitUtils::StringToDoubleWithUnit(sBytes,UnitUtils::checkUnit(sBytes)));
			std::cout<<"bytes: "<<mConf->Bytes<<std::endl;
		}

		// set packets, to be transmitted
		if (mCmdOptTable.count("packets"))
		{
			std::string sPackets=mCmdOptTable["packets"];
			mConf->Packets = static_cast<uint64_t>(UnitUtils::StringToDoubleWithUnit(sPackets,UnitUtils::checkUnit(sPackets)));
			std::cout<<"packet: "<<mConf->Packets<<std::endl;
		}

		// set data rate
		if (mCmdOptTable.count("rate"))
		{
			std::string sRate=mCmdOptTable["rate"];
			mConf->BPS = static_cast<uint64_t>(UnitUtils::StringToDoubleWithUnit(sRate,UnitUtils::checkUnit(sRate)));
			std::cout<<"rate: "<<mConf->BPS<<std::endl;
		}

		// set packet size
		if (mCmdOptTable.count("packetSize"))
		{
			std::string sPacketSize=mCmdOptTable["packetSize"];
			mConf->Size = static_cast<uint64_t>(UnitUtils::StringToDoubleWithUnit(sPacketSize,UnitUtils::checkUnit(sPacketSize)));
			std::cout<<"Size: "<<mConf->Size<<std::endl;
		}

		// set flag for dumping samples as well as the dump file name
		if (mCmdOptTable.count("dumpSamples"))
		{
			mConf->DumpSamples = true;
			if (mCmdOptTable["dumpSamples"].length())
			{
				mConf->DumpFile = mCmdOptTable["dumpSamples"];
			}
		}

		// set the scheduling priority of the receiving process
		if (mCmdOptTable.count("priority"))
		{
			mConf->Priority = CStringUtils::stoi(mCmdOptTable["priority"]);
		}

		// set IP precedence (or DSCP)
		if (mCmdOptTable.count("TOS"))
		{
			mConf->TOS = CStringUtils::stoi(mCmdOptTable["TOS"]);
		}

		// perform some parameter precedence check
		normalize();

	} // --- end ltest client mode

	mCLineParsed = true;
} //--- end of parseCommandLine

/// override redundant parameters, takes proper preferences of parameters
/// and switches
void CCommandLine::normalize()
{
	if (mConf->ICA)
	{
		mConf->TimeCorrection=true;
	}
	/// test start and stop time as well as the duration
	if ((!(mConf->FinishTime).IsNil())
			&& ((mConf->StartTime > mConf->FinishTime)
					|| (mConf->FinishTime < HPTimer::HighPerTimer::Now())))
	{
		(mConf->StartTime).SetNow();
	}

	/// duration overrides the finish time
	if (!(mConf->Duration).IsNil() && (!(mConf->StartTime).IsNil()))
	{
		mConf->FinishTime = mConf->StartTime + mConf->Duration;
	}

	/** the amount of bytes takes precedence over the duration and finish time
	 */
	if (mCmdOptTable.count("bytes") != 0)
	{
		mConf->Duration = HPTimer::HighPerTimer::Nil();
	}
	// if bytes have not been set, but duration has been set, reset bytes
	else if (!(mConf->Duration).IsNil())
	{
		mConf->Bytes = 0;
	}

	/** the amount of packets takes precedence over the duration and finish
	 time and bytes
	 */
	if (mCmdOptTable.count("packets") != 0)
	{
		mConf->Duration = HPTimer::HighPerTimer::Nil();
		mConf->Bytes = 0;
	}
	/** if data rate explicitely set to 0, the full performance source should
	 * be started
	 */
	if (mConf->BPS == 0)
	{
		mConf->SType = ST_FS;
	}
}

/// accessor to the Conf struct
ConfLtestStruct *
CCommandLine::conf()
{
	return mConf;
}

///for web mode: set conf struct obtained from web server
void CCommandLine::SetWebConf(ConfLtestStruct * pWebConf)
{
	mConf = pWebConf;
	return;
}

/**
 parse the time string with the format <sec>[.<usec>], sets seconds and microseconds parameters.
 Returns true if no parse errors have occured, false otherways
 */
bool CCommandLine::parseTimeString(const std::string pTimeStr,
		unsigned int & pSec, unsigned int & pUsec)
{
	CStringTokenizer tok(pTimeStr, ".", false, false);
	if (tok.countTokens() > 2)
	{
		std::cerr << "wrong time format. Should be <sec>[.<usec>]" << std::endl;
		return false;
	}
	pSec = CStringUtils::stoi(tok.nextToken());
	pUsec = 0;
	if (tok.hasMoreTokens())
		pUsec = CStringUtils::stoi(tok.nextToken());
	return true;
}

UNIT UnitUtils::checkUnit(std::string string)
{
	UNIT eUnit;
	if (string == "")
	{
		eUnit = None;
	}
	else if (string[string.length() - 1] == 105)
	{
		if (string[string.length() - 2] == 71)
		{
			eUnit = Gi;
		}
		else if (string[string.length() - 2] == 77)
		{
			eUnit = Mi;
		}
		else if (string[string.length() - 2] == 107)
		{
			eUnit = ki;
		}
	}
	else if (string[string.length() - 1] == 71)
	{
		eUnit = G;
	}
	else if (string[string.length() - 1] == 77)
	{
		eUnit = M;
	}
	else if (string[string.length() - 1] == 107)
	{
		eUnit = k;
	}
	else
	{
		eUnit = None;
	}
	return eUnit;
}

double UnitUtils::StringToDoubleWithUnit(std::string string, UNIT eUnit)
{
	double dValue = 0;
	switch (eUnit)
	{
	case None:
		dValue = CStringUtils::stof(string);
		break;
	case G:
		dValue = CStringUtils::stof(string.substr(0, string.length() - 1))
				* pow(10, 9);
		break;
	case M:
		dValue = CStringUtils::stof(string.substr(0, string.length() - 1))
				* pow(10, 6);
		break;
	case k:
		dValue = CStringUtils::stof(string.substr(0, string.length() - 1))
				* pow(10, 3);
		break;
	case Gi:
		dValue = CStringUtils::stof(string.substr(0, string.length() - 2))
				* pow(2, 30);
		break;
	case Mi:
		dValue = CStringUtils::stof(string.substr(0, string.length() - 2))
				* pow(2, 20);
		break;
	case ki:
		dValue = CStringUtils::stof(string.substr(0, string.length() - 2))
				* pow(2, 10);
		break;
	}
	return dValue;
}

/// prints ltest usage
void CCommandLine::printUsage(const char * basename)
{
	std::cout<<"\nLTest is a tool for measurement of network performance\n\n";
	std::cout<<"Usage:"<<std::endl;
	std::cout<<"    Server side:"<<std::endl;
	std::cout<<"    -S    Starts ltest server listener on <GsoapServerHost:GsoapPort>"<<std::endl;
	std::cout<<"          example:"<<std::endl;
	std::cout<<"          "<<std::endl;
	std::cout<<"          ltest -S [-C [<GsoapServerHost>]:<GsoapPort>]"<<std::endl;
	std::cout<<std::endl;
	std::cout<<"          ltest -C <GsoapServerHost>:[<GsoapPort>] -R [ReceiverGsoapHost:ReceiverGsoapPort] [-p <protocol>]"<<std::endl;
	std::cout<<"          [-s sec[.usec]] [-d sec[.usec]] [-f sec[.usec]] [-F <FifoFile> | -O] [-l [host]:port] [-Z size]"<<std::endl;
	std::cout<<"          [-b bytes ] [-z packets] [-r rate] [-B] [-P [<priority>]] [-T <TOS>][-Y[<MaxDeviation>]]"<<std::endl;
	std::cout<<"          [-D [FileName]] remoteHost[:remotePort]"<<std::endl;
	std::cout<<std::endl;
	std::cout<<"    Client side:"<<std::endl;
	std::cout<<"    remoteHost[:remotePort]    Starts the ltest client"<<std::endl;
	std::cout<<std::endl;
	std::cout<<"          example:"<<std::endl;
	std::cout<<std::endl;
	std::cout<<"          ltest remoteHost[:remotePort] [-L [<LocalGsoapHost>]:<GsoapPort>] [-C [<GsoapServerHost>]:<GsoapPort>]"<<std::endl;
	std::cout<<"          [-p <protocol>]] [-s sec[.usec]] [-d sec[.usec]] [-f sec[.usec]] [-F <FifoFile> | -O] [-l [host]:port]"<<std::endl;
	std::cout<<"          [-Z size] [-b bytes ] [-z packets] [-r rate] [-B] [-P [<priority>]] [-T <TOS>][-Y[<MaxDeviation>]]"<<std::endl;
	std::cout<<"          [-D [FileName]]"<<std::endl;
	std::cout<<std::endl;
	std::cout<<"    Available subcommands:"<<std::endl;
	std::cout<<"    (Address setting):"<<std::endl;
	std::cout<<"    -L    gives the Hostname or IP address and port number on that local GSOAP listener(applicable only on the"<<std::endl;
	std::cout<<"          client side) will start."<<std::endl;
	std::cout<<"    -C    gives the Hostname or IP address and port number on that GSOAP server listens listen for incoming"<<std::endl;
	std::cout<<"          connections. The GSOAP port on ltest server defaults to 5555."<<std::endl;
	std::cout<<"          (NOTE! This means on the GSOAP server the local GSOAP address, on the LTest client it means the the"<<std::endl;
	std::cout<<"          address, on that target LTest server listens for a connection. In remote sender mode the -C parameters"<<std::endl;
	std::cout<<"          gives the Gsoap address of the remote sender implementation, see section REMOTE SENDER MODE)."<<std::endl;
	std::cout<<"    -l    gives the address of the local Socket to be used for sending packets."<<std::endl;
	std::cout<<std::endl;
	std::cout<<"    (Session setting):"<<std::endl;
	std::cout<<"    -P    <priority>    gives the priority of the process, valid values are positive numbers between 0-99,"<<std::endl;
	std::cout<<"          works only if ltest is running as root user, defaults to 99."<<std::endl;
	std::cout<<"    -T    gives the TOS value for the data stream."<<std::endl;
	std::cout<<"    -p    gives the used transport protocol, defaults to UDP. Currently supported protocols are: "<<std::endl;
	std::cout<<"          TCP, UDP, FastTCP, HSTCP (HighSpeedTCP)."<<std::endl;
	std::cout<<"    -D    gives the path, where the dump of the measurement (line by line for each sent sample) should be written."<<std::endl;
	std::cout<<"    -B    sets receiver in Blocking mode - at most one data steam in a given time is allowed."<<std::endl;
	std::cout<<"    -Y    [microseconds]    perform time correction of clocks between the sender and receiver. The optional"<<std::endl;
	std::cout<<"          parameter <microseconds> gives the maximum permitted jitter during clock correction tests."<<std::endl;
	std::cout<<"    -I    use Improved Time Correction."<<std::endl;
	std::cout<<std::endl;
	std::cout<<"    (Time setting):"<<std::endl;
	std::cout<<"    -s    gives the start time of the session, defaults to current time."<<std::endl;
	std::cout<<"    -d    gives the duration of the session."<<std::endl;
	std::cout<<"    -f    gives the finish time of the session. Finish time overrides the duration"<<std::endl;
	std::cout<<"          parameter. "<<std::endl;
	std::cout<<"          (The time format in s, d, and f switches is <seconds>.<microseconds>. Microseconds are"<<std::endl;
	std::cout<<"          optional. Seconds are counted since 1.January 1970, see time(2)."<<std::endl;
	std::cout<<std::endl;
	std::cout<<"    (Source setting):"<<std::endl;
	std::cout<<"    -Z    gives the size of packets, using a CBR or Poisson source."<<std::endl;
	std::cout<<"    -b    gives the amount of bytes to be sent - overrides duration and finish time parameters."<<std::endl;
	std::cout<<"    -z    gives the amount of packets to be sent - overrides duration, finish time and bytes parameters."<<std::endl;
	std::cout<<"    -r    gives the data rate in bits/s for CBR source, defaults to 1 Mbit/s."<<std::endl;
	std::cout<<"    -F    <FifoFile>    start a FIFO source instead of a CBR source. FifoFile is the name of the FIFO file."<<std::endl;
	std::cout<<"    -O    starts a Poisson source instead of a CBR source (a data source with expotential distribution of"<<std::endl;
	std::cout<<"          interpacket times)."<<std::endl;
	std::cout<<std::endl;
	std::cout<<"    -h    help."<<std::endl;
	std::cout<<std::endl;
	std::cout<<"    Example:"<<std::endl;
	std::cout<<"    Start the server with: ./ltest -S"<<std::endl;
	std::cout<<"    and for UDP connection the client with: ./ltest <ServerAddress>"<<std::endl;
	std::cout<<"    or  for TCP connection the client with: ./ltest <ServerAddress>:5554 -p TCP -L <ClientAddress>:5554"<<std::endl;
	std::cout<<std::endl;
	std::cout<<"REMOTE SENDER MODE"<<std::endl;
	std::cout<<"with the -R option, ltest can be started in the remote sender mode. Hereby, the ltest client only controls the"<<std::endl;
	std::cout<<"measurement session, while the measurement is performed between the Gsoap server, given by the -C option, and"<<std::endl;
	std::cout<<"the Receiver, whose Gsoap address is giben by -R, or the data receiver address."<<std::endl;
	std::cout<<std::endl;
	std::cout<<"Enjoy it!"<<std::endl;
	exit(0);
} // --- end printUsage()

// E O F

