#include "Camera.h"


Camera::Camera()
{
	RTT_LOG("Inited");
	cameraPos = vec3(0.7, 0, 9);
	
}

void Camera::Tick(float deltaTime)
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
		vec2 windowPos = vec2(GetProcessInfo()->surface->positionY + 7, GetProcessInfo()->surface->positionX + 31);
		vec2 absPos = mousePosX - windowPos - vec2((int)(GetProcessInfo()->surface->resolutionX * 0.5), (int)(GetProcessInfo()->surface->resolutionY * 0.5));
		int pos[2] = { int(windowPos.x + GetProcessInfo()->surface->resolutionX * 0.5), int(windowPos.y + GetProcessInfo()->surface->resolutionY * 0.5) };
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
	GetProcessInfo()->camera = getMatrixFromRotation(cameraRotation.y, -cameraRotation.x, 0.0);
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



	GetProcessInfo()->camera[0][3] = cameraPos.x;
	GetProcessInfo()->camera[1][3] = cameraPos.y;
	GetProcessInfo()->camera[2][3] = cameraPos.z;

	mat3x3 cube = getMatrixFromRotation(cameraRotation.y, cameraRotation.x, 0.0);
	cube *= vec3(0.5, 0.5, 0.5);
	mat3x3 cube2 = matrixYaw * matrixPitch;
	vec3 cubeRelativePos = cube2 * (vec3(1, 0.5, -1));


	rttGUI()->Text("Camera position: " + std::to_string(GetProcessInfo()->camera[0][3])+", "+ std::to_string(GetProcessInfo()->camera[1][3]) +", " + std::to_string(GetProcessInfo()->camera[2][3]));


	
	light.pos[0] = 0;
	light.pos[1] = 4.5;
	light.pos[2] = 7;
	light.size = 1;

	light.color[0] = 10;
	light.color[1] = 10;
	light.color[2] = 10;
	light.Render();

	
}
