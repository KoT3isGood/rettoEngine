#include "Tokenizer.h"

Tokenizer::Tokenizer(std::string fileDirectory)
{
	currentCharacter = 0;
	std::string myText = "";

	std::ifstream MyReadFile(fileDirectory);
	if (MyReadFile.good()) {
		// Use a while loop together with the getline() function to read the file line by line
		while (std::getline(MyReadFile, myText)) {
			// Output the text from the file

			while (currentCharacter < myText.size()) {
				std::string token = getNextToken(&myText);
				tokens.push_back(token);
			}
			tokens.push_back("\n");
			currentCharacter = 0;
		}
		// Close the file
		MyReadFile.close();
	}
}
std::string Tokenizer::getNextToken(std::string* text)
{
	std::string token = "";
	std::string val = "";
	int starterVal = currentCharacter;

	std::vector<std::string> bannedTokens = {};

	for (int i = currentCharacter; i < size(*text); i++) {

		char name = char(text->at(currentCharacter)) + 0;
		val = name;
		if (!std::isalnum(name)) {
			if (currentCharacter == starterVal) {
				token += val;
				currentCharacter += 1;
			}


			break;
		}
		token += val;
		currentCharacter += 1;
	}
	return token;
}