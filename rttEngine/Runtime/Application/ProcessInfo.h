#pragma once
#include "AppInfo.h"
#include "Runtime\Layers\RenderingLayer.h"

struct ProcessInfo {
	int argc;
	char** argv;
	AppInfo* appInfo;
	RenderingLayer* renderingLayer = nullptr;
};

ProcessInfo* getProcessInfo();