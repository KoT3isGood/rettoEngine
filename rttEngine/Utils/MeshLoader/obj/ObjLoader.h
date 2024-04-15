#pragma once
#include "Utils\Tokenizer\Tokenizer.h"
#include "Utils\Logging\Logger.h"
#include "Utils\MeshLoader\MeshData.h"
class OBJLoader {
public:
	OBJLoader() = default;
	OBJLoader(std::string file, MeshData* mesh);
private:
	Tokenizer tokenizer;
	int token = 0;
	uint32_t vertex=0;
	uint32_t index=0;
	void skipSpaces();
};