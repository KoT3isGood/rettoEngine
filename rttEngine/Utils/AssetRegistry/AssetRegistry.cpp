#include "AssetRegistry.h"
#define RTT_CURRENT_APP "asset"
AssetRegistry::AssetRegistry()
{
	std::filesystem::path meshesPath = "Content/Meshes";
	meshes["Content/Meshes/none.obj"] = 0;
	uint32_t meshesCount = 1;
	for (auto const mesh : std::filesystem::recursive_directory_iterator(meshesPath)) {
		if (mesh.is_directory()) {
			continue;
		}
		std::string meshPath = std::filesystem::path(mesh).generic_string();
		if (meshPath == "Content/Meshes/none.obj") {
			continue;
		}
		RTT_LOG(meshPath);
		meshes[meshPath] = meshesCount;
		meshesCount++;
	}
	std::filesystem::path texturesPath = "Content/Textures";
	uint32_t texturesCount = 0;
	for (auto const texture : std::filesystem::recursive_directory_iterator(texturesPath)) {
		if (texture.is_directory()) {
			continue;
		}
		std::string texturePath = std::filesystem::path(texture).generic_string();
		RTT_LOG(texturePath);
		textures[texturePath] = texturesCount;
		texturesCount++;
	}
}
