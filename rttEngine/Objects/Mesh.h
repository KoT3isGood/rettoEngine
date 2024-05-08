#pragma once
#include <stdint.h>
struct ProcessInfo;
#include "Utils\Application\ProcessInfo.h"
class Mesh {
public:
	// Default constructor
	Mesh() = default;
	// Instance ID that will be accesed
	uint64_t instanceID = 0;
	// Rotation matrix of mesh
	float rot[9] = {1,0,0,0,1,0,0,0,1};
	// Position of mesh
	float pos[3] = {0,0,0};

	// Submits mesh to the rendering
	void Render();
};

class Light {
public:
	Light() = default;
	float pos[3] = { 0,0,0 };
	float size = 1.0;
	float color[3] = { 1,1,1 };
	void Render();
};
struct LightSourceShader {
	float pos[3] = { 0,0,0 };
	float size = 1.0;
	float color[3] = { 1,1,1 };
	float unused = 0.0;
};