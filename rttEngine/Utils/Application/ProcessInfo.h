#pragma once
#include "AppInfo.h"
#include "Utils\Layers\RenderingLayer.h"
#include "Utils\Surface\Surface.h"
#include "Utils\VeryCoolMath\Math.h"
#include "Utils\AssetRegistry\AssetRegistry.h"
#include <chrono>

class Level;
class RenderingLayer;
#include "Objects\Level.h"

// ProcessInfo is a very big buffer that communicates with an Engine
struct ProcessInfo {
	int argc;
	char** argv;
	AppInfo* appInfo;
	RenderingLayer* renderingLayer = nullptr;
	Surface* surface;
	Level* level;
	mat3x4 camera;
	AssetRegistry assetRegistry;
};
// Returns memory address of Engine instance
ProcessInfo* GetProcessInfo();
// Used to get rough time from the start of the app, used for deltaTime
float getRunningTime();