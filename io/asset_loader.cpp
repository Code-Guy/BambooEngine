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

void AssetLoader::loadModel(const std::string& filename, StaticMeshComponent& staticMeshComponent, SkeletalMeshComponent& skeletalMeshComponent)
{
	Assimp::Importer importer;
	const aiScene* assScene = importer.ReadFile(filename.c_str(),
		aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

	if (!assScene || assScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !assScene->mRootNode)
	{
		throw std::runtime_error((boost::format("failed to load model：%s, error: %s") % filename % importer.GetErrorString()).str());
	}

	if (!assScene->mMeshes[0]->HasBones())
	{
		staticMeshComponent.mesh = std::make_shared<StaticMesh>();
	}
	else
	{
		skeletalMeshComponent.mesh = std::make_shared<SkeletalMesh>();
	}
	processNode(assScene->mRootNode, assScene, filename, staticMeshComponent, skeletalMeshComponent);
}

void AssetLoader::loadAnimation(const std::string& filename)
{
	Assimp::Importer importer;
	const aiScene* assScene = importer.ReadFile(filename.c_str(),
		aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

	if (!assScene || assScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !assScene->mRootNode)
	{
		throw std::runtime_error((boost::format("failed to load model：%s, error: %s") % filename % importer.GetErrorString()).str());
	}

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

void AssetLoader::processNode(aiNode* assNode, const aiScene* assScene, const std::string& filename, StaticMeshComponent& staticMeshComponent, SkeletalMeshComponent& skeletalMeshComponent)
{
	for (uint32_t i = 0; i < assNode->mNumMeshes; ++i)
	{
		aiMesh* assMesh = assScene->mMeshes[assNode->mMeshes[i]];
		if (!assMesh->HasBones())
		{
			auto& mesh = staticMeshComponent.mesh;
			uint32_t baseIndex = static_cast<uint32_t>(mesh->vertices.size());
			processSection(assMesh, assScene, filename, baseIndex, mesh->indices, staticMeshComponent.sections);
			processStaticVertices(assMesh, assScene, mesh->vertices);
		}
		else
		{
			auto& mesh = skeletalMeshComponent.mesh;
			uint32_t baseIndex = static_cast<uint32_t>(mesh->vertices.size());
			processSection(assMesh, assScene, filename, baseIndex, mesh->indices, skeletalMeshComponent.sections);
			processSkeletalVertices(assMesh, assScene, mesh->vertices);
		}
	}

	for (uint32_t i = 0; i < assNode->mNumChildren; ++i)
	{
		processNode(assNode->mChildren[i], assScene, filename, staticMeshComponent, skeletalMeshComponent);
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
