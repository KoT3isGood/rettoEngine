#pragma once
#include <stdint.h>
struct ProcessInfo;
#include "Utils\Application\ProcessInfo.h"
class Mesh {
public:
	Mesh() = default;
	uint64_t instanceID = 0;
	float rot[9] = {1,0,0,0,1,0,0,0,1};
	float pos[3] = {0,0,0};
	void Render();
};