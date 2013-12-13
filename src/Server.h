 
#ifndef __SERVER_H__
#define __SERVER_H__

#include <string>
#include <sstream>
#include <map>
#include <exception>

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <vector>

#include "network.h"
#include "DataSource.h"
#include "soapltestService.h"
#include "SessionIf_impl.h"

/// the main LTest server object
class GsoapServer {
 public:
  /// ctor
  GsoapServer(ltestService &service, FlowIF_impl *flowif);
  /// dtor
  virtual ~GsoapServer();
  /// main running loop for the LTest server
  void Run();
  
  ///push SOAP_SOCKET in the queue 
  void enqueue(SOAP_SOCKET psock);  
  ///pop SOAP_SOCKET from the queue 
  static std::shared_ptr<SOAP_SOCKET> dequeue(); 
  /// deal with the SOAP_SOCKET in the queue 
  static void * process_queue(ltestService * service);  
  
 private:
  /// the pointer of service class
  ltestService * mService;
  FlowIF_impl flowif;
  
  static std::queue<SOAP_SOCKET> mSock;
  static std::mutex mMut;
  static std::condition_variable mEmptyCondition;
  static  std::vector<std::thread> threads; 
  static constexpr int MAX_THR = 50 ;

};

#endif  //__CSERVER_H__