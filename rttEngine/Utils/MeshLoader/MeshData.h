#pragma once
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
struct Material {
	float albedo[3] = {0,0,0};
	float metalness = 0.0;
	float roughness = 0.0;
	uint32_t albedoTexture = 0;
	uint32_t normalTexture = 0;
};
class MeshData {
public:
	MeshData();
	std::vector<float> vertices;
	std::vector<uint32_t> indicies;
	std::vector<float> uvs;
	std::vector<uint32_t> uvIndicies;
	std::vector<uint32_t> materialSizes;
	std::unordered_map<std::string, uint32_t> materials;
	std::map <std::string, Material> materialsArray;
};