#include "MeshTest.h"


MeshTest::MeshTest()
{
	RTT_LOG("Inited");
	cameraPos = vec3(-5, 0, 2);
}

void MeshTest::Tick(float deltaTime)
{
	time += deltaTime;
	rttGUI()->Window("Profiler", vec4(0, 0, 200, 200));
	rttGUI()->Text("frame time: " + std::to_string(int(deltaTime * 100000) / 100.0) + "ms");

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
	vec2 absPos = mousePosX - windowPos - vec2(getProcessInfo()->surface->resolutionX * 0.5, getProcessInfo()->surface->resolutionY * 0.5) + vec2(0, 0.5);
	int pos[2] = { int(windowPos.x + getProcessInfo()->surface->resolutionX * 0.5), int(windowPos.y + getProcessInfo()->surface->resolutionY * 0.5) };
	if (shouldLockMouse) {
		SetMousePosition(pos);
		cameraRotation += absPos * vec2(0.0174532925*0.25);
	};

	
	getProcessInfo()->camera = getMatrixFromRotation(cameraRotation.y, -cameraRotation.x, 0.0);
	mat3 matrixPitch = getMatrixFromRotation(-cameraRotation.y, 0.0, 0.0);
	mat3 matrixYaw = getMatrixFromRotation(0.0, -cameraRotation.x, 0.0);
	vec3 frontalDirection = matrixYaw* matrixPitch * vec3(1.0, 0.0, 0.0);
	vec3 sizeDirection = matrixYaw * vec3(0.0, -1.0, 0.0);

	if (IsKeyDown(0x41)) {
		rttGUI()->Text("A");
		cameraPos += deltaTime * 2* sizeDirection;
	};
	if (IsKeyDown(0x44)) {
		rttGUI()->Text("D");
		cameraPos -= deltaTime * 2* sizeDirection;
	};
	if (IsKeyDown(0x57)) {
		rttGUI()->Text("W");
		cameraPos += deltaTime * 2*frontalDirection;
	};
	if (IsKeyDown(0x53)) {
		rttGUI()->Text("S");
		cameraPos -= deltaTime * 2*frontalDirection;
	};



	getProcessInfo()->camera[0][3] = cameraPos.x;
	getProcessInfo()->camera[1][3] = cameraPos.y;
	getProcessInfo()->camera[2][3] = cameraPos.z;


	rttGUI()->Text("Camera position: " + std::to_string(getProcessInfo()->camera[0][3])+", "+ std::to_string(getProcessInfo()->camera[1][3]) +", " + std::to_string(getProcessInfo()->camera[2][3]));

	mesh.instanceID = getProcessInfo()->assetRegistry.meshes["Content/Meshes/sponza.obj"];
	mesh.Render();
	mesh.instanceID = getProcessInfo()->assetRegistry.meshes["Content/Meshes/cube.obj"];
	mesh.Render();

	

	
	
	if (IsKeyDown(0x01)) {
		vec3 lightPos = cameraPos + matrixYaw * matrixPitch * vec3(1.0, 0.5, -0.25);
		light.pos[0] = lightPos.x;
		light.pos[1] = lightPos.y;
		light.pos[2] = lightPos.z;
		light.size = 0.25;

		light.color[0] = 3.0;
		light.color[1] = 3.0;
		light.color[2] = 3.0;
		light.Render();
	}
}
