#pragma once
#include "AppInfo.h"
#include "Runtime\Layers\RenderingLayer.h"
#include "Runtime\Surface\Surface.h"

struct ProcessInfo {
	int argc;
	char** argv;
	AppInfo* appInfo;
	RenderingLayer* renderingLayer = nullptr;
	Surface* surface;
};

ProcessInfo* getProcessInfo();