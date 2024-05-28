#pragma once

#include "AppInfo.h"
#include <iostream>
#include "Utils\Logging\Logger.h"
#include "Objects\Level.h"
#include "ProcessInfo.h"

#ifdef Win64
#include "Utils/Surface/Win64/Win64Surface.h"
#ifdef ReleaseRTT
#define CREATE_GAME(appinfo) \
void Init(); \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow){\
getProcessInfo()->argc = __argc; \
getProcessInfo()->argv = __argv; \
getProcessInfo()->appInfo = &appInfo; \
Win64Surface surface = Win64Surface(appInfo.name); \
Level currentLevel = Level(); \
getProcessInfo()->level = &currentLevel; \
Init(); \
surface.StartUpdateLoop(); \
return 0; \
}; 
#else
#define CREATE_GAME(appinfo) \
void Init(); \
int main(int argc, char* argv[]) { \
getProcessInfo()->argc = argc; \
getProcessInfo()->argv = argv; \
getProcessInfo()->appInfo = &appInfo; \
Level currentLevel = Level(); \
getProcessInfo()->level = &currentLevel; \
Win64Surface surface = Win64Surface(appInfo.name); \
Init(); \
surface.StartUpdateLoop(); \
RTT_LOG("GAME WAS CLOSED SUCCESSFULLY, HAVE FUN IN DEVELOPMENT!"); \
return 0; \
}; 
#endif
#endif