#pragma once

#include <glm/glm.hpp>
#include <vector>

struct StaticVertex
{
	glm::vec3 position;
	glm::vec2 texCoord;
	glm::vec3 normal;
};

#define BONE_NUM_PER_VERTEX 4
struct SkinnedVertex
{
	glm::vec3 position;
	glm::vec2 texCoord;
	glm::vec3 normal;

	uint32_t BoneID[BONE_NUM_PER_VERTEX];
	float Weights[BONE_NUM_PER_VERTEX];
};

struct StaticMesh
{
	std::vector<StaticVertex> vertices;
	std::vector<uint32_t> indices;
};

struct SkinnedMesh
{
	std::vector<SkinnedVertex> vertices;
	std::vector<uint32_t> indices;
};