               
#include "Utils\Application\AppGeneration.h"

AppInfo appInfo{
	"rttGame",  // App Name
	"1.0.0.0",    // App Version
	"testGame", // App Description
	10			// Build ID
};

CREATE_GAME(appInfo)

#include "Objects\Level.h"
#include "MeshTest.h"

MeshTest meshTest = MeshTest();

void Init() {
	Level* level = GetCurrentLevel();
	level->InitActor(&meshTest);
	RTT_LOG("Inited");
};