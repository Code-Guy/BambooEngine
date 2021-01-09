#include "asset_loader.h"
#include "utility/utility.h"

#include <fstream>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>

/* assimp和glm的转换函数 */
glm::vec3 assVectorToGlmVector(const aiVector3D& assVector)
{
	return glm::vec3(assVector.x, assVector.y, assVector.z);
}

glm::quat assQuatToGlmQuat(const aiQuaternion& assQuat)
{
	glm::quat q;
	q.w = assQuat.w;
	q.x = assQuat.x;
	q.y = assQuat.y;
	q.z = assQuat.z;
	return q;
}

glm::mat4 assMatToGlmMat(const aiMatrix4x4& assMat)
{
	glm::mat4 mat;
	mat[0][0] = assMat.a1;
	mat[1][0] = assMat.b1;
	mat[2][0] = assMat.c1;
	mat[3][0] = assMat.d1;

	mat[0][1] = assMat.a2;
	mat[1][1] = assMat.b2;
	mat[2][1] = assMat.c2;
	mat[3][1] = assMat.d2;

	mat[0][2] = assMat.a3;
	mat[1][2] = assMat.b3;
	mat[2][2] = assMat.c3;
	mat[3][2] = assMat.d3;

	mat[0][3] = assMat.a4;
	mat[1][3] = assMat.b4;
	mat[2][3] = assMat.c4;
	mat[3][3] = assMat.d4;

	return mat;
}

AssetLoader& AssetLoader::getInstance()
{
	static AssetLoader loader;
	return loader;
}

