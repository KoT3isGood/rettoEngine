#pragma once
#include "../Main/SetProperties.h"
#include "Objects/Actor.h"
#include "Objects/Mesh.h"
#include "Utils/Logging/Logger.h"
#include "Utils\IO\InputOutput.h"
#include "Utils\VeryCoolMath\Math.h"
#include "Utils\rttGUI\rttGUI.h"
#include "Rendering\Vulkan\VulkanLayer.h"

class StaticMesh : public Actor {
public:
	StaticMesh();
	virtual void Tick(float deltaTime) override;
	mat3 rotation;
	vec3 position;
private:

};