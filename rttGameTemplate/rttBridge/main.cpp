
#define RTT_CURRENT_APP "rttGame"
#include "Runtime\Application\AppGeneration.h"

AppInfo appInfo{
	"rttGame",  // App Name
	"1.0.0",    // App Version
	"testGame", // App Description
	10			// Build ID
};

CREATE_GAME(appInfo)
