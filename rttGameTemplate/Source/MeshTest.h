#pragma once

#include "../Main/SetProperties.h"
#include "Objects/Actor.h"
#include "Objects/Mesh.h"
#include "Utils/Logging/Logger.h"
#include "Utils\IO\InputOutput.h"
#include "Utils\VeryCoolMath\Math.h"
#include "Utils\rttGUI\rttGUI.h"
#include "Rendering\Vulkan\VulkanLayer.h"

class MeshTest : public Actor {
public:
	MeshTest();
	virtual void Tick(float deltaTime) override;
	float time=0.0;
private:
	bool shouldDraw = true;
	Mesh mesh = Mesh();
	Light light = Light();
	vec3 cameraPos = vec3(0.0);
	vec2 cameraRotation = vec2(0.0);
	std::vector<vec3> poses;
	bool shouldLockMouse = false;
	bool isPressingEsc = false;
};