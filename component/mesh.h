#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

struct StaticVertex
{
	glm::vec3 position;
	glm::vec2 texCoord;
	glm::vec3 normal;
};

struct SkeletalVertex : public StaticVertex
{
	glm::ivec4 bones;
	glm::vec4 weights;
};

struct StaticMesh
{
	std::vector<StaticVertex> vertices;
	std::vector<uint32_t> indices;
};

struct SkeletalMesh
{
	std::vector<SkeletalVertex> vertices;
	std::vector<uint32_t> indices;
};