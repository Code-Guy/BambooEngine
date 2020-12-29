#pragma once

#include "component/base_component.h"

#include <glm/glm.hpp>
#include <map>
#include <string>

class BaseEntity
{
public:
	BaseEntity();
	virtual ~BaseEntity();

	virtual void tick(float deltaTime);

protected:
	void registerComponent(std::string name, BaseComponent* component);
	void unregisterComponent(std::string name);

private:
	BaseEntity* m_parent;

	glm::vec3 m_position;
	glm::vec3 m_rotation;
	glm::vec3 m_scale;

	std::map<std::string, BaseComponent*> m_componentMap;
};