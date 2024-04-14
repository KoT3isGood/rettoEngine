#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <optional>

class Tokenizer {
public:
	Tokenizer() = default;
	Tokenizer(std::string file);
	std::vector<std::string> tokens;

	std::string operator[](const int token) {
		return tokens[token];
	}
private:
	int currentCharacter = 0;
	std::string getNextToken(std::string* text);
};