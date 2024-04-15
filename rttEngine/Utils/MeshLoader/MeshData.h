#pragma once
#include <vector>
class MeshData {
public:
	MeshData();
	std::vector<float> vertices;
	std::vector<uint32_t> indicies;
};