#pragma once

#include <string>
#include <vector>
#include <tuple>

#include "component/mesh_component.h"

class AssetLoader
{
public:
	static AssetLoader& getInstance();

	std::vector<char> loadBinary(const std::string& filename);
	std::vector<StaticMeshComponent> loadModel(const std::string& filename);
	Texture loadTexure(const std::string& filename);

private:
	std::string loadString(const std::string& filename);

	void processNode(struct aiNode* node, const struct aiScene* scene, const std::string& filename, std::vector<StaticMeshComponent>& staticMeshComponents);
	void processMesh(struct aiMesh* mesh, const struct aiScene* scene, const std::string& filename, std::vector<StaticMeshComponent>& staticMeshComponents);
};