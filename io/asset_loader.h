#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <boost/format.hpp>

#include "component/component.h"

class AssetLoader
{
public:
	static AssetLoader& getInstance();

	std::vector<char> loadBinary(const std::string& filename);
	void loadModel(const std::string& filename, StaticMeshComponent& staticMeshComp, SkeletalMeshComponent& skeletalMeshComp, AnimatorComponent& animatorComp);
	std::shared_ptr<Texture> loadTexure(const std::string& filename);

private:
	std::string loadString(const std::string& filename);

	void processMeshNode(struct aiNode* assNode, const struct aiScene* assScene, const std::string& filename, 
		StaticMeshComponent& staticMeshComp, SkeletalMeshComponent& skeletalMeshComp);
	void processBoneNode(struct aiNode* assNode, std::shared_ptr<Skeleton>& skeleton);
	void processSkeleton(struct aiMesh* assMesh, std::shared_ptr<Skeleton>& skeleton);
	void processAnimation(const struct aiScene* assScene, const std::string& filename, AnimatorComponent& animatorComp);

	void processSection(struct aiMesh* assMesh, const struct aiScene* assScene, const std::string& filename, 
		uint32_t baseIndex, std::vector<uint32_t>& indices, std::vector<Section>& sections);
	void processStaticVertices(struct aiMesh* assMesh, const struct aiScene* assScene, std::vector<StaticVertex>& staticVertices);
	void processSkeletalVertices(struct aiMesh* assMesh, const struct aiScene* assScene, std::vector<SkeletalVertex>& skeletalVertices);
};