#pragma once
#include "AppInfo.h"
#include "Utils\Layers\RenderingLayer.h"
#include "Utils\Surface\Surface.h"
#include "Utils\VeryCoolMath\Math.h"
#include <chrono>

class Level;
class RenderingLayer;
#include "Objects\Level.h"

struct ProcessInfo {
	int argc;
	char** argv;
	AppInfo* appInfo;
	RenderingLayer* renderingLayer = nullptr;
	Surface* surface;
	Level* level; // Should be replaced with layer of levels
	mat3x4 camera;
};

ProcessInfo* getProcessInfo();
float getRunningTime();