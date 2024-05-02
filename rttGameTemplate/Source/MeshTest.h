#pragma once

#include "../Main/SetProperties.h"
#include "Objects/Actor.h"
#include "Objects/Mesh.h"
#include "Utils/Logging/Logger.h"
#include "Utils\IO\InputOutput.h"
#include "Utils\VeryCoolMath\Math.h"
#include "Utils\rttGUI\rttGUI.h"

class MeshTest : public Actor {
public:
	MeshTest();
	virtual void Tick(float deltaTime) override;
	float time=0.0;
private:
	Mesh mesh = Mesh();
};