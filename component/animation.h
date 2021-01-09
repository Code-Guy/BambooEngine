#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>
#include <map>
#include <memory>

struct Bone
{
	std::string name;
	glm::mat4 localTransform;
	glm::mat4 globalTransform;

	Bone* parent = nullptr;
	std::vector<Bone*> children;
};

struct Skeleton
{
	std::vector<Bone> bones;
	std::map<std::string, uint8_t> nameIndexMap;
};

struct VectorKey
{
	float time;
	glm::vec3 value;
};

struct QuatKey
{
	float time;
	glm::quat value;
};

struct Animation
{
	std::string name;
	float duration;
	float step;

	bool isCompatible(std::shared_ptr<Skeleton> skeleton);

	std::map<std::string, std::vector<VectorKey>> positionKeys;
	std::map<std::string, std::vector<QuatKey>> rotationKeys;
	std::map<std::string, std::vector<VectorKey>> scaleKeys;
};