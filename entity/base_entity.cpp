#include "base_entity.h"

BaseEntity::BaseEntity()
{
	m_parent = nullptr;

	m_position = glm::vec3(0.0f);
	m_rotation = glm::vec3(0.0f);
	m_scale = glm::vec3(1.0f);
}

BaseEntity::~BaseEntity()
{

}

void BaseEntity::tick(float deltaTime)
{
	for (const auto& iter : m_componentMap)
	{
		iter.second->tick(deltaTime);
	}
}

void BaseEntity::registerComponent(std::string name, BaseComponent* component)
{
	m_componentMap[name] = component;
}

void BaseEntity::unregisterComponent(std::string name)
{
	if (m_componentMap.find(name) != m_componentMap.end())
	{
		m_componentMap.erase(name);
	}
}
