#include "animation.h"

void Bone::update()
{
	glm::mat4 updatedLocalTransform = transform.matrix() * localTransform;
	if (parent)
	{
		globalTransform = parent->globalTransform * updatedLocalTransform;
	}
	else
	{
		globalTransform = updatedLocalTransform;
	}

	for (auto& iter : children)
	{
		iter->update();
	}
}

bool Animation::isCompatible(std::shared_ptr<Skeleton> skeleton)
{
	for (auto& iter : positionKeys)
	{
		if (!skeleton->hasBone(name))
		{
			return false;
		}
	}
	for (auto& iter : rotationKeys)
	{
		if (!skeleton->hasBone(name))
		{
			return false;
		}
	}
	for (auto& iter : scaleKeys)
	{
		if (!skeleton->hasBone(name))
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
