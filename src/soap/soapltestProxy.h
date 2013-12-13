/* soapltestProxy.h
   Generated by gSOAP 2.8.10 from ltestgsoap.h

Copyright(C) 2000-2012, Robert van Engelen, Genivia Inc. All Rights Reserved.
The generated code is released under one of the following licenses:
1) GPL or 2) Genivia's license for commercial use.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
*/

#ifndef soapltestProxy_H
#define soapltestProxy_H
#include "soapH.h"

class SOAP_CMAC ltestProxy
{ public:
	struct soap *soap;
	bool own;
	/// Endpoint URL of service 'ltestProxy' (change as needed)
	const char *soap_endpoint;
	/// Constructor
	ltestProxy();
	/// Constructor to use/share an engine state
	ltestProxy(struct soap*);
	/// Constructor with endpoint URL
	ltestProxy(const char *url);
	/// Constructor with engine input+output mode control
	ltestProxy(soap_mode iomode);
	/// Constructor with URL and input+output mode control
	ltestProxy(const char *url, soap_mode iomode);
	/// Constructor with engine input and output mode control
	ltestProxy(soap_mode imode, soap_mode omode);
	/// Destructor frees deserialized data
	virtual	~ltestProxy();
	/// Initializer used by constructors
	virtual	void ltestProxy_init(soap_mode imode, soap_mode omode);
	/// Delete all deserialized data (uses soap_destroy and soap_end)
	virtual	void destroy();
	/// Delete all deserialized data and reset to default
	virtual	void reset();
	/// Disables and removes SOAP Header from message
	virtual	void soap_noheader();
	/// Get SOAP Header structure (NULL when absent)
	virtual	const SOAP_ENV__Header *soap_header();
	/// Get SOAP Fault structure (NULL when absent)
	virtual	const SOAP_ENV__Fault *soap_fault();
	/// Get SOAP Fault string (NULL when absent)
	virtual	const char *soap_fault_string();
	/// Get SOAP Fault detail as string (NULL when absent)
	virtual	const char *soap_fault_detail();
	/// Close connection (normally automatic, except for send_X ops)
	virtual	int soap_close_socket();
	/// Force close connection (can kill a thread blocked on IO)
	virtual	int soap_force_close_socket();
	/// Print fault
	virtual	void soap_print_fault(FILE*);
#ifndef WITH_LEAN
	/// Print fault to stream
#ifndef WITH_COMPAT
	virtual	void soap_stream_fault(std::ostream&);
#endif

	/// Put fault into buffer
	virtual	char *soap_sprint_fault(char *buf, size_t len);
#endif

	/// Web service operation 'Port' (returns error code or SOAP_OK)
	virtual	int Port(unsigned short &pPort) { return Port(NULL, NULL, pPort); }
	virtual	int Port(const char *endpoint, const char *soap_action, unsigned short &pPort);

	/// Web service operation 'release-ssession' (returns error code or SOAP_OK)
	virtual	int release_ssession(struct ReleaseSSessionResponse &response) { return release_ssession(NULL, NULL, response); }
	virtual	int release_ssession(const char *endpoint, const char *soap_action, struct ReleaseSSessionResponse &response);

	/// Web service operation 'release-rsession' (returns error code or SOAP_OK)
	virtual	int release_rsession(struct ReleaseRSessionResponse &response) { return release_rsession(NULL, NULL, response); }
	virtual	int release_rsession(const char *endpoint, const char *soap_action, struct ReleaseRSessionResponse &response);

	/// Web service operation 'get-samples-whole' (returns error code or SOAP_OK)
	virtual	int get_samples_whole(std::vector<struct whole_sample_t >&GetSampleList) { return get_samples_whole(NULL, NULL, GetSampleList); }
	virtual	int get_samples_whole(const char *endpoint, const char *soap_action, std::vector<struct whole_sample_t >&GetSampleList);

	/// Web service operation 'getBasicSourceStats' (returns error code or SOAP_OK)
	virtual	int getBasicSourceStats(struct BasicSourceStats_t &GetBasicSourceStatics) { return getBasicSourceStats(NULL, NULL, GetBasicSourceStatics); }
	virtual	int getBasicSourceStats(const char *endpoint, const char *soap_action, struct BasicSourceStats_t &GetBasicSourceStatics);

	/// Web service operation 'get-samples' (returns error code or SOAP_OK)
	virtual	int get_samples(std::vector<struct sample_t >&sample_list_t) { return get_samples(NULL, NULL, sample_list_t); }
	virtual	int get_samples(const char *endpoint, const char *soap_action, std::vector<struct sample_t >&sample_list_t);

	/// Web service operation 'finish-session' (returns error code or SOAP_OK)
	virtual	int finish_session(unsigned int pLastSeqNo, struct FinishSessionResponse &response) { return finish_session(NULL, NULL, pLastSeqNo, response); }
	virtual	int finish_session(const char *endpoint, const char *soap_action, unsigned int pLastSeqNo, struct FinishSessionResponse &response);

	/// Web service operation 'get-time-by-gsoap' (returns error code or SOAP_OK)
	virtual	int get_time_by_gsoap(struct tv_type &ptv) { return get_time_by_gsoap(NULL, NULL, ptv); }
	virtual	int get_time_by_gsoap(const char *endpoint, const char *soap_action, struct tv_type &ptv);

	/// Web service operation 'start-UDP-time-port' (returns error code or SOAP_OK)
	virtual	int start_UDP_time_port(unsigned short pPort, unsigned short &OPort) { return start_UDP_time_port(NULL, NULL, pPort, OPort); }
	virtual	int start_UDP_time_port(const char *endpoint, const char *soap_action, unsigned short pPort, unsigned short &OPort);

	/// Web service operation 'release-UDP-time-port' (returns error code or SOAP_OK)
	virtual	int release_UDP_time_port(struct ReleaseUDPTimePortResponse &response) { return release_UDP_time_port(NULL, NULL, response); }
	virtual	int release_UDP_time_port(const char *endpoint, const char *soap_action, struct ReleaseUDPTimePortResponse &response);

	/// Web service operation 'start-rsession' (returns error code or SOAP_OK)
	virtual	int start_rsession(struct sourceDesc_t sDec, struct StartRSessionResponse &response) { return start_rsession(NULL, NULL, sDec, response); }
	virtual	int start_rsession(const char *endpoint, const char *soap_action, struct sourceDesc_t sDec, struct StartRSessionResponse &response);

	/// Web service operation 'start-ssession' (returns error code or SOAP_OK)
	virtual	int start_ssession(struct sourceDesc_t sDec, struct senderDesc_t senderDesc, struct StartSSessionResponse &response) { return start_ssession(NULL, NULL, sDec, senderDesc, response); }
	virtual	int start_ssession(const char *endpoint, const char *soap_action, struct sourceDesc_t sDec, struct senderDesc_t senderDesc, struct StartSSessionResponse &response);

	/// Web service operation 'print-sDesc' (returns error code or SOAP_OK)
	virtual	int print_sDesc(struct sourceDesc_t sDec, std::string &str) { return print_sDesc(NULL, NULL, sDec, str); }
	virtual	int print_sDesc(const char *endpoint, const char *soap_action, struct sourceDesc_t sDec, std::string &str);

	/// Web service operation 'ping' (returns error code or SOAP_OK)
	virtual	int ping(struct PingResponse &response) { return ping(NULL, NULL, response); }
	virtual	int ping(const char *endpoint, const char *soap_action, struct PingResponse &response);
};
#endif