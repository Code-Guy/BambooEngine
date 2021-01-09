#include "animation.h"

bool Animation::isCompatible(std::shared_ptr<Skeleton> skeleton)
{
	for (auto& iter : positionKeys)
	{
		if (skeleton->nameIndexMap.find(iter.first) == skeleton->nameIndexMap.end())
		{
			return false;
		}
	}
	for (auto& iter : rotationKeys)
	{
		if (skeleton->nameIndexMap.find(iter.first) == skeleton->nameIndexMap.end())
		{
			return false;
		}
	}
	for (auto& iter : scaleKeys)
	{
		if (skeleton->nameIndexMap.find(iter.first) == skeleton->nameIndexMap.end())
		{
			return false;
		}
	}
	return true;
}