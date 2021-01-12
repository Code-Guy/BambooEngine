#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "core/engine_type.h"

struct Bone
{
	std::string name;
	glm::mat4 globalInverseBindPoseMatrix;
	glm::mat4 localBindPoseMatrix;
	QuatTransform animatedTransform;

	Bone* parent = nullptr;
	std::vector<Bone*> children;

	void update();
	glm::mat4 matrix();

private:
	glm::mat4 globalBindPoseMatrix;
};

struct Skeleton
{
	std::vector<Bone> bones;
	std::map<std::string, uint8_t> nameIndexMap;

	bool hasBone(const std::string& name);
	Bone& getBone(const std::string& name);

	void update();

private:
	Bone invalidBone;
};

struct AnimKey
{
	float time;

	bool operator<(const AnimKey& other) const
	{
		return time < other.time;
	}
};

struct VectorKey : public AnimKey
{
	glm::vec3 value;
};

struct QuatKey : public AnimKey
{
	glm::quat value;
};

struct Animation
{
	std::string name;
	float duration;
	float frameRate;

	bool isCompatible(std::shared_ptr<Skeleton> skeleton);

	std::map<std::string, std::vector<VectorKey>> positionKeys;
	std::map<std::string, std::vector<QuatKey>> rotationKeys;
	std::map<std::string, std::vector<VectorKey>> scaleKeys;
};