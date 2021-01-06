#pragma once

#include "component/component.h"

#include <glm/glm.hpp>
#include <map>
#include <string>
#include <memory>

class Entity
{
public:
	Entity();
	virtual ~Entity();

	virtual void pre();
	virtual void begin();
	virtual void tick(float deltaTime);
	virtual void end();
	virtual void post();

protected:
	void registerComponent(std::string name, Component* component);
	void unregisterComponent(std::string name);

private:
	std::weak_ptr<Entity> m_parent;

	glm::vec3 m_position;
	glm::vec3 m_rotation;
	glm::vec3 m_scale;

	std::map<std::string, Component*> m_componentMap;
};