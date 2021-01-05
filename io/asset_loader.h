#pragma once

#include <string>
#include <vector>
#include <tuple>

#include "component/static_mesh_component.h"

class AssetLoader
{
public:
	static AssetLoader& getInstance();

	std::vector<char> loadBinary(const std::string& filename);
	std::shared_ptr<StaticMeshComponent> loadModel(const std::string& filename);
	std::shared_ptr<Texture> loadTexure(const std::string& filename);

	std::vector<std::string> traverseFiles(const std::string& directory);

private:
	std::string loadString(const std::string& filename);

	void processNode(struct aiNode* assNode, const struct aiScene* assScene, const std::string& filename, std::shared_ptr<StaticMeshComponent>& staticMeshComponent);
	void processMesh(struct aiMesh* assMesh, const struct aiScene* assScene, const std::string& filename, std::shared_ptr<StaticMeshComponent>& staticMeshComponent);

	std::string basename(const std::string& filename);
};