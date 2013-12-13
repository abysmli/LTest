//============================================================================
// Name        : SecurityCheck.h
// Author      : Li, Yuan
// email	   : abysmli@gmail.com
// Version     : 1.1
// Copyright   : All rights reserved by Anhalt University of Applied Sciences
// Description : Expiration time check class
//============================================================================
#ifndef SECURITYCHECK_H_
#define SECURITYCHECK_H_
#include <iostream>
#include <time.h>
#include <stdint.h>
#include "strtok.h"
#include "HighPerTimer.h"

class SecurityCheck
{
private:
	uint64_t iLeftValidity;
	std::string sLeftValidity;
	bool bExpirationFlag;
public:
	SecurityCheck();
	bool checkExpiration();
	uint64_t getIntLeftValidity();
	std::string getStringLeftValidity();
};

#endif /* SECURITYCHECK_H_ */
