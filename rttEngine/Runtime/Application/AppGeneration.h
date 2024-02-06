#pragma once

#include "AppInfo.h"
#include <iostream>
#include "Runtime\Logging\Logger.h"
#include "ProcessInfo.h"

#ifdef Win64
#include "Runtime/Surface/Win64/Win64Surface.h"
#ifdef ReleaseRTT
#define CREATE_GAME(appinfo) \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow){\
getProcessInfo()->argc = __argc; \
getProcessInfo()->argv = __argv; \
getProcessInfo()->appInfo = &appInfo; \
Win64Surface surface = Win64Surface(appInfo.name); \
surface.StartUpdateLoop(); return 0; \
}; 
#else
#define CREATE_GAME(appinfo) \
int main(int argc, char* argv[]) { \
getProcessInfo()->argc = argc; \
getProcessInfo()->argv = argv; \
getProcessInfo()->appInfo = &appInfo; \
Win64Surface surface = Win64Surface(appInfo.name); \
surface.StartUpdateLoop(); return 0; \
}; 
#endif
#endif