std::vector<char> AssetLoader::loadBinary(const std::string& filename)
{
	// std::ios::ate 从文件尾开始读，好处是可以获取到文件大小
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error((boost::format("failed to load shader：%s") % filename).str());
	}

	std::vector<char> buffer;
	size_t fileSize = (size_t)file.tellg();
	buffer.resize(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

void AssetLoader::loadModel(const std::string& filename, StaticMeshComponent& staticMeshComp, SkeletalMeshComponent& skeletalMeshComp, AnimatorComponent& animatorComp)
{
	Assimp::Importer importer;
	const aiScene* assScene = importer.ReadFile(filename.c_str(),
		aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

	if (!assScene || !assScene->mRootNode)
	{
		throw std::runtime_error((boost::format("failed to load model：%s, error: %s") % filename % importer.GetErrorString()).str());
	}

	if (assScene->mNumAnimations > 0)
	{
		processAnimation(assScene, animatorComp);
		return;
	}

	if (assScene->mNumMeshes == 0)
	{
		throw std::runtime_error((boost::format("model file %s doesn't have any mesh!") % filename).str());
	}

	if (assScene->mMeshes[0]->HasBones())
	{
		std::shared_ptr<Skeleton> skeleton = std::make_shared<Skeleton>();
		processSkeleton(assScene->mMeshes[0], skeleton);
		processBoneNode(assScene->mRootNode, skeleton);

		skeletalMeshComp.mesh = std::make_shared<SkeletalMesh>();
		skeletalMeshComp.skeleton = skeleton;
	}
	else
	{
		staticMeshComp.mesh = std::make_shared<StaticMesh>();
	}

	processMeshNode(assScene->mRootNode, assScene, filename, staticMeshComp, skeletalMeshComp);
}

std::shared_ptr<Texture> AssetLoader::loadTexure(const std::string& filename)
{
	std::shared_ptr<Texture> texture = std::make_shared<Texture>();
	texture->name = Utility::basename(filename);
	texture->data = stbi_load(filename.c_str(), &texture->width, &texture->height, &texture->channels, STBI_rgb_alpha);

	if (!texture->data)
	{
		throw std::runtime_error((boost::format("failed to load texture：%s") % filename).str());
	}

	return texture;
}

std::string AssetLoader::loadString(const std::string& filename)
{
	constexpr auto read_size = std::size_t{ 4096 };
	auto stream = std::ifstream{ filename.data() };
	stream.exceptions(std::ios_base::badbit);

	auto out = std::string{};
	auto buf = std::string(read_size, '\0');
	while (stream.read(&buf[0], read_size)) 
	{
		out.append(buf, 0, stream.gcount());
	}
	out.append(buf, 0, stream.gcount());
	return out;
}

void AssetLoader::processMeshNode(aiNode* assNode, const aiScene* assScene, const std::string& filename, 
	StaticMeshComponent& staticMeshComp, SkeletalMeshComponent& skeletalMeshComp)
{
	for (uint32_t i = 0; i < assNode->mNumMeshes; ++i)
	{
		aiMesh* assMesh = assScene->mMeshes[assNode->mMeshes[i]];
		if (!assMesh->HasBones())
		{
			auto& mesh = staticMeshComp.mesh;
			uint32_t baseIndex = static_cast<uint32_t>(mesh->vertices.size());
			processSection(assMesh, assScene, filename, baseIndex, mesh->indices, staticMeshComp.sections);
			processStaticVertices(assMesh, assScene, mesh->vertices);
		}
		else
		{
			auto& mesh = skeletalMeshComp.mesh;
			uint32_t baseIndex = static_cast<uint32_t>(mesh->vertices.size());
			processSection(assMesh, assScene, filename, baseIndex, mesh->indices, skeletalMeshComp.sections);
			processSkeletalVertices(assMesh, assScene, mesh->vertices);
		}
	}

	for (uint32_t i = 0; i < assNode->mNumChildren; ++i)
	{
		processMeshNode(assNode->mChildren[i], assScene, filename, staticMeshComp, skeletalMeshComp);
	}
}

void AssetLoader::processBoneNode(struct aiNode* assNode, std::shared_ptr<Skeleton>& skeleton)
{
	std::string parentName = assNode->mName.C_Str();
	auto& nameIndexMap = skeleton->nameIndexMap;
	if (nameIndexMap.find(parentName) != nameIndexMap.end())
	{
		Bone& parentBone = skeleton->bones[nameIndexMap[parentName]];
		for (uint32_t i = 0; i < assNode->mNumChildren; ++i)
		{
			std::string childName = assNode->mChildren[i]->mName.C_Str();
			if (nameIndexMap.find(childName) != nameIndexMap.end())
			{
				Bone& childBone = skeleton->bones[nameIndexMap[childName]];
				parentBone.children.push_back(&childBone);
				childBone.parent = &parentBone;
			}
		}
	}

	for (uint32_t i = 0; i < assNode->mNumChildren; ++i)
	{
		processBoneNode(assNode->mChildren[i], skeleton);
	}
}

void AssetLoader::processSkeleton(struct aiMesh* assMesh, std::shared_ptr<Skeleton>& skeleton)
{
	for (uint8_t i = 0; i < static_cast<uint8_t>(assMesh->mNumBones); ++i)
	{
		aiBone* assBone = assMesh->mBones[i];
		std::string name = assBone->mName.C_Str();
		skeleton->bones.push_back({ name, assMatToGlmMat(assBone->mOffsetMatrix) });
		skeleton->nameIndexMap[name] = i;
	}
}

void AssetLoader::processAnimation(const struct aiScene* assScene, AnimatorComponent& animatorComp)
{
	for (uint32_t i = 0; i < assScene->mNumAnimations; ++i)
	{
		aiAnimation* assAnimation = assScene->mAnimations[i];
		std::shared_ptr<Animation> animation = std::make_shared<Animation>();

		animation->name = assAnimation->mName.C_Str();
		animation->duration = static_cast<float>(assAnimation->mDuration);
		double ticksPerSecond = assAnimation->mTicksPerSecond > 0.0 ? assAnimation->mTicksPerSecond : 24.0;
		animation->step = 1.0f / static_cast<float>(ticksPerSecond);
		for (uint32_t j = 0; j < assAnimation->mNumChannels; ++j)
		{
			aiNodeAnim* channel = assAnimation->mChannels[j];
			std::string name = channel->mNodeName.C_Str();

			for (uint32_t k = 0; k < channel->mNumPositionKeys; ++k)
			{
				const aiVectorKey& key = channel->mPositionKeys[k];
				animation->positionKeys[name].push_back({ static_cast<float>(key.mTime), assVectorToGlmVector(key.mValue) });
			}
			for (uint32_t k = 0; k < channel->mNumRotationKeys; ++k)
			{
				const aiQuatKey& key = channel->mRotationKeys[k];
				animation->rotationKeys[name].push_back({ static_cast<float>(key.mTime), assQuatToGlmQuat(key.mValue) });
			}
			for (uint32_t k = 0; k < channel->mNumScalingKeys; ++k)
			{
				const aiVectorKey& key = channel->mScalingKeys[k];
				animation->scaleKeys[name].push_back({ static_cast<float>(key.mTime), assVectorToGlmVector(key.mValue) });
			}
		}

		animatorComp.animations[animation->name] = animation;
	}
}

void AssetLoader::processSection(struct aiMesh* assMesh, const struct aiScene* assScene, const std::string& filename, 
	uint32_t baseIndex, std::vector<uint32_t>& indices, std::vector<Section>& sections)
{
	sections.push_back(Section{});
	Section& section = sections.back();

	// indices
	for (uint32_t i = 0; i < assMesh->mNumFaces; ++i)
	{
		const aiFace& face = assMesh->mFaces[i];
		for (uint32_t j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(baseIndex + face.mIndices[j]);
		}
	}
	section.indexCount = static_cast<uint32_t>(indices.size());

	// materials
	std::shared_ptr<Material> material = std::make_shared<Material>();
	aiMaterial* assMaterial = assScene->mMaterials[assMesh->mMaterialIndex];
	boost::filesystem::path modelParentPath = boost::filesystem::path(filename).parent_path();

	// 1.diffuse maps
	for (uint32_t i = 0; i < assMaterial->GetTextureCount(aiTextureType_DIFFUSE); ++i)
	{
		aiString aiStr;
		assMaterial->GetTexture(aiTextureType_DIFFUSE, i, &aiStr);
		boost::filesystem::path boostPath(aiStr.C_Str());
		std::string baseTexFilename = (modelParentPath / boost::filesystem::path("texture") / boostPath.filename()).string();
		material->baseTex = loadTexure(baseTexFilename);
	}
	// 如果贴图不存在，使用默认贴图
	if (!material->baseTex)
	{
		material->baseTex = loadTexure("asset/texture/default_texture.jpg");
	}
	section.material = material;
}

void AssetLoader::processStaticVertices(struct aiMesh* assMesh, const struct aiScene* assScene, std::vector<StaticVertex>& staticVertices)
{
	// vertices
	for (uint32_t i = 0; i < assMesh->mNumVertices; ++i)
	{
		StaticVertex vertex;
		vertex.position = { assMesh->mVertices[i].x, assMesh->mVertices[i].y, assMesh->mVertices[i].z };
		vertex.texCoord = { assMesh->mTextureCoords[0][i].x, assMesh->mTextureCoords[0][i].y };
		vertex.normal = { assMesh->mNormals[i].x, assMesh->mNormals[i].y, assMesh->mNormals[i].z };

		staticVertices.push_back(vertex);
	}
}

void AssetLoader::processSkeletalVertices(struct aiMesh* assMesh, const struct aiScene* assScene, std::vector<SkeletalVertex>& skeletalVertices)
{
	// vertices
	for (uint32_t i = 0; i < assMesh->mNumVertices; ++i)
	{
		SkeletalVertex vertex;
		vertex.position = { assMesh->mVertices[i].x, assMesh->mVertices[i].y, assMesh->mVertices[i].z };
		vertex.texCoord = { assMesh->mTextureCoords[0][i].x, assMesh->mTextureCoords[0][i].y };
		vertex.normal = { assMesh->mNormals[i].x, assMesh->mNormals[i].y, assMesh->mNormals[i].z };

		// 初始化骨骼索引和权重列表
		for (uint32_t j = 0; j < 4; ++j)
		{
			vertex.bones[j] = INVALID_BONE;
			vertex.weights[j] = 0.0f;
		}

		skeletalVertices.push_back(vertex);
	}

	// 缓存每个顶点当前已经设置的骨骼数量
	std::vector<uint32_t> vertexBoneNums(skeletalVertices.size(), 0);
	for (uint8_t i = 0; i < static_cast<uint8_t>(assMesh->mNumBones); ++i)
	{
		aiBone* assBone = assMesh->mBones[i];

		for (uint32_t j = 0; j < assBone->mNumWeights; ++j)
		{
			aiVertexWeight& weight = assBone->mWeights[j];

			uint32_t vertexBoneIndex = vertexBoneNums[weight.mVertexId]++;
			skeletalVertices[weight.mVertexId].bones[vertexBoneIndex] = i;
			skeletalVertices[weight.mVertexId].weights[vertexBoneIndex] = weight.mWeight;
		}
	}
}
