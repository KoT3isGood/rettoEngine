#include "ObjLoader.h"

OBJLoader::OBJLoader(std::string file, MeshData* mesh)
{
	tokenizer = Tokenizer(file);
	for (token = 0; token < tokenizer.tokens.size(); token++) {


		

		// We should skip if starting thing == #

		if (tokenizer[token] == "#") {
			token++;
			while (tokenizer[token] != "\n") {
				token++;
			}

		};
		// Handle materials
		if (tokenizer[token] == "mtllib") {
			std::string materialName = "";
			skipSpaces();
			token++;
			while (tokenizer[token] != "\n") {
				materialName += tokenizer[token];
				token++;
			}

		};

		if (tokenizer[token] == "o") {
			std::string objectName = "";
			skipSpaces();
			token++;
			while (tokenizer[token] != "\n") {
				objectName += tokenizer[token];
				token++;
			}

		};

		if (tokenizer[token] == "v") {
			
			std::string v1 = "";
			std::string v2 = "";
			std::string v3 = "";
			skipSpaces();
			token++;
			while (tokenizer[token] != " ") {
				v1 += tokenizer[token];
				token++;
			}
			skipSpaces();
			token--;
			while (tokenizer[token] != " ") {
				v2 += tokenizer[token];
				token++;
			}
			skipSpaces();
			token--;
			while (tokenizer[token] != "\n" && tokenizer[token] != " ") {
				v3 += tokenizer[token];
				token++;
			}	
			mesh->vertices.resize(mesh->vertices.size()+3);
			mesh->vertices[vertex*3] = std::stof(v1);
			mesh->vertices[vertex*3+1] = std::stof(v2);
			mesh->vertices[vertex*3+2] = std::stof(v3);
			vertex++;
		};

		if (tokenizer[token] == "vn") {
			std::string v1 = "";
			std::string v2 = "";
			std::string v3 = "";
			skipSpaces();
			token++;
			while (tokenizer[token] != " ") {
				v1 += tokenizer[token];
				token++;
			}
			skipSpaces();
			token--;
			while (tokenizer[token] != " ") {
				v2 += tokenizer[token];
				token++;
			}
			skipSpaces();
			token--;
			while (tokenizer[token] != "\n" && tokenizer[token] != " ") {
				v3 += tokenizer[token];
				token++;
			}
		};

		if (tokenizer[token] == "vt") {
			std::string v1 = "";
			std::string v2 = "";
			skipSpaces();
			token++;
			while (tokenizer[token] != " ") {
				v1 += tokenizer[token];
				token++;
			}
			skipSpaces();
			token--;
			while (tokenizer[token] != "\n" && tokenizer[token] != " ") {
				v2 += tokenizer[token];
				token++;
			}

		};

		if (tokenizer[token] == "f") {
			std::string v1 = "";
			std::string v2 = "";
			std::string v3 = "";

			std::string vt1 = "";
			std::string vt2 = "";
			std::string vt3 = "";

			std::string vn1 = "";
			std::string vn2 = "";
			std::string vn3 = "";

			skipSpaces();
			token++;
			v1 += tokenizer[token];
			token+=2;
			vt1 += tokenizer[token];
			token+=2;
			vn1 += tokenizer[token];
			token++;
			skipSpaces();
			token--;
			v2 += tokenizer[token];
			token += 2;
			vt2 += tokenizer[token];
			token += 2;
			vn2 += tokenizer[token];
			token++;
			skipSpaces();
			token--;
			v3 += tokenizer[token];
			token += 2;
			vt3 += tokenizer[token];
			token += 2;
			vn3 += tokenizer[token];

			mesh->indicies.resize(mesh->indicies.size() + 3);
			mesh->indicies[index*3] = std::stoi(v1)-1;
			mesh->indicies[index*3 + 1] = std::stoi(v2)-1;
			mesh->indicies[index*3 + 2] = std::stoi(v3)-1;
			index++;
		};
	};
	tokenizer.tokens = {};
}

void OBJLoader::skipSpaces()
{
	while (tokenizer[token] == " ") {
		token++;
	};
	token++;
}
