#pragma once

#include <string>
#include <vector>
#include <tuple>

#include "component/component.h"

class AssetLoader
{
public:
	static AssetLoader& getInstance();

	std::vector<char> loadBinary(const std::string& filename);
	void loadModel(const std::string& filename, StaticMeshComponent& staticMeshComponent, SkeletalMeshComponent& skeletalMeshComponent);
	void loadAnimation(const std::string& filename);
	std::shared_ptr<Texture> loadTexure(const std::string& filename);

private:
	std::string loadString(const std::string& filename);

	void processNode(struct aiNode* assNode, const struct aiScene* assScene, const std::string& filename,
		StaticMeshComponent& staticMeshComponent, SkeletalMeshComponent& skeletalMeshComponent);
	void processSection(struct aiMesh* assMesh, const struct aiScene* assScene, const std::string& filename, 
		uint32_t baseIndex, std::vector<uint32_t>& indices, std::vector<Section>& sections);
	void processStaticVertices(struct aiMesh* assMesh, const struct aiScene* assScene, std::vector<StaticVertex>& staticVertices);
	void processSkeletalVertices(struct aiMesh* assMesh, const struct aiScene* assScene, std::vector<SkeletalVertex>& skeletalVertices);
};