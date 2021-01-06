#include "entity.h"

Entity::Entity()
{
	m_position = glm::vec3(0.0f);
	m_rotation = glm::vec3(0.0f);
	m_scale = glm::vec3(1.0f);
}

Entity::~Entity()
{

}

void Entity::pre()
{
	for (const auto& iter : m_componentMap)
	{
		iter.second->pre();
	}
}

void Entity::begin()
{
	for (const auto& iter : m_componentMap)
	{
		iter.second->begin();
	}
}

void Entity::tick(float deltaTime)
{
	for (const auto& iter : m_componentMap)
	{
		iter.second->tick(deltaTime);
	}
}

void Entity::end()
{
	for (const auto& iter : m_componentMap)
	{
		iter.second->end();
	}
}

void Entity::post()
{
	for (const auto& iter : m_componentMap)
	{
		iter.second->post();
	}
}

void Entity::registerComponent(std::string name, Component* component)
{
	m_componentMap[name] = component;
}

void Entity::unregisterComponent(std::string name)
{
	if (m_componentMap.find(name) != m_componentMap.end())
	{
		m_componentMap.erase(name);
	}
}
