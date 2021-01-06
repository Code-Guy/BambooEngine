#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>

struct Texture
{
	std::string name;
	int width;
	int height;
	int channels;

	uint8_t* data = nullptr;

	~Texture()
	{
		if (data)
		{
			free(data);
		}
	}
};

struct Material
{
	std::shared_ptr<Texture> baseTex;
	std::shared_ptr<Texture> metallicTex;
	std::shared_ptr<Texture> roughnessTex;
	std::shared_ptr<Texture> normalTex;
	std::shared_ptr<Texture> aoTex;

	glm::vec2 tiling = {1.0f, 1.0f};
	glm::vec2 offset = { 0.0f, 0.0f };
};