#include "ObjLoader.h"

OBJLoader::OBJLoader(std::string file, MeshData* mesh)
{

	RTT_LOG(std::filesystem::current_path().string() + "/" + file);

	std::ifstream MyReadFile(file);

	if (MyReadFile.good()) {
		std::string line = "";
		while (std::getline(MyReadFile, line)) {
			// Output the text from the file


			std::string firstToken = "";
			int tokenSize = 0;
			while (tokenSize < line.size()) {
				if (!std::isspace(line[tokenSize])) {
					firstToken += line[tokenSize];
					tokenSize += 1;
				}
				else {
					break;
				}
			}

			// Comments
			if (firstToken == "#") {
				continue;
			}
			if (firstToken == "v") {
				float x, y, z;
				sscanf_s(line.c_str(), "v %f %f %f", &x, &y, &z);
				size_t size = mesh->vertices.size();
				mesh->vertices.resize(size + 3);
				mesh->vertices[size] = x;
				mesh->vertices[size + 1] = y;
				mesh->vertices[size + 2] = z;
			}
			if (firstToken == "vt") {
				float x, y;
				sscanf_s(line.c_str(), "vt %f %f", &x, &y);
			}
			if (firstToken == "vn") {
				float x, y, z;
				sscanf_s(line.c_str(), "vn %f %f %f", &x, &y, &z);
			}
			if (hasUv == true) {
				if (firstToken == "f") {
					uint32_t v0, v1, v2, vt0 = -1, vt1, vt2, vn0, vn1, vn2;
					sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &v0, &vt0, &vn0, &v1, &vt1, &vn1, &v2, &vt2, &vn2);
					if (vt0 == -1) {
						sscanf_s(line.c_str(), "f %d//%d %d//%d %d//%d", &v0, &vn0, &v1, &vn1, &v2, &vn2);
						hasUv = false;
					}
					size_t size = mesh->indicies.size();
					mesh->indicies.resize(size + 3);
					mesh->indicies[size] = v0 - 1;
					mesh->indicies[size + 1] = v1 - 1;
					mesh->indicies[size + 2] = v2 - 1;
				}
			}
			else {
				if (firstToken == "f") {
					uint32_t v0, v1, v2, vn0, vn1, vn2;
					sscanf_s(line.c_str(), "f %d//%d %d//%d %d//%d", &v0, &vn0, &v1, &vn1, &v2, &vn2);
					size_t size = mesh->indicies.size();
					mesh->indicies.resize(size + 3);
					mesh->indicies[size] = v0 - 1;
					mesh->indicies[size + 1] = v1 - 1;
					mesh->indicies[size + 2] = v2 - 1;
				}
			};

			// Vertices
		}
		MyReadFile.close();
		return;
	}
}
