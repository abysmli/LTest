#ifndef __IMPLEMENT_H__
#define __IMPLEMENT_H__

#include <pthread.h>
#include <string>
#include <sstream>
#include <map>
#include <exception>
#include <sys/types.h>
#include <vector>
#include "network.h"
#include "DataSource.h"
#include "TimeCorrect.h"
#include "soapltestService.h"
#include "soapltestProxy.h"
#include "network.h"

class SSessionIf_impl;
class RSessionIf_impl;
/// the implementation of the ltest interface
class FlowIF_impl//: virtual public FlowIF
{
public: 
    FlowIF_impl();
    virtual ~FlowIF_impl();
    /// routine for starting the receiver session
    void start_rsession(struct sourceDesc_t & sDesc);
    /// routine for starting the sender session
    void start_ssession(struct sourceDesc_t & sDesc, struct senderDesc_t & senderDesc);    
    std::string print_sDesc(struct sourceDesc_t &sDec);
    
    RSessionIf_impl * GetRSessionPtr();
    
    SSessionIf_impl * GetSSessionPtr();
    
    /// decrement the session counter in the server
    int32_t decrement_session_counter();
    
    bool Blocked();
    void Blocked(bool Block);
    ///finish the server and exit the program
    virtual void Exit();
    void ping();
    pthread_mutex_t mBlockedM;
    
    ltestProxy *flproxy;
    
private:
  ///mSession thread handler
  pthread_t mSessionThread;
  /// the server _is_blocked flag
  bool mBlocked;
  ///the session counter
  int32_t mSessionCounter;
  
  ///time till blocking is requested
  struct tv_type mBlockTill;
  ///pointer to the sender session
  SSessionIf_impl *mSSession;
  ///pointer to the receiver session
  RSessionIf_impl *mRSession;    
  
};

class SessionIf_impl//: virtual public SessionIf
{
public:
    SessionIf_impl(FlowIF_impl *pFlowIF_ptr);
    /// maximum length of the sample list
    static const uint32_t SAMPLE_LIST_MAX = 1000;
    /// close the connection of the pertinent socket
    void close_socket();
    
        /// get port address, assigned by operating system
    uint16_t Port();
    /// server port
    uint16_t mServerPort;
 
protected:
  ///dtor, protected, because should be only on a heap, so destroyed
  ///only by delete(this)
  virtual ~SessionIf_impl();
  CIPSocket * mSocket;
  CDataSource * mDataSource;
  
  ///finished flag
  bool mFinished;
  /// backlog processing finished, structures freezed flag
  bool mFreezed;
  /// Mutex for freezed flag
  pthread_mutex_t mFreezedM;
  /// freezed condition variable
  pthread_cond_t mFreezedC;
  /// pointer to the flow interface
  FlowIF_impl * mFlowIF_ptr;
  
  
  
};


class SSessionIf_impl: virtual public SessionIf_impl// virtual public SenderSessionIf,                       
{
public:
  ///ctor
  SSessionIf_impl(struct senderDesc_t & senderDesc, FlowIF_impl *pFlowIF);
  ///dtor
  virtual ~SSessionIf_impl();
  bool run_sender(struct senderDesc_t & senderDesc, struct sourceDesc_t & sDesc);
  ///delete session structure, deaktivate appropriated incarnation
  void release_session();
  /// get whole sample headers from the data sender
  void get_samples(std::vector<struct whole_sample_t> & whole_sample_list_t);
  /// return basic source statistics to the client, controlling session
  BasicSourceStats_t getBasicSourceStats();
  /// create a new thread for this session
  pthread_t createThread(int pPrio=-1);

private:
  /// the client object
  ltestProxy *rproxy;
  
};


class RSessionIf_impl: virtual public SessionIf_impl //virtual public ReceiverSessionIf,
                       
{
public:
  
  RSessionIf_impl(struct sourceDesc_t & sDesc, FlowIF_impl * pFlowIF);
  virtual ~RSessionIf_impl();
  ///get received samples headers
  void get_samples(std::vector<struct sample_t> & sample_list_t);
  ///set finish flag for the session and close the appropriate socket
  void finish_session(uint32_t pLastSeqNo);
  /// delete session structure, deactivate appropriate incarnation
  void release_session();
  /// get server time
  tv_type get_time_by_gsoap();
  ///start UDP listener port for the time syncronization
  uint16_t start_UDP_time_port(uint16_t pPort);
  /// release the listener port for the time syncronization
  void release_UDP_time_port();
  /// run the session
  void Run();
  /// create a new thread for this session
  pthread_t createThread(int pPrio=-1); 
  /** the UDP time listener implementation
   * should be private, but then the thread starting with that method doesn't work
   * Workarounds???
   */  
  void run_UDP_time();


  
private: 
   
  ///the UDP_time_port
  CUDPSocket * mUDPTimeSocket_ptr;
  /// finish the session and freeze the data source
  void ballout();
  ///flow describtion
  struct sourceDesc_t mDesc;
  /// the data receiver instance
  CDataReceiver * mReceiver;
  /// sequence number of the last sent packet
  uint32_t mLastSeqNo;
        
  /// The thread id of the UDP time corretion port thread
  pthread_t mUDPTimeThreadID;  
  
  ///finish flag for the UDP time port thread
  bool mUDPTimeFinished;
  
  /// data receiving loop finished
  bool mRecvFinished;
  
  
};

#endif //__IMPLEMENT_H__