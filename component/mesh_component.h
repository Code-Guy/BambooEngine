#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "material.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec2 texCoord;
	glm::vec3 normal;
};

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};

struct StaticMeshComponent
{
	Mesh mesh;
	Material material;
};

struct SkinnedMeshComponent
{
	Mesh mesh;
	Material material;
};