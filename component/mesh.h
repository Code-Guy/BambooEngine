#pragma once

#include <glm/glm.hpp>
#include <vector>

struct StaticVertex
{
	glm::vec3 position;
	glm::vec2 texCoord;
	glm::vec3 normal;
};

#define INVALID_BONE -1
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