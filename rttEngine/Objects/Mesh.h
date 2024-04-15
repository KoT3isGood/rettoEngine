#pragma once
#include <stdint.h>
class Mesh {
public:
	Mesh() = default;
	uint64_t instanceID = -1;
	float rot[9] = {1,0,0,0,1,0,0,0,1};
	float pos[3] = {0,0,0};
	void Render();
};