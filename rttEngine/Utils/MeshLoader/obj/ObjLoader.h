#pragma once
#include "Utils\Logging\Logger.h"
#include "Utils\MeshLoader\MeshData.h"
#include <string>
#include <fstream>
#include <istream>
#include <filesystem>
#include <sstream>
class OBJLoader {
public:
	OBJLoader() = default;
	OBJLoader(std::string file, MeshData* mesh);
private:
	bool hasUv = true;
	int token = 0;
	uint32_t vertex = 0;
	uint32_t index = 0;
};