               
#include "Utils\Application\AppGeneration.h"

AppInfo appInfo{
	"rttGame",  // App Name
	"1.0.0.0",    // App Version
	"testGame", // App Description
	10			// Build ID
};

CREATE_GAME(appInfo)

#include "Objects\Level.h"
#include "Camera.h"
#include <Sun.h>
#include <StaticMesh.h>

Camera cam = Camera();
Sun sun = Sun();
StaticMesh testMesh = StaticMesh();

void Init() {

	Level* level = GetCurrentLevel();
	level->InitActor(&cam);
	//level->InitActor(&sun);
	level->InitActor(&testMesh);
	testMesh.position = vec3(1, 1, 1);
	RTT_LOG("Inited");
};