#pragma once
#include "Utils\Logging\Logger.h"
#include <unordered_map>
#include <string>
#include <filesystem>

class AssetRegistry {
public:
	AssetRegistry();
	std::unordered_map<std::string, uint32_t> meshes;
	std::unordered_map<std::string, uint32_t> textures;
	std::unordered_map<std::string, uint32_t> materials;
};