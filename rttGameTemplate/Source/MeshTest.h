#pragma once

#include "../Main/SetProperties.h"
#include "Objects/Actor.h"
#include "Objects/Mesh.h"
#include "Utils/Logging/Logger.h"

class MeshTest : public Actor {
public:
	MeshTest();
	virtual void Tick(float deltaTime) override;
private:
	Mesh mesh = Mesh();
};