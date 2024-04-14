#pragma once
#include "Utils\Tokenizer\Tokenizer.h"

class OBJLoader {
public:
	OBJLoader() = default;
	std::vector<float> vertices;
	std::vector<uint32_t> indicies;
private:
	Tokenizer tokenizer;
	OBJLoader(std::string file);
};