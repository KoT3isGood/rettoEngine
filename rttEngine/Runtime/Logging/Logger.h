#pragma once
#include "Runtime/Application/AppInfo.h"

#include <iostream>
#include <Windows.h>
#include <string>
#include <assert.h>

#ifndef RTT_CURRENT_APP
#define RTT_CURRENT_APP "rtt"
#endif

void logData(std::string data, std::string runnerName);
#define RTT_LOG(x) logData(x,RTT_CURRENT_APP)

#define RTT_ASSERT(x) assert(x);