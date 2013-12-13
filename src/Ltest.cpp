#include <iostream>
#include <sys/stat.h>	// for open()
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <exception>
#include <algorithm>
#include "SessionIf_impl.h"
#include "TimeCorrect.h"
#include "DataSource.h"
#include "network.h"
#include "Client.h"
#include "Server.h"
#include "WebGUI.h"
#include "ltest.nsmap"
#include "SecurityCheck.h"

int main(int argc, char **argv)
{

	SecurityCheck securityInstance;
	if (securityInstance.checkExpiration())
	{
		std::cout << "License : Valid till " << securityInstance.getStringLeftValidity() << "\n\n";
#ifdef DEBUG_OUTPUT
		std::cout<< "DEBUG_INFORMATIONS: "<<std::endl;
		std::cout<< "License valid duration in seconds: " << securityInstance.getIntLeftValidity() << "\n";
		std::cout << "Command line: ";
		for (int i(0); i < argc; i++)
		{
			std::cout << argv[i] << " ";
		}
		std::cout << std::endl;
		std::cerr << "Version: 3-NewTP.5" << " Build: 1011"
				<< " compiled at " << __DATE__ <<" | "<<__TIME__<<std::endl<<std::endl;
#endif
		ConfLtestStruct * Conf;
		try
		{
			// Parse command line parameters
			CCommandLine::instance(argc, argv);
			// Access the system configuration
			Conf = CCommandLine::conf();
		} catch (const CNetwork_exception & ex)
		{
			std::cerr
					<< "Network exception caught during the system initialization. Reason: "
					<< ex.what() << " Code: " << ex.code() << std::endl;
			exit(1);
		}
		// check web mode firstly
		if (Conf->WebMode)
		{
			std::thread gSOAPServer([=]()
			{
				GsoapAddress gsoaplocaladdress = Conf->GsoapLocalAddress;
				ltestService mService(SOAP_IO_KEEPALIVE);
				FlowIF_impl flowif;
				GsoapServer server(mService, &flowif);
				server.Run();
			});
			gSOAPServer.detach();
			sleep(0.3);
			RunWebGUI();
		}
		else if (Conf->Server)
		{
			GsoapAddress gsoaplocaladdress = Conf->GsoapLocalAddress;
			ltestService mService(SOAP_IO_KEEPALIVE);
			FlowIF_impl flowif;
			GsoapServer server(mService, &flowif);
			server.Run();
		}
		else
		{
			try
			{
				std::string URL = Conf->GsoapRemoteAddress.GsoapURL();
				ltestProxy Proxy(URL.c_str(), SOAP_IO_KEEPALIVE);
				GsoapClient client(Proxy);
				client.LtestHandler();
				client.Run();
				client.FetchResults();
			} catch (const CNetwork_exception & ex)
			{
				std::cerr << "CNetwork exception occured. Reason: " << ex.code()
						<< " Code: " << ex.what() << std::endl;
			} catch (...)
			{
				std::cerr << "unknown exception caught" << std::endl;
			}
		}
	}
	else
	{
		std::cerr << securityInstance.getStringLeftValidity();
	}
	return 0;
}

