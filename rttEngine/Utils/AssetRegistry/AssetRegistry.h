#pragma once
#include "Utils\Logging\Logger.h"
#include <map>
#include <unordered_map>
#include <string>
#include <filesystem>

class AssetRegistry {
public:
	AssetRegistry();
	std::map<std::string, uint32_t> meshes;
	std::map<std::string, uint32_t> textures;
	std::map<std::string, uint32_t> materials;
};