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
			if (firstToken == "mtllib") {
				// Unsafe as fu
				std::string s;
				s.resize(64);
				sscanf_s(line.c_str(),"mtllib %s",s.data(), 64);
				uint32_t matNameLenght = 1;
				for (auto character : s) {
					if (character == '\0') {
						s.resize(matNameLenght);
					};
					matNameLenght++;
				}
				RTT_LOG("Loading Materials: " + s);
				std::string material = std::filesystem::current_path().string() + "/Content/Materials/" + s;
				std::ifstream readMaterial(material);
				std::string linemtl = "";
				std::string currentMaterial = "";
				uint32_t currentMaterialIndex = 0;
				if (readMaterial.good()) {
					while (std::getline(readMaterial, linemtl)) {
						
						std::string firstTokenMaterial = "";
						int tokenSizemtl = 0;
						while (tokenSizemtl < linemtl.size()) {
							if (!std::isspace(linemtl[tokenSizemtl])) {
								firstTokenMaterial += linemtl[tokenSizemtl];
								tokenSizemtl += 1;
							}
							else {
								break;
							}
						}
						if (firstTokenMaterial == "#") {
							continue;
						}
						if (firstTokenMaterial == "newmtl") {
							std::string sm;
							sm.resize(64);
							sscanf_s(linemtl.c_str(), "newmtl %s", sm.data(), 64);
							uint32_t materialNameLenght = 1;
							for (auto character : sm) {
								if (character == '\0') {
									sm.resize(materialNameLenght - 1);
									break;
								};
								materialNameLenght++;
								}
							mesh->materials[sm] = currentMaterialIndex;
							mesh->materialsArray[sm] = Material();
							currentMaterial = sm;
							currentMaterialIndex++;
						}
						if (firstTokenMaterial == "Ka") {
							float x=0, y=0, z=0;
							sscanf_s(linemtl.c_str(), "Ka %f %f %f", &x, &y, &z);
							Material* currentMat = &mesh->materialsArray[currentMaterial];
							currentMat->albedo[0] = x;
							currentMat->albedo[1] = y;
							currentMat->albedo[2] = z;
						}
						if (firstTokenMaterial == "map_Ka") {
							std::string sm;
							sm.resize(64);
							sscanf_s(linemtl.c_str(), "map_Ka %s", sm.data(), 64);
							uint32_t materialNameLenght = 1;
							for (auto character : sm) {
								if (character == '\0') {
									sm.resize(materialNameLenght-1);
									break;
								};
								materialNameLenght++;
							}
							sm = "Content/Textures/" + sm;
							Material* currentMat = &mesh->materialsArray[currentMaterial];
							AssetRegistry* ar = &GetProcessInfo()->assetRegistry;
							currentMat->albedoTexture = GetProcessInfo()->assetRegistry.textures[sm];
						}
					}
				}
				readMaterial.close();
			}
			if (firstToken == "usemtl") {
				std::string sm;
				sm.resize(64);
				sscanf_s(line.c_str(), "usemtl %s", sm.data(), 64);
				uint32_t materialNameLenght = 1;
				for (auto character : sm) {
					if (character == '\0') {
						sm.resize(materialNameLenght - 1);
						break;
					};
					materialNameLenght++;
				}
				currentMaterial = mesh->materials[sm];
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
				size_t size = mesh->uvs.size();
				mesh->uvs.resize(size + 2);
				mesh->uvs[size] = x;
				mesh->uvs[size + 1] = y;
			}
			if (firstToken == "vn") {
				float x, y, z;
				sscanf_s(line.c_str(), "vn %f %f %f", &x, &y, &z);
			}
			// TODO: Refactor
			if (hasUv == true) {
				
				if (firstToken == "f") {
					uint32_t v0, v1, v2, vt0 = -1, vt1, vt2, vn0, vn1, vn2;
					sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &v0, &vt0, &vn0, &v1, &vt1, &vn1, &v2, &vt2, &vn2);
					if (vt0 == -1) {
						sscanf_s(line.c_str(), "f %d//%d %d//%d %d//%d", &v0, &vn0, &v1, &vn1, &v2, &vn2);
						hasUv = false;
					}
					else {
						size_t size = mesh->uvIndicies.size();
						mesh->uvIndicies.resize(size + 3);
						mesh->uvIndicies[size] = vt0 - 1;
						mesh->uvIndicies[size + 1] = vt1 - 1;
						mesh->uvIndicies[size + 2] = vt2 - 1;
					}
					size_t size = mesh->indicies.size();
					mesh->indicies.resize(size + 3);
					mesh->indicies[size] = v0 - 1;
					mesh->indicies[size + 1] = v1 - 1;
					mesh->indicies[size + 2] = v2 - 1;

					size_t size2 = mesh->materialSizes.size();
					mesh->materialSizes.resize(size2 + 1);
					mesh->materialSizes[size2] = currentMaterial;
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

					size_t size2 = mesh->materialSizes.size();
					mesh->materialSizes.resize(size2 + 1);
					mesh->materialSizes[size2] = currentMaterial;
				}
			};

			// Vertices
		}
		MyReadFile.close();
		return;
	}
}
