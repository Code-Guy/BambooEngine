#pragma once

#include <string>
#include <vector>
#include <tuple>

#include "component/mesh_component.h"

class AssetLoader
{
public:
	static AssetLoader& getInstance();

	void loadBinary(const std::string& filename, std::vector<char>& buffer);
	void loadModel(const std::string& filename, std::vector<StaticMeshComponent>& staticMeshComponents);
	void loadTexure(const std::string& filename, Texture& texture);

private:
	void processNode(struct aiNode* node, const struct aiScene* scene, const std::string& filename, std::vector<StaticMeshComponent>& staticMeshComponents);
	void processMesh(struct aiMesh* mesh, const struct aiScene* scene, const std::string& filename, std::vector<StaticMeshComponent>& staticMeshComponents);
};