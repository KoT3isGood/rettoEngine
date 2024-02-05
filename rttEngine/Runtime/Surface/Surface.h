#pragma once
#include "Runtime\Logging\Logger.h"

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
};