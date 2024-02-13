#pragma once
#include "AppInfo.h"
#include "Runtime\Layers\RenderingLayer.h"
#include "Runtime\Surface\Surface.h"
#include <chrono>

struct ProcessInfo {
	int argc;
	char** argv;
	AppInfo* appInfo;
	RenderingLayer* renderingLayer = nullptr;
	Surface* surface;
};

ProcessInfo* getProcessInfo();
float getRunningTime();