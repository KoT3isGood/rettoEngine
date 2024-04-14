#pragma once
#include <stdint.h>
class Mesh {
public:
	Mesh() = default;
	uint64_t instanceID;
	float rot[9];
	float pos[3];
};