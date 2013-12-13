 
#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <pthread.h>
#include <string>
#include <sstream>
#include <map>
#include <exception>
#include <vector>
#include <signal.h>
#include "network.h"
#include "DataSource.h"
#include "soapltestProxy.h"
#include "TimeCorrect.h"
#include "config.h"



/// main client control claass,
/// start ltest client, run a session and fetch as well as print out results
class GsoapClient {
 public:
  /// ctor
  GsoapClient(ltestProxy &proxy);
  /// dtor
  virtual ~GsoapClient();
  /// run the ltest client
  void Run();
  /// fetch results from the remote sender/receiver
  void FetchResults();
  
  // if caught SIGINT, SIGTERM, SIGHUP, clean up(release session)
  static void deal_signal(int dummy);
  
  ///signal handler for SIGHUP, SIGTERM, SIGINT
  void LtestHandler();
  
  /// for web mode: get data source ptr
  CDataSource* GetDataSource();
  
 private:
  /// data source ptr
  CDataSource * mDataSource;
  /// pointer of proxy class
  static ltestProxy *mProxy;
  /// The calculated time difference between sender and receiver
  HPTimer::HighPerTimer mDeltaTime;           
  
  
  
};

#endif  //__CLIENT_H__