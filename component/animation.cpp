#include "animation.h"

void Bone::update()
{
	glm::mat4 localMatrix = animatedTransform.matrix();
	if (localMatrix == glm::mat4(1.0f))
	{
		localMatrix = localBindPoseMatrix;
	}

	if (parent)
	{
		globalBindPoseMatrix = parent->globalBindPoseMatrix * localMatrix;
	}
	else
	{
		globalBindPoseMatrix = localMatrix;
	}

	for (auto& iter : children)
	{
		iter->update();
	}
}

glm::mat4 Bone::matrix()
{
	return globalBindPoseMatrix * globalInverseBindPoseMatrix;
}

bool Animation::isCompatible(std::shared_ptr<Skeleton> skeleton)
{
	for (auto& iter : positionKeys)
	{
		if (!skeleton->hasBone(iter.first))
		{
			return false;
		}
	}
	for (auto& iter : rotationKeys)
	{
		if (!skeleton->hasBone(iter.first))
		{
			return false;
		}
	}
	for (auto& iter : scaleKeys)
	{
		if (!skeleton->hasBone(iter.first))
		{
			return false;
		}
	}
	return true;
}

bool Skeleton::hasBone(const std::string& name)
{
	return nameIndexMap.find(name) != nameIndexMap.end();
}

Bone& Skeleton::getBone(const std::string& name)
{
	if (!hasBone(name))
	{
		return invalidBone;
	}
	return bones[nameIndexMap[name]];
}

void Skeleton::update()
{
	if (!bones.empty())
	{
		bones.front().update();
	}
}
