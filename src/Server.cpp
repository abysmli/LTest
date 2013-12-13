#include "Server.h"
#include "config.h"
#include <stdlib.h>

std::queue<SOAP_SOCKET> GsoapServer::mSock;
 
std::mutex GsoapServer::mMut;
std::condition_variable GsoapServer::mEmptyCondition;
std::vector<std::thread> GsoapServer::threads;
constexpr int GsoapServer::MAX_THR;




GsoapServer::GsoapServer(ltestService &service, FlowIF_impl *flowif)
{
  ConfLtestStruct * Conf = CCommandLine::conf(); 
  GsoapAddress gsoaplocaladdress=Conf->GsoapLocalAddress;  
   mService = &service;
   mService->soap->bind_flags=SO_REUSEADDR;
   mService->soap->user = (void*)flowif;

   ltestService *srv_thr[MAX_THR];
  
   SOAP_SOCKET m = mService->bind(NULL, gsoaplocaladdress.Port(), 100); 
   while(!soap_valid_socket(m))
   {
     mService->soap_stream_fault(std::cerr);
     m = mService->bind(NULL, gsoaplocaladdress.Port(), 100);    
   }
   std::cout<<"socket bind successful "<< m<< std::endl; 
   for(int i=0; i< MAX_THR; i++)
   {
     srv_thr[i] = mService->copy();
     //std::cout<< "starting thread "<< i << std::endl;
     threads.push_back(std::thread(process_queue, srv_thr[i]));
    
   }
}


void 
GsoapServer::enqueue(SOAP_SOCKET psock)
{

  /// block execution here, if other thread already locked mMut
  std::lock_guard<std::mutex> lock(mMut);
  /// if were here no other thread is owned/lock mMut. so we can modify the internal data
  mSock.push(psock);
  /// OK! notify one of threads which was blocked because, queue empty 
  mEmptyCondition.notify_one();
  
}/// lock.unlock()! does not have such method! mMut is now unlocked in dctor of lock_guard 


std::shared_ptr<SOAP_SOCKET>
GsoapServer::dequeue()
{
  ///own the mMut like in enqueue method except that lock can be unlocked manually without need to destructor of lock!
  std::unique_lock<std::mutex> lock(mMut);
  /// if we are here. mMute is locked and no other thread can access/modify the data!
  /// wait() method first checks if mData is not empty, allowes execution to go on. else :
  /// unlocks the mMut and waits for signla.
  /// because mMute is released other threads have a chance to Push new data into queue
  /// ... in notify this condition variable!
  while(mSock.empty())
  {
    mEmptyCondition.wait(lock);
  }
  /// if we are are here, mData is not empty and mMut is locked !
  std::shared_ptr<SOAP_SOCKET> ret = std::shared_ptr<SOAP_SOCKET>(std::make_shared<SOAP_SOCKET>(mSock.front()));
  mSock.pop();
  return ret;
  
}


void *
GsoapServer::process_queue(ltestService * service)
{
  
  for(;;)
  {
        std::shared_ptr<SOAP_SOCKET> sock= dequeue();
        service->soap->socket = *sock;
        if (!soap_valid_socket(service->soap->socket))
        {
         break;
        }
        service->serve();
        service->destroy();
        std::cerr<< "served"<<std::endl;
  }
  return NULL;
}

void 
GsoapServer::Run()
{
  for(;;)
  {
       SOAP_SOCKET s = mService->accept();
       if(SOAP_INVALID_SOCKET== s)
       { 
        if(mService->soap->errnum)
        {
          mService->soap_stream_fault(std::cerr);
          continue;
        }
        else 
        {
          std::cout<<  "server timed out"<< std::endl;
          break;
        }
        exit(-1);
       }
       enqueue(s);
    
  }
  
  for(int i=0; i< MAX_THR; i++)
  {
     std::cout<< "waiting for thread "<< i<< "to terminate..."<< std::endl;
     threads.at(i).join();
     std::cout<< "terminated"<< std::endl;
  }  
}



GsoapServer::~GsoapServer()
{
    if(mService!=NULL)
     delete mService;
  
}















