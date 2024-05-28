#pragma once
#include "../Main/SetProperties.h"
#include "Objects/Actor.h"
#include "Objects/Mesh.h"
#include "Utils/Logging/Logger.h"
#include "Utils\IO\InputOutput.h"
#include "Utils\VeryCoolMath\Math.h"
#include "Utils\rttGUI\rttGUI.h"
#include "Rendering\Vulkan\VulkanLayer.h"

class Sun : public Actor {
public:
	Sun();
	virtual void Tick(float deltaTime) override;
	Light light;
	float time=0.0;
};