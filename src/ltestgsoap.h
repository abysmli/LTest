//gsoap ns service name: ltest
//gsoap ns service namespaces: http://localhost/ltest.wsdl
//gsoap ns service location: http://localhost:5555
//gsoap ns service executable:ltest.cgi 
//gsoap ns service encoding: encoded
//gsoap ns schema namespaces: urn:ltest


// author:
// file:
// creation date:

#import "stlvector.h"


/// struct tv_type, like C tv stuct
  struct tv_type {
    uint32_t tv_sec;
    uint32_t tv_usec;
  };


/// struct with received sample data
struct sample_t {
  char Header[12];
};




/// key (string) - value (int) pair for socket options, to be passed to the receiver or
/// sender socket
struct sock_opt_t {
  std::string key;
  uint32_t    value;

};

/// struct with the whole sample data (sender as well as receiver data)
struct whole_sample_t {
  char Header[24];
};



/// protocol type
enum proto_t { P_TCP, P_UDP, P_SDP, P_UDT, P_FastTCP, P_HSTCP };

/// source type:  CBR: Constand bit rate, Poisson: poisson distribution,
/// FIFO: timestamps and packet length are read from the standard input in the format <seconds.microseconds>\t<packet lengh>
/// FS: full speed source - the data source is trying to emmitt data packets as soon as possible
enum SourceType { ST_CBR, ST_POISSON, ST_FIFO, ST_FS };

/// structure with source description, for the receiver, to know
/// transport parameters as well as start and stop time, etc.
struct sourceDesc_t {
  struct tv_type StartTime;
  struct tv_type StopTime;
  uint32_t sample_n;
  uint32_t maxPDUSize;
  uint32_t ListenAddr;
  uint16_t ListenPort;
  enum proto_t Proto;
  int32_t Prio; // process scheduling priority - between 0 and 100, defaults to zero
  char TOS; // IP TOS value
  bool Block; // session is blocked, so not second session on the same server at the same time will be accepted.
  std::vector<struct sock_opt_t> SockOpts;
};

/// structure with the detailled sender description
/// of the data sender
struct senderDesc_t {
  uint32_t  LocalAddr;
  uint16_t LocalPort;
  uint32_t  RemoteAddr;
  uint16_t RemotePort;
  std::string         RemoteURL;
  uint32_t  Bytes;
  uint32_t  Packets;
  enum proto_t        Proto;
  struct tv_type        StartTime;
  struct tv_type        Duration;
  struct tv_type        FinishTime;
  int32_t           Priority;
  enum SourceType     SType;
  char           TOS;
  /// CBR specific parameters
  uint64_t BPS;
  uint32_t  Size;
  /// FIFO specific parameters
  std::string         FifoName;
  bool        TimeCorrection;
  unsigned long  MaxTimeDev;
  std::vector<struct sock_opt_t> SockOpts;
};



/** statistics structure for transmitting statistics results
    between the remote sender and local sender
 */
struct  BasicSourceStats_t {
  struct tv_type       StartTime;
  struct tv_type       Duration;
  struct tv_type       FinishTime;
  uint32_t SentPackets;
  int64_t     SentBytes;
  uint32_t RecvPackets;
  int64_t     RecvBytes;
  uint32_t Duplicates;
  uint32_t Reordered;
  uint32_t Corrupted;
  int64_t     DeltaTime; // time correction value
  
};

/// The interface methods
//FIXME: each function has to have a comment as preamble
int ns__Port(uint16_t &pPort);

int ns__release_ssession( struct ReleaseSSessionResponse { } &response);

int ns__release_rsession( struct ReleaseRSessionResponse { } &response);

int ns__get_samples_whole( std::vector<struct whole_sample_t> & GetSampleList);

int ns__getBasicSourceStats( struct BasicSourceStats_t & GetBasicSourceStatics);

///remote ReceiverSessionIf interfaces
int ns__get_samples( std::vector<struct sample_t> &sample_list_t);

int ns__finish_session(uint32_t pLastSeqNo, struct FinishSessionResponse { } &response);

int ns__get_time_by_gsoap(struct tv_type & ptv);

int ns__start_UDP_time_port(uint16_t pPort, uint16_t & OPort);

int ns__release_UDP_time_port(struct ReleaseUDPTimePortResponse{ } &response);

///remote FlowIf interfaces
int ns__start_rsession( struct sourceDesc_t sDec, struct StartRSessionResponse { } &response);

int ns__start_ssession( struct sourceDesc_t sDec, struct senderDesc_t senderDesc, struct StartSSessionResponse { } &response);

int ns__print_sDesc( struct sourceDesc_t sDec, std::string & str);

int ns__ping(struct PingResponse{ } &response);

