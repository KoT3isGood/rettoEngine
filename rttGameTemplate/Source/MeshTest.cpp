#include "MeshTest.h"


MeshTest::MeshTest()
{
	RTT_LOG("Inited");
	cameraPos = vec3(0.7, 20, 9);
}

void MeshTest::Tick(float deltaTime)
{
	// DATA PERFOMANCE
	time += deltaTime;
	time2 += deltaTime;
	rttGUI()->Window("Profiler", vec4(0, 0, 200, 200));
	rttGUI()->Text("frame time: " + std::to_string(int(deltaTime * 100000) / 100.0) + "ms");
	if (time2 >= 1.0) {
		time2 = 0;
		RTT_LOG(std::to_string(int(deltaTime * 100000) / 100.0) + "ms");
		RTT_LOG(std::to_string(int(1/deltaTime)) + " FPS");
	}

	// BASIC CAMERA MOVEMENT
	// ROTATION
	if (IsGameActive()) {
		if (IsKeyDown(VK_ESCAPE)) {
			rttGUI()->Text("ESC: " + std::to_string(shouldLockMouse));
			if (!isPressingEsc) {
				shouldLockMouse = shouldLockMouse ? false : true;
			}
			isPressingEsc = true;
		}
		else {
			isPressingEsc = false;
		};

		vec2 mousePosX = vec2(GetMousePosition()[0], GetMousePosition()[1]);
		vec2 windowPos = vec2(getProcessInfo()->surface->positionY + 7, getProcessInfo()->surface->positionX + 31);
		vec2 absPos = mousePosX - windowPos - vec2((int)(getProcessInfo()->surface->resolutionX * 0.5), (int)(getProcessInfo()->surface->resolutionY * 0.5));
		int pos[2] = { int(windowPos.x + getProcessInfo()->surface->resolutionX * 0.5), int(windowPos.y + getProcessInfo()->surface->resolutionY * 0.5) };
		if (shouldLockMouse) {
			SetMousePosition(pos);
			if (previousState&&shouldLockMousePrev) {
				cameraRotation += absPos * vec2(0.0174532925 * 0.25);
			}
		};
	}
	previousState = IsGameActive();
	shouldLockMousePrev = shouldLockMouse;
	
	// POSITION
	getProcessInfo()->camera = getMatrixFromRotation(cameraRotation.y, -cameraRotation.x, 0.0);
	mat3 matrixPitch = getMatrixFromRotation(-cameraRotation.y, 0.0, 0.0);
	mat3 matrixYaw = getMatrixFromRotation(0.0, -cameraRotation.x, 0.0);
	vec3 frontalDirection = matrixYaw* matrixPitch * vec3(1.0, 0.0, 0.0);
	vec3 sizeDirection = matrixYaw * vec3(0.0, -1.0, 0.0);

	if (IsGameActive()) {
		if (IsKeyDown(0x41)) {
			rttGUI()->Text("A");
			cameraPos += deltaTime * 4 * sizeDirection;
		};
		if (IsKeyDown(0x44)) {
			rttGUI()->Text("D");
			cameraPos -= deltaTime * 4 * sizeDirection;
		};
		if (IsKeyDown(0x57)) {
			rttGUI()->Text("W");
			cameraPos += deltaTime * 4 * frontalDirection;
		};
		if (IsKeyDown(0x53)) {
			rttGUI()->Text("S");
			cameraPos -= deltaTime * 4 * frontalDirection;
		};
	}

	getProcessInfo()->camera[0][3] = cameraPos.x;
	getProcessInfo()->camera[1][3] = cameraPos.y;
	getProcessInfo()->camera[2][3] = cameraPos.z;


	rttGUI()->Text("Camera position: " + std::to_string(getProcessInfo()->camera[0][3])+", "+ std::to_string(getProcessInfo()->camera[1][3]) +", " + std::to_string(getProcessInfo()->camera[2][3]));
	
	for (int i = 0; i < 10;i++) {

		mesh.pos[0] = i * 40;
		mesh.pos[1] = 0;
		mesh.pos[2] = 0;
		mesh.instanceID = 2;
		mesh.Render();
	}


	

	
	/*if (IsGameActive()) {
		if (IsKeyDown(0x01)) {*/
			//vec3 lightPos = cameraPos + matrixYaw * matrixPitch * vec3(1.0, 0.5, -0.25);
			
	/*	}
	}*/
	/*light.pos[0] = sin(time*0.5)*10;
	light.pos[1] = 20;
	light.pos[2] = 6;
	light.size = 0.5;

	light.color[0] = 1.000000 * 10;
	light.color[1] = 0.850980 * 10;
	light.color[2] = 0.713725 * 10;
	light.Render();*/

	light.pos[0] = sin(time * 0.5+3.14) * 9;
	light.pos[1] = 20;
	light.pos[2] = 6;
	light.size = 1;

	light.color[0] = 1.000000 * 10;
	light.color[1] = 0.2 * 10;
	light.color[2] = 0.2 * 10;
	light.Render();

	light.pos[0] = 20;
	light.pos[1] = 20;
	light.pos[2] = 100;
	light.size = 1;

	light.color[0] = 1.000000 * 1000;
	light.color[1] = 1.0 * 1000;
	light.color[2] = 1.0 * 1000;
	light.Render();
}
