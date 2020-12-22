#pragma once

#include <glm/glm.hpp>

struct Texture
{
	int width;
	int height;
	int channels;

	uint8_t* data = nullptr;
};

struct Material
{
	Texture baseTex;
	Texture metallicTex;
	Texture roughnessTex;
	Texture normalTex;
	Texture aoTex;

	glm::vec2 tiling = {1.0f, 1.0f};
	glm::vec2 offset = { 0.0f, 0.0f };
};