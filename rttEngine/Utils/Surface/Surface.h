#pragma once
#include "Utils\Logging\Logger.h"

enum class SurfaceType {
	none = 0,
	win64,
	linux, // TODO: Implement Linux
	macos, // TODO: Implement MacOS
	ios,   // TODO: Implement IOS
};

class Surface {
public:
	SurfaceType type;

	// Constructor
	Surface();

	// Destructor
	~Surface();

	// Update surface
	virtual void StartUpdateLoop();

	float resolutionX = 0;
	float resolutionY = 0;
	float positionX = 0;
	float positionY = 0;

	virtual bool IsWindowActive();
};