#include "ObjLoader.h"

OBJLoader::OBJLoader(std::string file)
{
	tokenizer = Tokenizer(file);
}
