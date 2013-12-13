//============================================================================
// Name        : SecurityCheck.cpp
// Author      : Li, Yuan
// email	   : abysmli@gmail.com
// Version     : 1.1
// Copyright   : All rights reserved by Anhalt University of Applied Sciences
// Description : Expiration time check class
//============================================================================

#include "SecurityCheck.h"

SecurityCheck::SecurityCheck()
{
	std::string sBuildDate(__DATE__);
	std::string sBuildTime(__TIME__);
	CStringTokenizer CST(sBuildDate, " ");
	std::string sMonth = CST.nextToken();
	std::string sDay = CST.nextToken();
	std::string sYear = CST.nextToken();
	if (sMonth[0] == 'J' && sMonth[1] == 'a' && sMonth[2] == 'n')
	{
		sMonth = "01";
	}
	else if (sMonth[0] == 'F')
	{
		sMonth = "02";
	}
	else if (sMonth[0] == 'M' && sMonth[1] == 'a' && sMonth[2] == 'r')
	{
		sMonth = "03";
	}
	else if (sMonth[0] == 'A' && sMonth[1] == 'p')
	{
		sMonth = "04";
	}
	else if (sMonth[0] == 'M' && sMonth[1] == 'a' && sMonth[2] == 'y')
	{
		sMonth = "05";
	}
	else if (sMonth[0] == 'J' && sMonth[1] == 'u' && sMonth[2] == 'n')
	{
		sMonth = "06";
	}
	else if (sMonth[0] == 'J' && sMonth[1] == 'u' && sMonth[2] == 'l')
	{
		sMonth = "07";
	}
	else if (sMonth[0] == 'A' && sMonth[1] == 'u')
	{
		sMonth = "08";
	}
	else if (sMonth[0] == 'S')
	{
		sMonth = "09";
	}
	else if (sMonth[0] == 'O')
	{
		sMonth = "10";
	}
	else if (sMonth[0] == 'N')
	{
		sMonth = "11";
	}
	else
	{
		sMonth = "12";
	}
	std::string sTime(sYear + "-" + sMonth + "-" + sDay + " " + sBuildTime);
	struct tm tm;
	time_t epoch;
	if (strptime(sTime.c_str(), "%Y-%m-%d %H:%M:%S", &tm) != NULL)
	{
		epoch = mktime(&tm);
	}
	else
	{
		std::cerr << "\nConverte Build Time failed!\n";
		exit(-1);
	}
	HPTimer::HighPerTimer CBuildTime(epoch);
	HPTimer::HighPerTimer CCurrentTime;
	CCurrentTime.SetNow();
	iLeftValidity = 15552000 - (CCurrentTime - CBuildTime).Seconds();
	sLeftValidity = "";
	if (iLeftValidity <= 86400 && iLeftValidity > 0)
	{
		sLeftValidity = CStringUtils::ftos(iLeftValidity / 3600.0 + 1) + " hours";
	}
	else if (iLeftValidity > 86400 && iLeftValidity <= 2592000)
	{
		sLeftValidity = CStringUtils::ftos(iLeftValidity / 86400 + 1) + " days";
	}
	else if (iLeftValidity > 2592000 && iLeftValidity <= 31104000)
	{
		sLeftValidity = CStringUtils::ftos(iLeftValidity / 2592000 + 1) + " months";
	}
	else if (iLeftValidity > 31104000)
	{
		sLeftValidity = CStringUtils::ftos(iLeftValidity / 31104000 + 1) + " years";
	}
	else
	{
		sLeftValidity = "License Expired!";
		bExpirationFlag = false;
	}
	bExpirationFlag = true;
}

bool SecurityCheck::checkExpiration()
{
	return bExpirationFlag;
}

uint64_t SecurityCheck::getIntLeftValidity()
{
	return iLeftValidity;
}

std::string SecurityCheck::getStringLeftValidity()
{
	return sLeftValidity;
}
