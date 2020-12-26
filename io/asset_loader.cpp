#include "asset_loader.h"
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

std::vector<StaticMeshComponent> AssetLoader::loadModel(const std::string& filename)
{
	std::vector<StaticMeshComponent> staticMeshComponents;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename.c_str(),
		aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		throw std::runtime_error((boost::format("failed to load model：%s, error: %s") % filename.c_str() % importer.GetErrorString()).str());
	}

	processNode(scene->mRootNode, scene, filename, staticMeshComponents);

	return staticMeshComponents;
}

Texture AssetLoader::loadTexure(const std::string& filename)
{
	Texture texture;
	texture.data = stbi_load(filename.c_str(), &texture.width, &texture.height, &texture.channels, STBI_rgb_alpha);

	if (!texture.data)
	{
		throw std::runtime_error((boost::format("failed to load texture： %s") % filename.c_str()).str());
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

void AssetLoader::processNode(aiNode* node, const aiScene* scene, const std::string& filename, std::vector<StaticMeshComponent>& staticMeshComponents)
{
	for (uint32_t i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh, scene, filename, staticMeshComponents);
	}

	for (uint32_t i = 0; i < node->mNumChildren; ++i)
	{
		processNode(node->mChildren[i], scene, filename, staticMeshComponents);
	}
}

void AssetLoader::processMesh(aiMesh* mesh, const aiScene* scene, const std::string& filename, std::vector<StaticMeshComponent>& staticMeshComponents)
{
	StaticMeshComponent staticMeshComponent;

	// vertices
	staticMeshComponent.mesh.vertices.resize(mesh->mNumVertices);
	for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
	{
		staticMeshComponent.mesh.vertices[i].position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
		staticMeshComponent.mesh.vertices[i].texCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
		staticMeshComponent.mesh.vertices[i].normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
	}

	// indices
	for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
	{
		const aiFace& face = mesh->mFaces[i];
		for (uint32_t j = 0; j < face.mNumIndices; ++j)
		{
			staticMeshComponent.mesh.indices.push_back(face.mIndices[j]);
		}
	}

	// materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	boost::filesystem::path modelParentPath = boost::filesystem::path(filename).parent_path();

	// 1.diffuse maps
	for (uint32_t i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); ++i)
	{
		aiString aiStr;
		material->GetTexture(aiTextureType_DIFFUSE, i, &aiStr);
		boost::filesystem::path boostPath(aiStr.C_Str());
		std::string baseTexFilename = (modelParentPath / boost::filesystem::path("texture") / boostPath.filename()).string();
		staticMeshComponent.material.baseTex = loadTexure(baseTexFilename);
	}

	staticMeshComponents.push_back(staticMeshComponent);
}