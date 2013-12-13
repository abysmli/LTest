
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include <sstream>
#include <exception>
#include "HighPerTimer.h"
#include "soapltestService.h"
#include "network.h"

typedef std::map <std::string, int> OptionsMap;

/*! \enum UNIT
 * 	\brief UNIT for Session Configuration
 */
enum UNIT
{
	None, G, Gi, M, Mi, k, ki,
};

/// struct with ltest configuration elements
struct ConfLtestStruct {
  /// tells, if LTest should start as server
  bool  Server;
  /// the source type
  SourceType SType;
  /// the name of the fifo file
  std::string FifoFile;
  /// block receiver
  bool Blocking;
  /// activate client-server time correction
  bool TimeCorrection;
  unsigned int MaxTimeDev;
  /// local data address
  CIPAddress   LocalDataAddress;
  /// run in remote Sender modus
  bool RemoteSender;
  /// address of local GSOAP daemon
  GsoapAddress GsoapLocalAddress;
  /// address of the remote GSOAP daemon
  GsoapAddress GsoapRemoteAddress;
  /// address of the receiver GSOAP daemon
  GsoapAddress ReceiverGsoapAddress;
  /// receiver data address
  CIPAddress   ReceiverDataAddress;
  /// transport protocol
  proto_t      Proto;
  HPTimer::HighPerTimer StartTime;
  HPTimer::HighPerTimer FinishTime;
  HPTimer::HighPerTimer Duration;
  /// bytes to be transmitted
  unsigned long Bytes;
  /// packets to be transmitted
  unsigned long Packets;
  /// sending data rate, bps
  unsigned long long  BPS;
  /// packet size
  long Size;
  /// flag for either dump samples or not
  bool DumpSamples;
  /// filename for the dump file
  std::string DumpFile;
  /// priority of the thread
  int Priority;
  /// TOS field value
  char TOS;
  /// Protocol specific parameters. This parameters are passed through to the appropriated
  /// socket and interpreted by the socket classes
  SockOptsMap SocketOptions;
  /// run in web modus 
  bool WebMode;
  /// use ICA | added by Li, Yuan 24.08.2013
  bool ICA;

  /// default constructor, sets default values
  ConfLtestStruct():
    Server(false),
    SType(ST_CBR),               // CBR source type
    FifoFile(""),
    Blocking(false),
    TimeCorrection(false),
    MaxTimeDev(1000000),               // defaults to 1 ms
    RemoteSender(false),
    Proto(P_UDP),
    StartTime(HPTimer::HighPerTimer::Nil()),
    FinishTime(HPTimer::HighPerTimer::Nil()),
    Duration(HPTimer::HighPerTimer::Nil()),
    Bytes(1024*1024),
    Packets(0),
    BPS(1000*1000),               // bits per second, defaults to 1 Mbit/s
    Size(1024),                    // Bytes
    DumpSamples(false),
    DumpFile(""),
    Priority(-1),
    TOS(0),
    WebMode(false),
  	ICA(false)
  {
  }
}; // -- end of Struct ConfLtestStruct



/// the config class for ltest
class CCommandLine{
 public:
  /// dtor
  ~CCommandLine();
  /// initializer for the command line object
  static void instance(int argc, char *argv[]);

  /// accessor to the system configuration
  static CCommandLine * instance();

  /// accessor to the configuration structure
  static ConfLtestStruct * conf();

  /// not yet implemented
  const std::string & getStringOpt(const std::string & cmdLineOption);
  /// not yet implemented
  const int getIntOpt(const std::string & cmdLineOption);
  /// not yet implemented
  const bool getBoolOpt(const std::string & cmdLineOption);
  /// print the usage
  static void printUsage(const char * basename);
    
  /// for web mode: set conf struct obtained from web server 
  static void SetWebConf( ConfLtestStruct * pWebConf);
 private:
  // ctor
    CCommandLine();
    void parseCommandLine(int argc, char * argv[]);
    static CCommandLine *  mCommandLine;
    typedef std::map < std::string, std::string > Table;
    typedef Table::iterator TableIter;
    static ConfLtestStruct * mConf;

  /** parses the time string, the time string is assumed in the format
      sec[.usec], usec defaults to 0
*/
    static bool parseTimeString(const std::string pTimeStr, unsigned int & pSec,
			      unsigned int & pUsec);

  /// check the priority of redundant parameters
  void normalize();
  bool mCLineParsed;

  std::map < const char, std::string> OptionNameMap;
  Table mCmdOptTable;
};

/** configuration file format error events
 */
class CConfigException : public std::exception {
 public:
  /// standard ctor
  CConfigException(std::string pErrString="", int pCode = 0):
    mErrString(pErrString),
    mCode(pCode)
    {
    };
    /// dtor
  virtual ~CConfigException() throw(){
  };
  /// error code
  int code(){
    return mCode;
  };
  /// error string
  std::string what(){
    return mErrString;
  };
 private:
     std::string mErrString;
  int mCode;

};

/*! \class UnitUtils | added by Li, Yuan
 */
class UnitUtils
{
public:
	//! convert string to double with unit
	static double StringToDoubleWithUnit(std::string string, UNIT eUnit);
	//! check the unit of a string
	static UNIT checkUnit(std::string string);
};


#endif  //__CONFIG_H__

